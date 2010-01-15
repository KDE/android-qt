/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "config.h"

#include <QPointer>
#include <QMap>

QT_BEGIN_NAMESPACE

class TabbedBrowser;
class HelpDialog;
class HelpWindow;
class QMenu;
class QDockWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    virtual ~MainWindow();

    TabbedBrowser *browsers() const;
    HelpDialog *helpDialog() const;

    void setupPopupMenu(QMenu *menu);
    static QString urlifyFileName(const QString &fileName);

    void removePendingBrowser(HelpWindow *win);

public slots:
    MainWindow *newWindow();

    void setup();
    void showLink(const QString &link);
    void showLinks(const QStringList &links);
    void saveSettings();
    void updateBookmarkMenu();
    void printingFinished();

private slots:
    void on_actionNewWindow_triggered();
    void on_actionGoHome_triggered();
    void on_actionFilePrint_triggered();
    void on_actionClose_triggered();
    void on_actionHelpWhatsThis_triggered();
    void on_actionHelpAssistant_triggered();
    void on_actionAboutApplication_triggered();
    void on_actionAboutAssistant_triggered();
    void on_actionSaveAs_triggered();
    void on_actionSyncToc_triggered();

    void about();
    void setupBookmarkMenu();
    void showBookmark(QAction *action);
    void showLinkFromClient(const QString &link);
    void showQtHelp();
    void showSearchLink(const QString &link, const QStringList &terms);
    void showGoActionLink();
    void updateProfileSettings();
    void backwardAvailable(bool);
    void forwardAvailable(bool);

    void browserTabChanged();
    void copyAvailable(bool yes);
    void updateTabActions(int index);
    void showFontSettingsDialog();

protected:
    void closeEvent(QCloseEvent *);
    void timerEvent(QTimerEvent *);

private:
    void setupGoActions();
    bool insertActionSeparator();
    void updateApplicationFontSettings(FontSettings &settings);

private:
    Ui::MainWindow ui;

    QList<QAction*> goActions;
    uint setupCompleted:1;
    TabbedBrowser *tabs;
    QMap<QAction*, QString> bookmarks;
    HelpDialog *helpDock;
    QDockWidget *dw;
    static QList<MainWindow*> windows;
    QMap<QAction*,QString> *goActionDocFiles;
    QList<QPair<HelpWindow*,QString> > pendingBrowsers;
};

#endif // MAINWINDOW_H

QT_END_NAMESPACE
