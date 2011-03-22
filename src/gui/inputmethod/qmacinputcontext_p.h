/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMACINPUTCONTEXT_P_H
#define QMACINPUTCONTEXT_P_H

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

#include "QtGui/qinputcontext.h"
#include "private/qt_mac_p.h"

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QMacInputContext : public QInputContext
{
    Q_OBJECT
    //Q_DECLARE_PRIVATE(QMacInputContext)
    void createTextDocument();
public:
    explicit QMacInputContext(QObject* parent = 0);
    virtual ~QMacInputContext();

    virtual void setFocusWidget(QWidget *w);
    virtual QString identifierName() { return QLatin1String("mac"); }
    virtual QString language();

    virtual void reset();

    virtual bool isComposing() const;

    static OSStatus globalEventProcessor(EventHandlerCallRef, EventRef, void *);
    static void initialize();
    static void cleanup();

    EventRef lastKeydownEvent() { return keydownEvent; }
    void setLastKeydownEvent(EventRef);

protected:
    void mouseHandler(int pos, QMouseEvent *);
private:
    bool composing;
    bool recursionGuard;
    TSMDocumentID textDocument;
    QString currentText;
    EventRef keydownEvent;
};

QT_END_NAMESPACE

#endif // QMACINPUTCONTEXT_P_H
