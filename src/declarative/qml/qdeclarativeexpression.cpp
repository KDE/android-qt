/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativeexpression.h"
#include "qdeclarativeexpression_p.h"

#include "qdeclarativeengine_p.h"
#include "qdeclarativecontext_p.h"
#include "qdeclarativerewrite_p.h"
#include "qdeclarativecompiler_p.h"

#include <QtCore/qdebug.h>
#include <QtScript/qscriptprogram.h>

#include <private/qscriptdeclarativeclass_p.h>

QT_BEGIN_NAMESPACE

bool QDeclarativeDelayedError::addError(QDeclarativeEnginePrivate *e)
{
    if (!e || prevError) return false;

    if (e->inProgressCreations == 0) return false; // Not in construction

    prevError = &e->erroredBindings;
    nextError = e->erroredBindings;
    e->erroredBindings = this;
    if (nextError) nextError->prevError = &nextError;

    return true;
}

QDeclarativeExpressionData::QDeclarativeExpressionData()
: q(0), dataRef(0), expressionFunctionValid(false), expressionRewritten(false), me(0), 
  trackChange(false), isShared(false), line(-1), guardList(0), guardListLength(0)
{
}

QDeclarativeExpressionData::~QDeclarativeExpressionData()
{
    if (guardList) { delete [] guardList; guardList = 0; }
    if (dataRef) dataRef->release();
}

QDeclarativeExpressionPrivate::QDeclarativeExpressionPrivate()
: data(new QDeclarativeExpressionData)
{
    data->q = this;
}

QDeclarativeExpressionPrivate::QDeclarativeExpressionPrivate(QDeclarativeExpressionData *d)
: data(d)
{
    data->q = this;
}

QDeclarativeExpressionPrivate::~QDeclarativeExpressionPrivate()
{
    if (data) { data->q = 0; data->release(); data = 0; }
}

void QDeclarativeExpressionPrivate::init(QDeclarativeContext *ctxt, const QString &expr, 
                                QObject *me)
{
    data->expression = expr;

    data->QDeclarativeAbstractExpression::setContext(ctxt);
    data->me = me;
}

void QDeclarativeExpressionPrivate::init(QDeclarativeContext *ctxt, void *expr, QDeclarativeRefCount *rc, 
                                QObject *me, const QString &url, int lineNumber)
{
    data->url = url;
    data->line = lineNumber;

    if (data->dataRef) data->dataRef->release();
    data->dataRef = rc;
    if (data->dataRef) data->dataRef->addref();

    quint32 *exprData = (quint32 *)expr;
    QDeclarativeCompiledData *dd = (QDeclarativeCompiledData *)rc;

    data->expressionRewritten = true;
    data->expression = QString::fromRawData((QChar *)(exprData + 2), exprData[1]);

    int progIdx = *(exprData);
    bool isShared = progIdx & 0x80000000;
    progIdx &= 0x7FFFFFFF;

    QDeclarativeEngine *engine = ctxt->engine();
    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(engine);
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    if (isShared) {

        if (!dd->cachedClosures.at(progIdx)) {
            QScriptContext *scriptContext = QScriptDeclarativeClass::pushCleanContext(scriptEngine);
            scriptContext->pushScope(ep->contextClass->newSharedContext());
            dd->cachedClosures[progIdx] = new QScriptValue(scriptEngine->evaluate(data->expression, data->url, data->line));
            scriptEngine->popContext();
        }

        data->expressionFunction = *dd->cachedClosures.at(progIdx);
        data->isShared = true;
        data->expressionFunctionValid = true;

    } else {

#if !defined(Q_OS_SYMBIAN) //XXX Why doesn't this work?
        if (!dd->cachedPrograms.at(progIdx)) {
            dd->cachedPrograms[progIdx] =
                new QScriptProgram(data->expression, data->url, data->line);
        }

        data->expressionFunction = evalInObjectScope(ctxt, me, *dd->cachedPrograms.at(progIdx));
#else
        data->expressionFunction = evalInObjectScope(ctxt, me, data->expression);
#endif

        data->expressionFunctionValid = true;
    }

    data->QDeclarativeAbstractExpression::setContext(ctxt);
    data->me = me;
}

QScriptValue QDeclarativeExpressionPrivate::evalInObjectScope(QDeclarativeContext *context, QObject *object, 
                                                     const QString &program)
{
    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(context->engine());
    QScriptContext *scriptContext = QScriptDeclarativeClass::pushCleanContext(&ep->scriptEngine);
    scriptContext->pushScope(ep->contextClass->newContext(context, object));
    QScriptValue rv = ep->scriptEngine.evaluate(program);
    ep->scriptEngine.popContext();
    return rv;
}

QScriptValue QDeclarativeExpressionPrivate::evalInObjectScope(QDeclarativeContext *context, QObject *object, 
                                                     const QScriptProgram &program)
{
    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(context->engine());
    QScriptContext *scriptContext = QScriptDeclarativeClass::pushCleanContext(&ep->scriptEngine);
    scriptContext->pushScope(ep->contextClass->newContext(context, object));
    QScriptValue rv = ep->scriptEngine.evaluate(program);
    ep->scriptEngine.popContext();
    return rv;
}

/*!
    \class QDeclarativeExpression
  \since 4.7
    \brief The QDeclarativeExpression class evaluates JavaScript in a QML context.
*/

/*!
    Create an invalid QDeclarativeExpression.

    As the expression will not have an associated QDeclarativeContext, this will be a
    null expression object and its value will always be an invalid QVariant.
 */
QDeclarativeExpression::QDeclarativeExpression()
: QObject(*new QDeclarativeExpressionPrivate, 0)
{
}

/*!  \internal */
QDeclarativeExpression::QDeclarativeExpression(QDeclarativeContext *ctxt, void *expr,
                             QDeclarativeRefCount *rc, QObject *me, 
                             const QString &url, int lineNumber,
                             QDeclarativeExpressionPrivate &dd)
: QObject(dd, 0)
{
    Q_D(QDeclarativeExpression);
    d->init(ctxt, expr, rc, me, url, lineNumber);
}

/*!
    Create a QDeclarativeExpression object.

    The \a expression JavaScript will be executed in the \a ctxt QDeclarativeContext.
    If specified, the \a scope object's properties will also be in scope during
    the expression's execution.
*/
QDeclarativeExpression::QDeclarativeExpression(QDeclarativeContext *ctxt, const QString &expression,
                             QObject *scope)
: QObject(*new QDeclarativeExpressionPrivate, 0)
{
    Q_D(QDeclarativeExpression);
    d->init(ctxt, expression, scope);
}

/*!  \internal */
QDeclarativeExpression::QDeclarativeExpression(QDeclarativeContext *ctxt, const QString &expression,
                             QObject *scope, QDeclarativeExpressionPrivate &dd)
: QObject(dd, 0)
{
    Q_D(QDeclarativeExpression);
    d->init(ctxt, expression, scope);
}

/*!
    Destroy the QDeclarativeExpression instance.
*/
QDeclarativeExpression::~QDeclarativeExpression()
{
}

/*!
    Returns the QDeclarativeEngine this expression is associated with, or 0 if there
    is no association or the QDeclarativeEngine has been destroyed.
*/
QDeclarativeEngine *QDeclarativeExpression::engine() const
{
    Q_D(const QDeclarativeExpression);
    return d->data->context()?d->data->context()->engine():0;
}

/*!
    Returns the QDeclarativeContext this expression is associated with, or 0 if there
    is no association or the QDeclarativeContext has been destroyed.
*/
QDeclarativeContext *QDeclarativeExpression::context() const
{
    Q_D(const QDeclarativeExpression);
    return d->data->context();
}

/*!
    Returns the expression string.
*/
QString QDeclarativeExpression::expression() const
{
    Q_D(const QDeclarativeExpression);
    return d->data->expression;
}

/*!
    Set the expression to \a expression.
*/
void QDeclarativeExpression::setExpression(const QString &expression)
{
    Q_D(QDeclarativeExpression);

    d->clearGuards();

    d->data->expression = expression;
    d->data->expressionFunctionValid = false;
    d->data->expressionRewritten = false;
    d->data->expressionFunction = QScriptValue();
}

void QDeclarativeExpressionPrivate::exceptionToError(QScriptEngine *scriptEngine, 
                                            QDeclarativeError &error)
{
    if (scriptEngine->hasUncaughtException() && 
        scriptEngine->uncaughtException().isError()) {

        QString fileName;
        int lineNumber = scriptEngine->uncaughtExceptionLineNumber();

        QScriptValue exception = scriptEngine->uncaughtException();
        QLatin1String fileNameProp("fileName");

        if (!exception.property(fileNameProp).toString().isEmpty()){
            fileName = exception.property(fileNameProp).toString();
        } else {
            fileName = QLatin1String("<Unknown File>");
        }

        error.setUrl(QUrl(fileName));
        error.setLine(lineNumber);
        error.setColumn(-1);
        error.setDescription(exception.toString());
    } else {
        error = QDeclarativeError();
    }
}

QVariant QDeclarativeExpressionPrivate::evalQtScript(QObject *secondaryScope, bool *isUndefined)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QDeclarativePerfTimer<QDeclarativePerf::BindValueQt> perfqt;
#endif

    QDeclarativeExpressionData *data = this->data;
    QDeclarativeContextPrivate *ctxtPriv = data->context()->d_func();
    QDeclarativeEngine *engine = data->context()->engine();
    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(engine);

    if (secondaryScope)
       ctxtPriv->defaultObjects.append(secondaryScope);

    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    if (!data->expressionFunctionValid) {

        QScriptContext *scriptContext = QScriptDeclarativeClass::pushCleanContext(scriptEngine);
        scriptContext->pushScope(ep->contextClass->newContext(data->context(), data->me));

        if (data->expressionRewritten) {
            data->expressionFunction = scriptEngine->evaluate(data->expression, 
                                                              data->url, data->line);
        } else {
            QDeclarativeRewrite::RewriteBinding rewriteBinding;

            bool ok = true;
            const QString code = rewriteBinding(data->expression, &ok);
            if (!ok) {
                scriptEngine->popContext();
                return QVariant();
            }
            data->expressionFunction = scriptEngine->evaluate(code, data->url, data->line);
        }

        scriptEngine->popContext();
        data->expressionFunctionValid = true;
    }

    QDeclarativeContext *oldSharedContext = 0;
    QObject *oldSharedScope = 0;
    if (data->isShared) {
        oldSharedContext = ep->sharedContext;
        oldSharedScope = ep->sharedScope;
        ep->sharedContext = data->context();
        ep->sharedScope = data->me;
    }

    QScriptValue svalue = data->expressionFunction.call();

    if (data->isShared) {
        ep->sharedContext = oldSharedContext;
        ep->sharedScope = oldSharedScope;
    }

    if (isUndefined)
        *isUndefined = svalue.isUndefined() || scriptEngine->hasUncaughtException();

    // Handle exception
    if (scriptEngine->hasUncaughtException()) {
       exceptionToError(scriptEngine, data->error);
       scriptEngine->clearExceptions();
       return QVariant();
    } else {
        data->error = QDeclarativeError();
    }

    if (secondaryScope) {
        QObject *last = ctxtPriv->defaultObjects.takeLast();
        Q_ASSERT(last == secondaryScope);
        Q_UNUSED(last);
    }

    QVariant rv;

    if (svalue.isArray()) {
        int length = svalue.property(QLatin1String("length")).toInt32();
        if (length && svalue.property(0).isObject()) {
            QList<QObject *> list;
            for (int ii = 0; ii < length; ++ii) {
                QScriptValue arrayItem = svalue.property(ii);
                QObject *d = arrayItem.toQObject();
                list << d;
            }
            rv = QVariant::fromValue(list);
        }
    } else if (svalue.isObject() &&
               ep->objectClass->scriptClass(svalue) == ep->objectClass) {
        QObject *o = svalue.toQObject();
        int type = QMetaType::QObjectStar;
        // If the object is null, we extract the predicted type.  While this isn't
        // 100% reliable, in many cases it gives us better error messages if we
        // assign this null-object to an incompatible property
        if (!o) type = ep->objectClass->objectType(svalue);

        return QVariant(type, &o);
    }

    if (rv.isNull())
        rv = svalue.toVariant();

    return rv;
}

QVariant QDeclarativeExpressionPrivate::value(QObject *secondaryScope, bool *isUndefined)
{
    Q_Q(QDeclarativeExpression);

    QVariant rv;
    if (!q->engine()) {
        qWarning("QDeclarativeExpression: Attempted to evaluate an expression in an invalid context");
        return rv;
    }

    if (data->expression.isEmpty())
        return rv;

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QDeclarativePerfTimer<QDeclarativePerf::BindValue> perf;
#endif

    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(q->engine());

    QDeclarativeExpression *lastCurrentExpression = ep->currentExpression;
    bool lastCaptureProperties = ep->captureProperties;
    QPODVector<QDeclarativeEnginePrivate::CapturedProperty> lastCapturedProperties;
    ep->capturedProperties.copyAndClear(lastCapturedProperties);

    ep->currentExpression = q;
    ep->captureProperties = data->trackChange;

    // This object might be deleted during the eval
    QDeclarativeExpressionData *localData = data;
    localData->addref();

    rv = evalQtScript(secondaryScope, isUndefined);

    ep->currentExpression = lastCurrentExpression;
    ep->captureProperties = lastCaptureProperties;

    // Check if we were deleted
    if (localData->q) {
        if ((!data->trackChange || !ep->capturedProperties.count()) && data->guardList) {
            clearGuards();
        } else if(data->trackChange) {
            updateGuards(ep->capturedProperties);
        }
    }

    localData->release();

    lastCapturedProperties.copyAndClear(ep->capturedProperties);

    return rv;
}

/*!
    Returns the value of the expression, or an invalid QVariant if the
    expression is invalid or has an error.

    \a isUndefined is set to true if the expression resulted in an
    undefined value.

    \sa hasError(), error()
*/
QVariant QDeclarativeExpression::value(bool *isUndefined)
{
    Q_D(QDeclarativeExpression);
    return d->value(0, isUndefined);
}

/*!
Returns true if the valueChanged() signal is emitted when the expression's evaluated
value changes.
*/
bool QDeclarativeExpression::notifyOnValueChanged() const
{
    Q_D(const QDeclarativeExpression);
    return d->data->trackChange;
}

/*!
Sets whether the valueChanged() signal is emitted when the expression's evaluated
value changes.

If true, the QDeclarativeExpression will monitor properties involved in the expression's 
evaluation, and emit QDeclarativeExpression::valueChanged() if they have changed.  This allows 
an application to ensure that any value associated with the result of the expression 
remains up to date.

If false, the QDeclarativeExpression will not montitor properties involved in the expression's 
evaluation, and QDeclarativeExpression::valueChanged() will never be emitted.  This is more efficient 
if an application wants a "one off" evaluation of the expression.

By default, notifyOnChange is false.
*/
void QDeclarativeExpression::setNotifyOnValueChanged(bool notifyOnChange)
{
    Q_D(QDeclarativeExpression);
    d->data->trackChange = notifyOnChange;
}

/*!
    Returns the source file URL for this expression.  The source location must
    have been previously set by calling setSourceLocation().
*/
QString QDeclarativeExpression::sourceFile() const
{
    Q_D(const QDeclarativeExpression);
    return d->data->url;
}

/*!
    Returns the source file line number for this expression.  The source location 
    must have been previously set by calling setSourceLocation().
*/
int QDeclarativeExpression::lineNumber() const
{
    Q_D(const QDeclarativeExpression);
    return d->data->line;
}

/*!
    Set the location of this expression to \a line of \a url. This information
    is used by the script engine.
*/
void QDeclarativeExpression::setSourceLocation(const QString &url, int line)
{
    Q_D(QDeclarativeExpression);
    d->data->url = url;
    d->data->line = line;
}

/*!
    Returns the expression's scope object, if provided, otherwise 0.

    In addition to data provided by the expression's QDeclarativeContext, the scope
    object's properties are also in scope during the expression's evaluation.
*/
QObject *QDeclarativeExpression::scopeObject() const
{
    Q_D(const QDeclarativeExpression);
    return d->data->me;
}

/*!
    Returns true if the last call to value() resulted in an error, 
    otherwise false.
    
    \sa error(), clearError()
*/
bool QDeclarativeExpression::hasError() const
{
    Q_D(const QDeclarativeExpression);
    return d->data->error.isValid();
}

/*!
    Clear any expression errors.  Calls to hasError() following this will
    return false.

    \sa hasError(), error()
*/
void QDeclarativeExpression::clearError()
{
    Q_D(QDeclarativeExpression);
    d->data->error = QDeclarativeError();
}

/*!
    Return any error from the last call to value().  If there was no error,
    this returns an invalid QDeclarativeError instance.

    \sa hasError(), clearError()
*/

QDeclarativeError QDeclarativeExpression::error() const
{
    Q_D(const QDeclarativeExpression);
    return d->data->error;
}

/*! \internal */
void QDeclarativeExpression::__q_notify()
{
    Q_D(QDeclarativeExpression);
    d->emitValueChanged();
}

void QDeclarativeExpressionPrivate::clearGuards()
{
    Q_Q(QDeclarativeExpression);

    static int notifyIdx = -1;
    if (notifyIdx == -1) 
        notifyIdx = 
            QDeclarativeExpression::staticMetaObject.indexOfMethod("__q_notify()");

    for (int ii = 0; ii < data->guardListLength; ++ii) {
        if (data->guardList[ii].data()) {
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 2))
            QMetaObject::disconnectOne(data->guardList[ii].data(), 
                                       data->guardList[ii].notifyIndex, 
                                       q, notifyIdx);
#else
            // QTBUG-6781
            QMetaObject::disconnect(data->guardList[ii].data(), 
                                    data->guardList[ii].notifyIndex, 
                                    q, notifyIdx);
#endif
        }
    }

    delete [] data->guardList; data->guardList = 0; 
    data->guardListLength = 0;
}

void QDeclarativeExpressionPrivate::updateGuards(const QPODVector<QDeclarativeEnginePrivate::CapturedProperty> &properties)
{
    //clearGuards();
    Q_Q(QDeclarativeExpression);

    static int notifyIdx = -1;
    if (notifyIdx == -1) 
        notifyIdx = 
            QDeclarativeExpression::staticMetaObject.indexOfMethod("__q_notify()");

    QDeclarativeExpressionData::SignalGuard *newGuardList = 0;
    
    if (properties.count() != data->guardListLength)
        newGuardList = new QDeclarativeExpressionData::SignalGuard[properties.count()];

    bool outputWarningHeader = false;
    int hit = 0;
    for (int ii = 0; ii < properties.count(); ++ii) {
        const QDeclarativeEnginePrivate::CapturedProperty &property = properties.at(ii);

        bool needGuard = true;
        if (ii >= data->guardListLength) {
            // New guard
        } else if(data->guardList[ii].data() == property.object && 
                  data->guardList[ii].notifyIndex == property.notifyIndex) {
            // Cache hit
            if (!data->guardList[ii].isDuplicate || 
                (data->guardList[ii].isDuplicate && hit == ii)) {
                needGuard = false;
                ++hit;
            }
        } else if(data->guardList[ii].data() && !data->guardList[ii].isDuplicate) {
            // Cache miss
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 2))
            QMetaObject::disconnectOne(data->guardList[ii].data(), 
                                       data->guardList[ii].notifyIndex, 
                                       q, notifyIdx);
#else
            // QTBUG-6781
            QMetaObject::disconnect(data->guardList[ii].data(), 
                                    data->guardList[ii].notifyIndex, 
                                    q, notifyIdx);
#endif
        } 
        /* else {
            // Cache miss, but nothing to do
        } */

        if (needGuard) {
            if (!newGuardList) {
                newGuardList = new QDeclarativeExpressionData::SignalGuard[properties.count()];
                for (int jj = 0; jj < ii; ++jj)
                    newGuardList[jj] = data->guardList[jj];
            }

            if (property.notifyIndex != -1) {
                bool existing = false;
                for (int jj = 0; !existing && jj < ii; ++jj) 
                    existing = newGuardList[jj].data() == property.object &&
                        newGuardList[jj].notifyIndex == property.notifyIndex;

                newGuardList[ii] = property.object;
                newGuardList[ii].notifyIndex = property.notifyIndex;
                if (existing)
                    newGuardList[ii].isDuplicate = true;
                else
                    QMetaObject::connect(property.object, property.notifyIndex,
                                         q, notifyIdx);
            } else {
                if (!outputWarningHeader) {
                    outputWarningHeader = true;
                    qWarning() << "QDeclarativeExpression: Expression" << q->expression()
                               << "depends on non-NOTIFYable properties:";
                }

                const QMetaObject *metaObj = property.object->metaObject();
                QMetaProperty metaProp = metaObj->property(property.coreIndex);

                qWarning().nospace() << "    " << metaObj->className()
                                     << "::" << metaProp.name();
            }
        } else if (newGuardList) {
            newGuardList[ii] = data->guardList[ii];
        }
    }

    for (int ii = properties.count(); ii < data->guardListLength; ++ii) {
        if (data->guardList[ii].data() && !data->guardList[ii].isDuplicate) {
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 2))
            QMetaObject::disconnectOne(data->guardList[ii].data(), 
                                       data->guardList[ii].notifyIndex, 
                                       q, notifyIdx);
#else
            // QTBUG-6781
            QMetaObject::disconnect(data->guardList[ii].data(), 
                                    data->guardList[ii].notifyIndex, 
                                    q, notifyIdx);
#endif
        }
    }

    if (newGuardList) {
        if (data->guardList) delete [] data->guardList;
        data->guardList = newGuardList;
        data->guardListLength = properties.count();
    }
}

/*!
    \fn void QDeclarativeExpression::valueChanged()

    Emitted each time the expression value changes from the last time it was
    evaluated.  The expression must have been evaluated at least once (by
    calling QDeclarativeExpression::value()) before this signal will be emitted.
*/

void QDeclarativeExpressionPrivate::emitValueChanged()
{
    Q_Q(QDeclarativeExpression);
    emit q->valueChanged();
}

QDeclarativeAbstractExpression::QDeclarativeAbstractExpression()
: m_context(0), m_prevExpression(0), m_nextExpression(0)
{
}

QDeclarativeAbstractExpression::~QDeclarativeAbstractExpression()
{
    if (m_prevExpression) {
        *m_prevExpression = m_nextExpression;
        if (m_nextExpression) 
            m_nextExpression->m_prevExpression = m_prevExpression;
    }
}

QDeclarativeContext *QDeclarativeAbstractExpression::context() const
{
    return m_context;
}

void QDeclarativeAbstractExpression::setContext(QDeclarativeContext *context)
{
    if (m_prevExpression) {
        *m_prevExpression = m_nextExpression;
        if (m_nextExpression) 
            m_nextExpression->m_prevExpression = m_prevExpression;
        m_prevExpression = 0;
        m_nextExpression = 0;
    }

    m_context = context;

    if (m_context) {
        QDeclarativeContextPrivate *cp = 
            static_cast<QDeclarativeContextPrivate *>(QObjectPrivate::get(m_context));
        m_nextExpression = cp->expressions;
        if (m_nextExpression) 
            m_nextExpression->m_prevExpression = &m_nextExpression;
        m_prevExpression = &cp->expressions;
        cp->expressions = this;
    }
}

void QDeclarativeAbstractExpression::refresh()
{
}

bool QDeclarativeAbstractExpression::isValid() const
{
    return m_context != 0;
}

QT_END_NAMESPACE

