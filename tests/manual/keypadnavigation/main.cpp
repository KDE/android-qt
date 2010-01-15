/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ui_keypadnavigation.h"

class KeypadNavigation : public QMainWindow
{
    Q_OBJECT

public:
    KeypadNavigation(QWidget *parent = 0)
        : QMainWindow(parent)
        , ui(new Ui_KeypadNavigation)
    {
        ui->setupUi(this);

        const struct {
            QObject *action;
            QWidget *page;
        } layoutMappings[] = {
            {ui->m_actionLayoutVerticalSimple,  ui->m_pageVerticalSimple},
            {ui->m_actionLayoutVerticalComplex, ui->m_pageVerticalComplex},
            {ui->m_actionLayoutTwoDimensional,  ui->m_pageTwoDimensional},
            {ui->m_actionLayoutSliderMagic,     ui->m_pageSliderMagic},
            {ui->m_actionLayoutChaos,           ui->m_pageChaos},
            {ui->m_actionLayoutDialogs,         ui->m_pageDialogs}
        };
        for (int i = 0; i < int(sizeof layoutMappings / sizeof layoutMappings[0]); ++i) {
            connect(layoutMappings[i].action, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
            m_layoutSignalMapper.setMapping(layoutMappings[i].action, layoutMappings[i].page);
        }
        connect(&m_layoutSignalMapper, SIGNAL(mapped(QWidget*)), ui->m_stackWidget, SLOT(setCurrentWidget(QWidget*)));

#ifdef QT_KEYPAD_NAVIGATION
        const struct {
            QObject *action;
            Qt::NavigationMode mode;
        } modeMappings[] = {
            {ui->m_actionModeNone,                  Qt::NavigationModeNone},
            {ui->m_actionModeKeypadTabOrder,        Qt::NavigationModeKeypadTabOrder},
            {ui->m_actionModeKeypadDirectional,     Qt::NavigationModeKeypadDirectional},
            {ui->m_actionModeCursorAuto,            Qt::NavigationModeCursorAuto},
            {ui->m_actionModeCursorForceVisible,    Qt::NavigationModeCursorForceVisible}
        };
        for (int i = 0; i < int(sizeof modeMappings / sizeof modeMappings[0]); ++i) {
            connect(modeMappings[i].action, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
            m_modeSignalMapper.setMapping(modeMappings[i].action, int(modeMappings[i].mode));
        }
        connect(&m_modeSignalMapper, SIGNAL(mapped(int)), SLOT(setNavigationMode(int)));
#else // QT_KEYPAD_NAVIGATION
        ui->m_menuNavigation_mode->deleteLater();
#endif // QT_KEYPAD_NAVIGATION

        const struct {
            QObject *button;
            Dialog dialog;
        } openDialogMappings[] = {
            {ui->m_buttonGetOpenFileName,       DialogGetOpenFileName},
            {ui->m_buttonGetSaveFileName,       DialogGetSaveFileName},
            {ui->m_buttonGetExistingDirectory,  DialogGetExistingDirectory},
            {ui->m_buttonGetColor,              DialogGetColor},
            {ui->m_buttonGetFont,               DialogGetFont},
            {ui->m_buttonQuestion,              DialogQuestion},
            {ui->m_buttonAboutQt,               DialogAboutQt},
            {ui->m_buttonGetItem,               DialogGetItem}
        };
        for (int i = 0; i < int(sizeof openDialogMappings / sizeof openDialogMappings[0]); ++i) {
            connect(openDialogMappings[i].button, SIGNAL(clicked()), &m_dialogSignalMapper, SLOT(map()));
            m_dialogSignalMapper.setMapping(openDialogMappings[i].button, int(openDialogMappings[i].dialog));
        }
        connect(&m_dialogSignalMapper, SIGNAL(mapped(int)), SLOT(openDialog(int)));
    }

    ~KeypadNavigation()
    {
        delete ui;
    }

protected slots:
#ifdef QT_KEYPAD_NAVIGATION
    void setNavigationMode(int mode)
    {
        QApplication::setNavigationMode(Qt::NavigationMode(mode));
    }
#endif // QT_KEYPAD_NAVIGATION

    void openDialog(int dialog)
    {
        switch (Dialog(dialog)) {
            case DialogGetOpenFileName:
                QFileDialog::getOpenFileName(this, QLatin1String("getOpenFileName"));
                break;
            case DialogGetSaveFileName:
                QFileDialog::getSaveFileName(this, QLatin1String("getSaveFileName"));
                break;
            case DialogGetExistingDirectory:
                QFileDialog::getExistingDirectory(this, QLatin1String("getExistingDirectory"));
                break;
            case DialogGetColor:
                QColorDialog::getColor(QColor(Qt::green), this, QLatin1String("getColor"));
                break;
            case DialogGetFont:
                QFontDialog::getFont(0, this);
                break;
            case DialogQuestion:
                QMessageBox::question(this, QLatin1String("question"), QLatin1String("�Hola, que tal?"));
                break;
            case DialogAboutQt:
                QMessageBox::aboutQt(this);
                break;
            case DialogGetItem:
                QInputDialog::getItem(this, QLatin1String("getItem"), QLatin1String("Choose a color"), QColor::colorNames());
                break;
            default:
            break;
        }
    }

private:
    enum Dialog {
        DialogGetOpenFileName,
        DialogGetSaveFileName,
        DialogGetExistingDirectory,
        DialogGetColor,
        DialogGetFont,
        DialogQuestion,
        DialogAboutQt,
        DialogGetItem
    };

    Ui_KeypadNavigation *ui;
    QSignalMapper m_layoutSignalMapper;
#ifdef QT_KEYPAD_NAVIGATION
    QSignalMapper m_modeSignalMapper;
#endif // QT_KEYPAD_NAVIGATION
    QSignalMapper m_dialogSignalMapper;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KeypadNavigation w;
#ifdef Q_WS_S60
    w.showMaximized();
#else
    w.show();
#endif
    return a.exec();
}

#include "main.moc"
