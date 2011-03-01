/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QtGui>

#include "previewwindow.h"

static QString windowFlagsToString(Qt::WindowFlags flags)
{
    QString text;

    Qt::WindowFlags type = (flags & Qt::WindowType_Mask);
    if (type == Qt::Window) {
        text = "Qt::Window";
    } else if (type == Qt::Dialog) {
        text = "Qt::Dialog";
    } else if (type == Qt::Sheet) {
        text = "Qt::Sheet";
    } else if (type == Qt::Drawer) {
        text = "Qt::Drawer";
    } else if (type == Qt::Popup) {
        text = "Qt::Popup";
    } else if (type == Qt::Tool) {
        text = "Qt::Tool";
    } else if (type == Qt::ToolTip) {
        text = "Qt::ToolTip";
    } else if (type == Qt::SplashScreen) {
        text = "Qt::SplashScreen";
    }

    if (flags & Qt::MSWindowsFixedSizeDialogHint)
        text += "\n| Qt::MSWindowsFixedSizeDialogHint";
    if (flags & Qt::X11BypassWindowManagerHint)
        text += "\n| Qt::X11BypassWindowManagerHint";
    if (flags & Qt::FramelessWindowHint)
        text += "\n| Qt::FramelessWindowHint";
    if (flags & Qt::WindowTitleHint)
        text += "\n| Qt::WindowTitleHint";
    if (flags & Qt::WindowSystemMenuHint)
        text += "\n| Qt::WindowSystemMenuHint";
    if (flags & Qt::WindowMinimizeButtonHint)
        text += "\n| Qt::WindowMinimizeButtonHint";
    if (flags & Qt::WindowMaximizeButtonHint)
        text += "\n| Qt::WindowMaximizeButtonHint";
    if (flags & Qt::WindowCloseButtonHint)
        text += "\n| Qt::WindowCloseButtonHint";
    if (flags & Qt::WindowContextHelpButtonHint)
        text += "\n| Qt::WindowContextHelpButtonHint";
    if (flags & Qt::WindowShadeButtonHint)
        text += "\n| Qt::WindowShadeButtonHint";
    if (flags & Qt::WindowStaysOnTopHint)
        text += "\n| Qt::WindowStaysOnTopHint";
    if (flags & Qt::CustomizeWindowHint)
        text += "\n| Qt::CustomizeWindowHint";
    return text;
}

PreviewWindow::PreviewWindow(QWidget *parent)
    : QWidget(parent)
{
    textEdit = new QTextEdit;
    textEdit->setReadOnly(true);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);

    closeButton = new QPushButton(tr("&Close"));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    showNormalButton = new QPushButton(tr("Show normal"));
    connect(showNormalButton, SIGNAL(clicked()), this, SLOT(showNormal()));
    showMaximizedButton = new QPushButton(tr("Show maximized"));
    connect(showMaximizedButton, SIGNAL(clicked()), this, SLOT(showMaximized()));
    showFullScreenButton = new QPushButton(tr("Show fullscreen"));
    connect(showFullScreenButton, SIGNAL(clicked()), this, SLOT(showFullScreen()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(textEdit);
    layout->addWidget(showNormalButton);
    layout->addWidget(showMaximizedButton);
    layout->addWidget(showFullScreenButton);
    layout->addWidget(closeButton);
    setLayout(layout);

    setWindowTitle(tr("Preview <QWidget>"));
}

void PreviewWindow::setWindowFlags(Qt::WindowFlags flags)
{
    QWidget::setWindowFlags(flags);

    QString text = windowFlagsToString(flags);
    textEdit->setPlainText(text);
}

PreviewDialog::PreviewDialog(QWidget *parent)
    : QDialog(parent)
{
    textEdit = new QTextEdit;
    textEdit->setReadOnly(true);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);

    closeButton = new QPushButton(tr("&Close"));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    showNormalButton = new QPushButton(tr("Show normal"));
    connect(showNormalButton, SIGNAL(clicked()), this, SLOT(showNormal()));
    showMaximizedButton = new QPushButton(tr("Show maximized"));
    connect(showMaximizedButton, SIGNAL(clicked()), this, SLOT(showMaximized()));
    showFullScreenButton = new QPushButton(tr("Show fullscreen"));
    connect(showFullScreenButton, SIGNAL(clicked()), this, SLOT(showFullScreen()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(textEdit);
    layout->addWidget(showNormalButton);
    layout->addWidget(showMaximizedButton);
    layout->addWidget(showFullScreenButton);
    layout->addWidget(closeButton);
    setLayout(layout);

    setWindowTitle(tr("Preview <QDialog>"));
}

void PreviewDialog::setWindowFlags(Qt::WindowFlags flags)
{
    QWidget::setWindowFlags(flags);

    QString text = windowFlagsToString(flags);
    textEdit->setPlainText(text);
}
