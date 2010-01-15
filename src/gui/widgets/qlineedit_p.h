/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QLINEEDIT_P_H
#define QLINEEDIT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qglobal.h"

#ifndef QT_NO_LINEEDIT
#include "private/qwidget_p.h"
#include "QtGui/qlineedit.h"
#include "QtGui/qtextlayout.h"
#include "QtGui/qstyleoption.h"
#include "QtCore/qbasictimer.h"
#include "QtGui/qcompleter.h"
#include "QtCore/qpointer.h"
#include "QtGui/qlineedit.h"

#include "private/qlinecontrol_p.h"

QT_BEGIN_NAMESPACE

class QLineEditPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QLineEdit)
public:

    QLineEditPrivate()
        : control(0), frame(1), contextMenuEnabled(1), cursorVisible(0),
        dragEnabled(0), clickCausedFocus(0), hscroll(0), vscroll(0),
        alignment(Qt::AlignLeading | Qt::AlignVCenter),
        leftTextMargin(0), topTextMargin(0), rightTextMargin(0), bottomTextMargin(0)
    {
    }

    ~QLineEditPrivate()
    {
        delete control;
    }

    QLineControl *control;

#ifndef QT_NO_CONTEXTMENU
    QPointer<QAction> selectAllAction;
#endif
    void init(const QString&);

    int xToPos(int x, QTextLine::CursorPosition = QTextLine::CursorBetweenCharacters) const;
    QRect cursorRect() const;
    void setCursorVisible(bool visible);

    void updatePasswordEchoEditing(bool);

    inline bool shouldEnableInputMethod() const
    {
        return !control->isReadOnly();
    }

    QPoint tripleClick;
    QBasicTimer tripleClickTimer;
    uint frame : 1;
    uint contextMenuEnabled : 1;
    uint cursorVisible : 1;
    uint dragEnabled : 1;
    uint clickCausedFocus : 1;
    int hscroll;
    int vscroll;
    uint alignment;
    static const int verticalMargin;
    static const int horizontalMargin;

    bool sendMouseEventToInputContext(QMouseEvent *e);

    QRect adjustedContentsRect() const;

    void _q_handleWindowActivate();
    void _q_textEdited(const QString &);
    void _q_cursorPositionChanged(int, int);
#ifdef QT_KEYPAD_NAVIGATION
    void _q_editFocusChange(bool);
#endif
    void _q_selectionChanged();
#ifndef QT_NO_COMPLETER
    void _q_completionHighlighted(QString);
#endif
#ifndef QT_NO_DRAGANDDROP
    QPoint dndPos;
    QBasicTimer dndTimer;
    void drag();
#endif

    int leftTextMargin;
    int topTextMargin;
    int rightTextMargin;
    int bottomTextMargin;

    QString placeholderText;
};

#endif // QT_NO_LINEEDIT

QT_END_NAMESPACE

#endif // QLINEEDIT_P_H
