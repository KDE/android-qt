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

#include "qdeclarativesystempalette_p.h"

#include <QApplication>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeSystemPalettePrivate : public QObjectPrivate
{
public:
    QPalette palette;
    QPalette::ColorGroup group;
};



/*!
    \qmlclass SystemPalette QDeclarativeSystemPalette
    \since 4.7
    \ingroup group_utility
    \brief The SystemPalette item gives access to the Qt palettes.
    \sa QPalette

    Example:
    \qml
    SystemPalette { id: myPalette; colorGroup: Qt.Active }

    Rectangle {
        width: 640; height: 480
        color: myPalette.window
        Text {
            anchors.fill: parent
            text: "Hello!"; color: myPalette.windowText
        }
    }
    \endqml
*/
QDeclarativeSystemPalette::QDeclarativeSystemPalette(QObject *parent)
    : QObject(*(new QDeclarativeSystemPalettePrivate), parent)
{
    Q_D(QDeclarativeSystemPalette);
    d->palette = QApplication::palette();
    d->group = QPalette::Active;
    qApp->installEventFilter(this);
}

QDeclarativeSystemPalette::~QDeclarativeSystemPalette()
{
}

/*!
    \qmlproperty color SystemPalette::window
    The window (general background) color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::window() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::Window);
}

/*!
    \qmlproperty color SystemPalette::windowText
    The window text (general foreground) color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::windowText() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::WindowText);
}

/*!
    \qmlproperty color SystemPalette::base
    The base color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::base() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::Base);
}

/*!
    \qmlproperty color SystemPalette::text
    The text color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::text() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::Text);
}

/*!
    \qmlproperty color SystemPalette::alternateBase
    The alternate base color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::alternateBase() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::AlternateBase);
}

/*!
    \qmlproperty color SystemPalette::button
    The button color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::button() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::Button);
}

/*!
    \qmlproperty color SystemPalette::buttonText
    The button text foreground color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::buttonText() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::ButtonText);
}

/*!
    \qmlproperty color SystemPalette::light
    The light color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::light() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::Light);
}

/*!
    \qmlproperty color SystemPalette::midlight
    The midlight color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::midlight() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::Midlight);
}

/*!
    \qmlproperty color SystemPalette::dark
    The dark color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::dark() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::Dark);
}

/*!
    \qmlproperty color SystemPalette::mid
    The mid color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::mid() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::Mid);
}

/*!
    \qmlproperty color SystemPalette::shadow
    The shadow color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::shadow() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::Shadow);
}

/*!
    \qmlproperty color SystemPalette::highlight
    The highlight color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::highlight() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::Highlight);
}

/*!
    \qmlproperty color SystemPalette::highlightedText
    The highlighted text color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QDeclarativeSystemPalette::highlightedText() const
{
    Q_D(const QDeclarativeSystemPalette);
    return d->palette.color(d->group, QPalette::HighlightedText);
}

/*!
    \qmlproperty QDeclarativeSystemPalette::ColorGroup SystemPalette::colorGroup

    The color group of the palette. It can be Active, Inactive or Disabled.
    Active is the default.

    \sa QPalette::ColorGroup
*/
QDeclarativeSystemPalette::ColorGroup QDeclarativeSystemPalette::colorGroup() const
{
    Q_D(const QDeclarativeSystemPalette);
    return (QDeclarativeSystemPalette::ColorGroup)d->group;
}

void QDeclarativeSystemPalette::setColorGroup(QDeclarativeSystemPalette::ColorGroup colorGroup)
{
    Q_D(QDeclarativeSystemPalette);
    d->group = (QPalette::ColorGroup)colorGroup;
    emit paletteChanged();
}

bool QDeclarativeSystemPalette::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == qApp) {
        if (event->type() == QEvent::ApplicationPaletteChange) {
            QApplication::postEvent(this, new QEvent(QEvent::ApplicationPaletteChange));
            return false;
        }
    }
    return QObject::eventFilter(watched, event);
}

bool QDeclarativeSystemPalette::event(QEvent *event)
{
    Q_D(QDeclarativeSystemPalette);
    if (event->type() == QEvent::ApplicationPaletteChange) {
        d->palette = QApplication::palette();
        emit paletteChanged();
        return true;
    }
    return QObject::event(event);
}

QT_END_NAMESPACE
