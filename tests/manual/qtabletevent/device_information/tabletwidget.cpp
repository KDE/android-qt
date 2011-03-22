/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite module of the Qt Toolkit.
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

#include "tabletwidget.h"
#include <QPainter>
#include <QApplication>

TabletWidget::TabletWidget()
{
    QPalette newPalette = palette();
    newPalette.setColor(QPalette::Window, Qt::white);
    newPalette.setColor(QPalette::WindowText, Qt::black);
    setPalette(newPalette);
    qApp->installEventFilter(this);
    resetAttributes();
}

bool TabletWidget::eventFilter(QObject *, QEvent *ev)
{
    switch (ev->type()) {
    case QEvent::TabletEnterProximity:
    case QEvent::TabletLeaveProximity:
    case QEvent::TabletMove:
    case QEvent::TabletPress:
    case QEvent::TabletRelease:
        {
            QTabletEvent *event = static_cast<QTabletEvent*>(ev);
            mType = event->type();
            mPos = event->pos();
            mGPos = event->globalPos();
            mHiResGlobalPos = event->hiResGlobalPos();
            mDev = event->device();
            mPointerType = event->pointerType();
            mUnique = event->uniqueId();
            mXT = event->xTilt();
            mYT = event->yTilt();
            mZ = event->z();
            mPress = event->pressure();
            mTangential = event->tangentialPressure();
            mRot = event->rotation();
            if (isVisible())
                update();
            break;
        }
    case QEvent::MouseMove:
        {
            resetAttributes();
            QMouseEvent *event = static_cast<QMouseEvent*>(ev);
            mType = event->type();
            mPos = event->pos();
            mGPos = event->globalPos();
        }
    default:
        break;
    }
    return false;
}

void TabletWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QStringList eventInfo;

    QString typeString("Event type: ");
    switch (mType) {
    case QEvent::TabletEnterProximity:
        typeString += "QEvent::TabletEnterProximity";
        break;
    case QEvent::TabletLeaveProximity:
        typeString += "QEvent::TabletLeaveProximity";
        break;
    case QEvent::TabletMove:
        typeString += "QEvent::TabletMove";
        break;
    case QEvent::TabletPress:
        typeString += "QEvent::TabletPress";
        break;
    case QEvent::TabletRelease:
        typeString += "QEvent::TabletRelease";
        break;
    case QEvent::MouseMove:
        typeString += "QEvent::MouseMove";
        break;
    }
    eventInfo << typeString;

    eventInfo << QString("Global position: %1 %2").arg(QString::number(mGPos.x()), QString::number(mGPos.y()));
    eventInfo << QString("Local position: %1 %2").arg(QString::number(mPos.x()), QString::number(mPos.y()));
    if (mType == QEvent::TabletEnterProximity || mType == QEvent::TabletLeaveProximity
        || mType == QEvent::TabletMove || mType == QEvent::TabletPress
        || mType == QEvent::TabletRelease) {

        eventInfo << QString("Hight res global position: %1 %2").arg(QString::number(mHiResGlobalPos.x()), QString::number(mHiResGlobalPos.y()));

        QString pointerType("Pointer type: ");
        switch (mPointerType) {
        case QTabletEvent::UnknownPointer:
            pointerType += "QTabletEvent::UnknownPointer";
            break;
        case QTabletEvent::Pen:
            pointerType += "QTabletEvent::Pen";
            break;
        case QTabletEvent::Cursor:
            pointerType += "QTabletEvent::Cursor";
            break;
        case QTabletEvent::Eraser:
            pointerType += "QTabletEvent::Eraser";
            break;
        }
        eventInfo << pointerType;


        QString deviceString = "Device type: ";
        switch (mDev) {
        case QTabletEvent::NoDevice:
            deviceString += "QTabletEvent::NoDevice";
            break;
        case QTabletEvent::Puck:
            deviceString += "QTabletEvent::Puck";
            break;
        case QTabletEvent::Stylus:
            deviceString += "QTabletEvent::Stylus";
            break;
        case QTabletEvent::Airbrush:
            deviceString += "QTabletEvent::Airbrush";
            break;
        case QTabletEvent::FourDMouse:
            deviceString += "QTabletEvent::FourDMouse";
            break;
        case QTabletEvent::RotationStylus:
            deviceString += "QTabletEvent::RotationStylus";
            break;
        }
        eventInfo << deviceString;

        eventInfo << QString("Pressure: %1").arg(QString::number(mPress));
        eventInfo << QString("Tangential pressure: %1").arg(QString::number(mTangential));
        eventInfo << QString("Rotation: %1").arg(QString::number(mRot));
        eventInfo << QString("xTilt: %1").arg(QString::number(mXT));
        eventInfo << QString("yTilt: %1").arg(QString::number(mYT));
        eventInfo << QString("z: %1").arg(QString::number(mZ));

        eventInfo << QString("Unique Id: %1").arg(QString::number(mUnique));
    }

    painter.drawText(rect(), eventInfo.join("\n"));
}

void TabletWidget::tabletEvent(QTabletEvent *event)
{
    event->accept();
}

