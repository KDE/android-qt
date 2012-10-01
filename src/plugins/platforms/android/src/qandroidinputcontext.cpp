/*
    Copyright (c) 2009-2011, BogDan Vatra <bog_dan_ro@yahoo.com>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the  BogDan Vatra <bog_dan_ro@yahoo.com> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY BogDan Vatra <bog_dan_ro@yahoo.com> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL BogDan Vatra <bog_dan_ro@yahoo.com> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <android/log.h>

#include "qandroidinputcontext.h"
#include "androidjnimain.h"
#include <QDebug>

static QAndroidInputContext * m_androidInputContext = 0;
static char const * const QtNativeInputConnectionClassName = "org/kde/necessitas/industrius/QtNativeInputConnection";
static char const * const QtExtractedTextClassName = "org/kde/necessitas/industrius/QtExtractedText";
static jclass m_extractedTextClass = 0;
static jmethodID m_classConstructorMethodID=0;
static jfieldID m_partialEndOffsetFieldID=0;
static jfieldID m_partialStartOffsetFieldID=0;
static jfieldID m_selectionEndFieldID=0;
static jfieldID m_selectionStartFieldID=0;
static jfieldID m_startOffsetFieldID=0;
static jfieldID m_textFieldID=0;

static jboolean commitText(JNIEnv *env, jobject /*thiz*/, jstring text, jint newCursorPosition)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    jboolean isCopy;
    const jchar * jstr = env->GetStringChars(text, &isCopy);
    QString str((const QChar*)jstr,  env->GetStringLength(text));
    env->ReleaseStringChars(text, jstr);
    return m_androidInputContext->commitText( str
                                            , newCursorPosition );
}

static jboolean deleteSurroundingText(JNIEnv */*env*/, jobject /*thiz*/, jint leftLength, jint rightLength)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return m_androidInputContext->deleteSurroundingText(leftLength, rightLength);
}

static jboolean finishComposingText(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return m_androidInputContext->finishComposingText();
}

static jint getCursorCapsMode(JNIEnv */*env*/, jobject /*thiz*/, jint reqModes)
{
    if (!m_androidInputContext)
        return 0;
    return m_androidInputContext->getCursorCapsMode(reqModes);
}

static jobject getExtractedText(JNIEnv *env, jobject /*thiz*/, int hintMaxChars, int hintMaxLines, jint flags)
{
    if (!m_androidInputContext)
        return 0;

    const QAndroidInputContext::ExtractedText & extractedText=m_androidInputContext->getExtractedText(hintMaxChars, hintMaxLines, flags);
    jobject object = env->NewObject(m_extractedTextClass, m_classConstructorMethodID);
    env->SetIntField(object, m_partialStartOffsetFieldID, extractedText.partialStartOffset);
    env->SetIntField(object, m_partialEndOffsetFieldID, extractedText.partialEndOffset);
    env->SetIntField(object, m_selectionStartFieldID, extractedText.selectionStart);
    env->SetIntField(object, m_selectionEndFieldID, extractedText.selectionEnd);
    env->SetIntField(object, m_startOffsetFieldID, extractedText.startOffset);
    env->SetObjectField(object, m_textFieldID, env->NewString((jchar*)extractedText.text.constData(), (jsize)extractedText.text.length()));
    return object;
}

static jstring getSelectedText(JNIEnv * env, jobject /*thiz*/, jint flags)
{
    if (!m_androidInputContext)
        return 0;
    const QString & text = m_androidInputContext->getSelectedText(flags);
    return env->NewString((jchar*)text.constData(), (jsize)text.length());
}

static jstring getTextAfterCursor(JNIEnv * env, jobject /*thiz*/, jint length, jint flags)
{
    if (!m_androidInputContext)
        return 0;
    const QString & text = m_androidInputContext->getTextAfterCursor(length, flags);
    return env->NewString((jchar*)text.constData(), (jsize)text.length());
}

static jstring getTextBeforeCursor(JNIEnv * env, jobject /*thiz*/, jint length, jint flags)
{
    if (!m_androidInputContext)
        return 0;
    const QString & text = m_androidInputContext->getTextBeforeCursor(length, flags);
    return env->NewString((jchar*)text.constData(), (jsize)text.length());
}

static jboolean setComposingText(JNIEnv * env, jobject /*thiz*/, jstring text, jint newCursorPosition)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    jboolean isCopy;
    const jchar * jstr = env->GetStringChars(text, &isCopy);
    QString str((const QChar*)jstr,  env->GetStringLength(text));
    env->ReleaseStringChars(text, jstr);
    return m_androidInputContext->setComposingText(str, newCursorPosition);
}

static jboolean setSelection(JNIEnv */*env*/, jobject /*thiz*/, jint start, jint end)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return m_androidInputContext->setSelection(start, end);
}

static jboolean selectAll(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return m_androidInputContext->selectAll();
}

static jboolean cut(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return m_androidInputContext->cut();
}

static jboolean copy(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return m_androidInputContext->copy();
}

static jboolean copyURL(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return m_androidInputContext->copyURL();
}

static jboolean paste(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return m_androidInputContext->paste();
}


static JNINativeMethod methods[] = {
    {"commitText", "(Ljava/lang/String;I)Z", (void *)commitText},
    {"deleteSurroundingText", "(II)Z", (void *)deleteSurroundingText},
    {"finishComposingText", "()Z", (void *)finishComposingText},
    {"getCursorCapsMode", "(I)I", (void *)getCursorCapsMode},
    {"getExtractedText", "(III)Lorg/kde/necessitas/industrius/QtExtractedText;", (void *)getExtractedText},
    {"getSelectedText", "(I)Ljava/lang/String;", (void *)getSelectedText},
    {"getTextAfterCursor", "(II)Ljava/lang/String;", (void *)getTextAfterCursor},
    {"getTextBeforeCursor", "(II)Ljava/lang/String;", (void *)getTextBeforeCursor},
    {"setComposingText", "(Ljava/lang/String;I)Z", (void *)setComposingText},
    {"setSelection", "(II)Z", (void *)setSelection},
    {"selectAll", "()Z", (void *)selectAll},
    {"cut", "()Z", (void *)cut},
    {"copy", "()Z", (void *)copy},
    {"copyURL", "()Z", (void *)copyURL},
    {"paste", "()Z", (void *)paste}
};


QAndroidInputContext::QAndroidInputContext(QObject *parent) :
    QInputContext(parent)
{
    JNIEnv * env = 0;
    if (QtAndroid::javaVM()->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return;
    }
    jclass clazz = QtAndroid::findClass(QtNativeInputConnectionClassName, env);
    if (clazz == NULL)
    {
        qCritical()<<"Native registration unable to find class '"<<QtNativeInputConnectionClassName<<"'";
        return;
    }
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) < 0)
    {
        qCritical()<<"RegisterNatives failed for '"<<QtNativeInputConnectionClassName<<"'";
        return;
    }

    clazz = QtAndroid::findClass(QtExtractedTextClassName, env);
    if (clazz == NULL)
    {
        qCritical()<<"Native registration unable to find class '"<<QtExtractedTextClassName<<"'";
        return;
    }

    m_extractedTextClass = (jclass)env->NewGlobalRef(clazz);
    m_classConstructorMethodID = env->GetMethodID(m_extractedTextClass, "<init>", "()V");
    if (m_classConstructorMethodID == NULL)
    {
        qCritical()<<"GetMethodID failed";
        return;
    }

    m_partialEndOffsetFieldID = env->GetFieldID(m_extractedTextClass, "partialEndOffset", "I");
    if (m_partialEndOffsetFieldID == NULL)
    {
        qCritical()<<"Can't find field partialEndOffset";
        return;
    }

    m_partialStartOffsetFieldID = env->GetFieldID(m_extractedTextClass, "partialStartOffset", "I");
    if (m_partialStartOffsetFieldID == NULL)
    {
        qCritical()<<"Can't find field partialStartOffset";
        return;
    }

    m_selectionEndFieldID = env->GetFieldID(m_extractedTextClass, "selectionEnd", "I");
    if (m_selectionEndFieldID == NULL)
    {
        qCritical()<<"Can't find field selectionEnd";
        return;
    }

    m_selectionStartFieldID = env->GetFieldID(m_extractedTextClass, "selectionStart", "I");
    if (m_selectionStartFieldID == NULL)
    {
        qCritical()<<"Can't find field selectionStart";
        return;
    }

    m_startOffsetFieldID = env->GetFieldID(m_extractedTextClass, "startOffset", "I");
    if (m_startOffsetFieldID == NULL)
    {
        qCritical()<<"Can't find field startOffset";
        return;
    }

    m_textFieldID = env->GetFieldID(m_extractedTextClass, "text", "Ljava/lang/String;");
    if (m_textFieldID == NULL)
    {
        qCritical()<<"Can't find field text";
        return;
    }
    qRegisterMetaType<QInputMethodEvent>("QInputMethodEvent");
    m_androidInputContext = this;
}

QAndroidInputContext::~QAndroidInputContext()
{
    m_androidInputContext = 0;
    m_extractedTextClass = 0;
    m_partialEndOffsetFieldID=0;
    m_partialStartOffsetFieldID=0;
    m_selectionEndFieldID=0;
    m_selectionStartFieldID=0;
    m_startOffsetFieldID=0;
    m_textFieldID=0;
}

QString QAndroidInputContext::identifierName()
{
    return "QAndroidInputContext";
}

bool QAndroidInputContext::isComposing() const
{
    return m_composingText.length();
}

QString QAndroidInputContext::language()
{
    return qgetenv("LANG");
}

void QAndroidInputContext::clear()
{
    m_composingText.clear();
    m_extractedText.clear();
}

void QAndroidInputContext::reset()
{
    clear();
    if (focusWidget())
        QtAndroid::resetSoftwareKeyboard();
    else
        QtAndroid::hideSoftwareKeyboard();
}

void QAndroidInputContext::update()
{
    reset();
}

void QAndroidInputContext::setFocusWidget( QWidget *w )
{
    QInputContext::setFocusWidget( w );
    if (!w)
        reset();
}

bool QAndroidInputContext::filterEvent( const QEvent * event )
{
    if (event->type() == QEvent::RequestSoftwareInputPanel)
    {
        QWidget * w = focusWidget();
        if (!w)
            return QInputContext::filterEvent(event);
        QRect wrect(w->mapToGlobal(w->rect().topLeft()),w->rect().size());
        QtAndroid::showSoftwareKeyboard(wrect.left(), wrect.top(), wrect.width(), wrect.height(), w->inputMethodHints());
        return true;
    }
    else if (event->type() == QEvent::CloseSoftwareInputPanel)
    {
        QtAndroid::hideSoftwareKeyboard();
        return true;
    } else if ( (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) && isComposing() )
    {
        finishComposingText();
    }
    return QInputContext::filterEvent(event);
}

void QAndroidInputContext::sendEvent(const QInputMethodEvent &event)
{
    QInputContext::sendEvent(event);
}

jboolean QAndroidInputContext::commitText(const QString &text, jint /*newCursorPosition*/)
{
    m_composingText = text;
    return finishComposingText();
}

jboolean QAndroidInputContext::deleteSurroundingText(jint leftLength, jint rightLength)
{
    QWidget * w = focusWidget();
    if (!w)
        return JNI_TRUE;
    const int cursorPos= w->inputMethodQuery(Qt::ImCursorPosition).toInt();
    setSelection(cursorPos-leftLength, cursorPos+rightLength);
    m_composingText.clear();
    finishComposingText();
    return JNI_TRUE;
}

jboolean QAndroidInputContext::finishComposingText()
{
    if (!focusWidget())
        return JNI_TRUE;
    QInputMethodEvent event;
    event.setCommitString(m_composingText);
    QMetaObject::invokeMethod(this, "sendEvent", Qt::BlockingQueuedConnection, Q_ARG(QInputMethodEvent, event));
    clear();
    return JNI_TRUE;
}

jint QAndroidInputContext::getCursorCapsMode(jint /*reqModes*/)
{
    QWidget * w = focusWidget();
    jint res = 0;
    if (!w)
        return res;

    const int qtInputMethodHints = w->inputMethodHints();

    if ( qtInputMethodHints & Qt::ImhPreferUppercase )
        res = CAP_MODE_SENTENCES;

    if ( qtInputMethodHints & Qt::ImhUppercaseOnly)
        res = CAP_MODE_CHARACTERS;

    return res;
}

const QAndroidInputContext::ExtractedText & QAndroidInputContext::getExtractedText(jint hintMaxChars, jint /*hintMaxLines*/, jint /*flags*/)
{
    QWidget * w = focusWidget();
    if (!w)
        return m_extractedText;
    if (hintMaxChars)
        m_extractedText.text = w->inputMethodQuery(Qt::ImSurroundingText).toString().right(hintMaxChars);
    m_extractedText.startOffset = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
    const QString & selection = w->inputMethodQuery(Qt::ImCurrentSelection).toString();
    const int selLen=selection.length();
    if (selLen)
    {
        m_extractedText.selectionStart = w->inputMethodQuery(Qt::ImAnchorPosition).toInt();
        m_extractedText.selectionEnd = m_extractedText.startOffset;
    }
    return m_extractedText;
}

QString QAndroidInputContext::getSelectedText(jint /*flags*/)
{
    QWidget * w = focusWidget();
    if (!w)
        return QString();
    return w->inputMethodQuery(Qt::ImCurrentSelection).toString();
}

QString QAndroidInputContext::getTextAfterCursor(jint length, jint /*flags*/)
{
    QWidget * w = focusWidget();
    if (!w)
        return QString();
    QString text = w->inputMethodQuery(Qt::ImSurroundingText).toString();
    if (!text.length())
        return text;
    int cursorPos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
    return text.mid(cursorPos, length);
}

QString QAndroidInputContext::getTextBeforeCursor(jint length, jint /*flags*/)
{
    QWidget * w = focusWidget();
    if (!w)
        return QString();
    QString text = w->inputMethodQuery(Qt::ImSurroundingText).toString();
    if (!text.length())
        return text;
    int cursorPos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
    const int wordLeftPos=cursorPos-length;
    return text.mid(wordLeftPos>0?wordLeftPos:0, cursorPos);
}

jboolean QAndroidInputContext::setComposingText(const QString & text, jint newCursorPosition)
{
    QWidget * w = focusWidget();
    if (!w)
        return JNI_FALSE;
    newCursorPosition+=text.length()-1;
    int cursorPos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
    m_composingText=text;
    QList<QInputMethodEvent::Attribute> attributes;
    attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor,
                                                cursorPos+newCursorPosition,
                                                1,
                                                QVariant()));
    QInputMethodEvent event(m_composingText, attributes);
    QMetaObject::invokeMethod(this, "sendEvent", Qt::BlockingQueuedConnection, Q_ARG(QInputMethodEvent, event));
    return JNI_TRUE;
}

jboolean QAndroidInputContext::setSelection(jint start, jint end)
{
    QList<QInputMethodEvent::Attribute> attributes;
    attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::Selection,
                                                start,
                                                end-start,
                                                QVariant()));
    QInputMethodEvent event(QString(), attributes);
    QMetaObject::invokeMethod(this, "sendEvent", Qt::BlockingQueuedConnection, Q_ARG(QInputMethodEvent, event));
    return JNI_TRUE;
}

jboolean QAndroidInputContext::selectAll()
{
#warning TODO
    return JNI_TRUE;
}

jboolean QAndroidInputContext::cut()
{
#warning TODO
    return JNI_TRUE;
}

jboolean QAndroidInputContext::copy()
{
#warning TODO
    return JNI_TRUE;
}

jboolean QAndroidInputContext::copyURL()
{
#warning TODO
    return JNI_TRUE;
}

jboolean QAndroidInputContext::paste()
{
#warning TODO
    return JNI_TRUE;
}
