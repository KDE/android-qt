/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "previewframe.h"
#include "previewwidget.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QPaintEvent>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

    class PreviewMdiArea: public QMdiArea {
    public:
        PreviewMdiArea(QWidget *parent = 0) : QMdiArea(parent) {}
    protected:
        bool viewportEvent ( QEvent * event );
    };

    bool PreviewMdiArea::viewportEvent (QEvent * event) {
        if (event->type() != QEvent::Paint)
            return QMdiArea::viewportEvent (event);
        QWidget *paintWidget = viewport();
        QPainter p(paintWidget);
        p.fillRect(rect(), paintWidget->palette().color(backgroundRole()).dark());
        p.setPen(QPen(Qt::white));
        //: Palette editor background
        p.drawText(0, height() / 2,  width(), height(), Qt::AlignHCenter,
                   QCoreApplication::translate("qdesigner_internal::PreviewMdiArea", "The moose in the noose\nate the goose who was loose."));
        return true;
    }

PreviewFrame::PreviewFrame(QWidget *parent) :
    QFrame(parent),
    m_mdiArea(new PreviewMdiArea(this))
{
    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(1);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    vbox->addWidget(m_mdiArea);

    setMinimumSize(ensureMdiSubWindow()->minimumSizeHint());
}

void PreviewFrame::setPreviewPalette(const QPalette &pal)
{
    ensureMdiSubWindow()->widget()->setPalette(pal);
}

void PreviewFrame::setSubWindowActive(bool active)
{
    m_mdiArea->setActiveSubWindow (active ? ensureMdiSubWindow() : static_cast<QMdiSubWindow *>(0));
}

QMdiSubWindow *PreviewFrame::ensureMdiSubWindow()
{
    if (!m_mdiSubWindow) {
        PreviewWidget *previewWidget = new PreviewWidget(m_mdiArea);
        m_mdiSubWindow = m_mdiArea->addSubWindow(previewWidget, Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
        m_mdiSubWindow->move(10,10);
        m_mdiSubWindow->showMaximized();
    }

    const Qt::WindowStates state = m_mdiSubWindow->windowState();
    if (state & Qt::WindowMinimized)
        m_mdiSubWindow->setWindowState(state & ~Qt::WindowMinimized);

    return m_mdiSubWindow;
}
}

QT_END_NAMESPACE
