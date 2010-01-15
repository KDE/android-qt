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

#include <QtCore/QUrl>
#include <QtGui/QMainWindow>

QT_BEGIN_NAMESPACE

class QAction;
class QLineEdit;
class QComboBox;
class QMenu;

class IndexWindow;
class QHelpEngineCore;
class QHelpEngine;
class CentralWidget;
class ContentWindow;
class BookmarkManager;
class BookmarkWidget;
class CmdLineParser;
class QtDocInstaller;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(CmdLineParser *cmdLine, QWidget *parent = 0);
    ~MainWindow();

    static void activateCurrentBrowser();
    static QString collectionFileDirectory(bool createDir = false,
        const QString &cacheDir = QString());
    static QString defaultHelpCollectionFileName();

public:
    void hideContents();
    void hideIndex();
    void hideBookmarks();
    void hideSearch();
    void setIndexString(const QString &str);
    void expandTOC(int depth);
    bool usesDefaultCollection() const;

signals:
    void initDone();

public slots:
    void showContents();
    void showIndex();
    void showBookmarks();
    void showSearch();
    void showSearchWidget();
    void syncContents();
    void activateCurrentCentralWidgetTab();
    void currentFilterChanged(const QString &filter);

private slots:
    void insertLastPages();
    void addBookmark();
    void gotoAddress();
    void showPreferences();
    void showNewAddress();
    void showAboutDialog();
    void copyAvailable(bool yes);
    void updateNavigationItems();
    void updateTabCloseAction();
    void showNewAddress(const QUrl &url);
    void addNewBookmark(const QString &title, const QString &url);
    void showTopicChooser(const QMap<QString, QUrl> &links, const QString &keyword);
    void updateApplicationFont();
    void filterDocumentation(const QString &customFilter);
    void setupFilterCombo();
    void lookForNewQtDocumentation();
    void indexingStarted();
    void indexingFinished();
    void displayInstallationError(const QString &errorMessage);
    void qtDocumentationInstalled(bool newDocsInstalled);
    void checkInitState();

    void updateBookmarkMenu();
    void showBookmark(QAction *action);

private:
    bool initHelpDB();
    void setupActions();
    void closeEvent(QCloseEvent *e);
    void activateDockWidget(QWidget *w);
    void updateAboutMenuText();
    void setupFilterToolbar();
    void setupAddressToolbar();
    QMenu *toolBarMenu();
    QWidget *setupBookmarkWidget();

    QHelpEngine *m_helpEngine;
    CentralWidget *m_centralWidget;
    IndexWindow *m_indexWindow;
    ContentWindow *m_contentWindow;
    BookmarkWidget *m_bookmarkWidget;
    BookmarkManager *m_bookmarkManager;
    QLineEdit *m_addressLineEdit;
    QComboBox *m_filterCombo;

    QAction *m_backAction;
    QAction *m_nextAction;
    QAction *m_homeAction;
    QAction *m_syncAction;
    QAction *m_copyAction;
    QAction *m_findAction;
    QAction *m_printAction;
    QAction *m_printPreviewAction;
    QAction *m_pageSetupAction;
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    QAction *m_resetZoomAction;
    QAction *m_aboutAction;
    QAction *m_closeTabAction;
    QAction *m_newTabAction;

    QMenu *m_viewMenu;
    QMenu *m_toolBarMenu;
    QMenu *m_bookmarkMenu;
    QAction *m_bookmarkMenuAction;

    CmdLineParser *m_cmdLine;

    QWidget *m_progressWidget;
    QtDocInstaller *m_qtDocInstaller;

    bool m_connectedInitSignals;
};

QT_END_NAMESPACE

#endif // MAINWINDOW_H
