/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "proitems.h"
#include "abstractproitemvisitor.h"

#include <QtCore/QFileInfo>

QT_BEGIN_NAMESPACE

// --------------- ProItem ------------
void ProItem::setComment(const QString &comment)
{
    m_comment = comment;
}

QString ProItem::comment() const
{
    return m_comment;
}

// --------------- ProBlock ----------------

ProBlock::ProBlock(ProBlock *parent)
{
    m_blockKind = 0;
    m_parent = parent;
    m_refCount = 1;
}

ProBlock::~ProBlock()
{
    foreach (ProItem *itm, m_proitems)
        if (itm->kind() == BlockKind)
            static_cast<ProBlock *>(itm)->deref();
        else
            delete itm;
}

void ProBlock::appendItem(ProItem *proitem)
{
    m_proitems << proitem;
}

void ProBlock::setItems(const QList<ProItem *> &proitems)
{
    m_proitems = proitems;
}

QList<ProItem *> ProBlock::items() const
{
    return m_proitems;
}

void ProBlock::setBlockKind(int blockKind)
{
    m_blockKind = blockKind;
}

int ProBlock::blockKind() const
{
    return m_blockKind;
}

void ProBlock::setParent(ProBlock *parent)
{
    m_parent = parent;
}

ProBlock *ProBlock::parent() const
{
    return m_parent;
}

ProItem::ProItemKind ProBlock::kind() const
{
    return ProItem::BlockKind;
}

ProItem::ProItemReturn ProBlock::Accept(AbstractProItemVisitor *visitor)
{
    if (visitor->visitBeginProBlock(this) == ReturnSkip)
        return ReturnTrue;
    ProItemReturn rt = ReturnTrue;
    for (int i = 0; i < m_proitems.count(); ++i) {
        rt = m_proitems.at(i)->Accept(visitor);
        if (rt != ReturnTrue && rt != ReturnFalse) {
            if (rt == ReturnLoop) {
                rt = ReturnTrue;
                while (visitor->visitProLoopIteration())
                    for (int j = i; ++j < m_proitems.count(); ) {
                        rt = m_proitems.at(j)->Accept(visitor);
                        if (rt != ReturnTrue && rt != ReturnFalse) {
                            if (rt == ReturnNext) {
                                rt = ReturnTrue;
                                break;
                            }
                            if (rt == ReturnBreak)
                                rt = ReturnTrue;
                            goto do_break;
                        }
                    }
              do_break:
                visitor->visitProLoopCleanup();
            }
            break;
        }
    }
    visitor->visitEndProBlock(this);
    return rt;
}

// --------------- ProVariable ----------------
ProVariable::ProVariable(const QString &name, ProBlock *parent)
    : ProBlock(parent)
{
    setBlockKind(ProBlock::VariableKind);
    m_variable = name;
    m_variableKind = SetOperator;
}

void ProVariable::setVariableOperator(VariableOperator variableKind)
{
    m_variableKind = variableKind;
}

ProVariable::VariableOperator ProVariable::variableOperator() const
{
    return m_variableKind;
}

void ProVariable::setVariable(const QString &name)
{
    m_variable = name;
}

QString ProVariable::variable() const
{
    return m_variable;
}

ProItem::ProItemReturn ProVariable::Accept(AbstractProItemVisitor *visitor)
{
    visitor->visitBeginProVariable(this);
    foreach (ProItem *item, m_proitems)
        item->Accept(visitor); // cannot fail
    visitor->visitEndProVariable(this);
    return ReturnTrue;
}

// --------------- ProValue ----------------
ProValue::ProValue(const QString &value, ProVariable *variable)
{
    m_variable = variable;
    m_value = value;
}

void ProValue::setValue(const QString &value)
{
    m_value = value;
}

QString ProValue::value() const
{
    return m_value;
}

void ProValue::setVariable(ProVariable *variable)
{
    m_variable = variable;
}

ProVariable *ProValue::variable() const
{
    return m_variable;
}

ProItem::ProItemKind ProValue::kind() const
{
    return ProItem::ValueKind;
}

ProItem::ProItemReturn ProValue::Accept(AbstractProItemVisitor *visitor)
{
    visitor->visitProValue(this);
    return ReturnTrue;
}

// --------------- ProFunction ----------------
ProFunction::ProFunction(const QString &text)
{
    m_text = text;
}

void ProFunction::setText(const QString &text)
{
    m_text = text;
}

QString ProFunction::text() const
{
    return m_text;
}

ProItem::ProItemKind ProFunction::kind() const
{
    return ProItem::FunctionKind;
}

ProItem::ProItemReturn ProFunction::Accept(AbstractProItemVisitor *visitor)
{
    return visitor->visitProFunction(this);
}

// --------------- ProCondition ----------------
ProCondition::ProCondition(const QString &text)
{
    m_text = text;
}

void ProCondition::setText(const QString &text)
{
    m_text = text;
}

QString ProCondition::text() const
{
    return m_text;
}

ProItem::ProItemKind ProCondition::kind() const
{
    return ProItem::ConditionKind;
}

ProItem::ProItemReturn ProCondition::Accept(AbstractProItemVisitor *visitor)
{
    visitor->visitProCondition(this);
    return ReturnTrue;
}

// --------------- ProOperator ----------------
ProOperator::ProOperator(OperatorKind operatorKind)
{
    m_operatorKind = operatorKind;
}

void ProOperator::setOperatorKind(OperatorKind operatorKind)
{
    m_operatorKind = operatorKind;
}

ProOperator::OperatorKind ProOperator::operatorKind() const
{
    return m_operatorKind;
}

ProItem::ProItemKind ProOperator::kind() const
{
    return ProItem::OperatorKind;
}

ProItem::ProItemReturn ProOperator::Accept(AbstractProItemVisitor *visitor)
{
    visitor->visitProOperator(this);
    return ReturnTrue;
}

// --------------- ProFile ----------------
ProFile::ProFile(const QString &fileName)
    : ProBlock(0)
{
    m_modified = false;
    setBlockKind(ProBlock::ProFileKind);
    m_fileName = fileName;

    QFileInfo fi(fileName);
    m_displayFileName = fi.fileName();
    m_directoryName = fi.absolutePath();
}

ProFile::~ProFile()
{
}

QString ProFile::displayFileName() const
{
    return m_displayFileName;
}

QString ProFile::fileName() const
{
    return m_fileName;
}

QString ProFile::directoryName() const
{
    return m_directoryName;
}

void ProFile::setModified(bool modified)
{
    m_modified = modified;
}

bool ProFile::isModified() const
{
    return m_modified;
}

ProItem::ProItemReturn ProFile::Accept(AbstractProItemVisitor *visitor)
{
    ProItemReturn rt;
    if ((rt = visitor->visitBeginProFile(this)) != ReturnTrue)
        return rt;
    ProBlock::Accept(visitor); // cannot fail
    return visitor->visitEndProFile(this);
}

QT_END_NAMESPACE
