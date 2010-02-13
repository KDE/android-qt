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

#include "helpviewer.h"
#include "centralwidget.h"

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtCore/QVariant>
#include <QtCore/QByteArray>
#include <QtCore/QTimer>

#include <QtGui/QMenu>
#include <QtGui/QKeyEvent>
#include <QtGui/QClipboard>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>

#include <QtHelp/QHelpEngine>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_WEBKIT)

class HelpNetworkReply : public QNetworkReply
{
public:
    HelpNetworkReply(const QNetworkRequest &request, const QByteArray &fileData,
        const QString &mimeType);

    virtual void abort();

    virtual qint64 bytesAvailable() const
        { return data.length() + QNetworkReply::bytesAvailable(); }

protected:
    virtual qint64 readData(char *data, qint64 maxlen);

private:
    QByteArray data;
    qint64 origLen;
};

HelpNetworkReply::HelpNetworkReply(const QNetworkRequest &request,
        const QByteArray &fileData, const QString& mimeType)
    : data(fileData), origLen(fileData.length())
{
    setRequest(request);
    setOpenMode(QIODevice::ReadOnly);

    setHeader(QNetworkRequest::ContentTypeHeader, mimeType);
    setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(origLen));
    QTimer::singleShot(0, this, SIGNAL(metaDataChanged()));
    QTimer::singleShot(0, this, SIGNAL(readyRead()));
}

void HelpNetworkReply::abort()
{
    // nothing to do
}

qint64 HelpNetworkReply::readData(char *buffer, qint64 maxlen)
{
    qint64 len = qMin(qint64(data.length()), maxlen);
    if (len) {
        qMemCopy(buffer, data.constData(), len);
        data.remove(0, len);
    }
    if (!data.length())
        QTimer::singleShot(0, this, SIGNAL(finished()));
    return len;
}

class HelpNetworkAccessManager : public QNetworkAccessManager
{
public:
    HelpNetworkAccessManager(QHelpEngine *engine, QObject *parent);

protected:
    virtual QNetworkReply *createRequest(Operation op,
        const QNetworkRequest &request, QIODevice *outgoingData = 0);

private:
    QHelpEngine *helpEngine;
};

HelpNetworkAccessManager::HelpNetworkAccessManager(QHelpEngine *engine,
        QObject *parent)
    : QNetworkAccessManager(parent), helpEngine(engine)
{
}

QNetworkReply *HelpNetworkAccessManager::createRequest(Operation /*op*/,
    const QNetworkRequest &request, QIODevice* /*outgoingData*/)
{
    const QUrl& url = request.url();
    QString mimeType = url.toString();
    if (mimeType.endsWith(QLatin1String(".svg"))
        || mimeType.endsWith(QLatin1String(".svgz"))) {
            mimeType = QLatin1String("image/svg+xml");
    } else if (mimeType.endsWith(QLatin1String(".css"))) {
        mimeType = QLatin1String("text/css");
    } else if (mimeType.endsWith(QLatin1String(".js"))) {
        mimeType = QLatin1String("text/javascript");
    } else if (mimeType.endsWith(QLatin1String(".txt"))) {
        mimeType = QLatin1String("text/plain");
    } else {
        mimeType = QLatin1String("text/html");
    }

    const QByteArray &ba = helpEngine->fileData(url);
    return new HelpNetworkReply(request, ba.isEmpty() ? " " : ba, mimeType);
}

class HelpPage : public QWebPage
{
public:
    HelpPage(CentralWidget *central, QHelpEngine *engine, QObject *parent);

protected:
    virtual QWebPage *createWindow(QWebPage::WebWindowType);
    virtual void triggerAction(WebAction action, bool checked = false);

    virtual bool acceptNavigationRequest(QWebFrame *frame,
        const QNetworkRequest &request, NavigationType type);

private:
    CentralWidget *centralWidget;
    QHelpEngine *helpEngine;
    bool closeNewTabIfNeeded;

    friend class HelpViewer;
    Qt::MouseButtons m_pressedButtons;
    Qt::KeyboardModifiers m_keyboardModifiers;
};

HelpPage::HelpPage(CentralWidget *central, QHelpEngine *engine, QObject *parent)
    : QWebPage(parent)
    , centralWidget(central)
    , helpEngine(engine)
    , closeNewTabIfNeeded(false)
    , m_pressedButtons(Qt::NoButton)
    , m_keyboardModifiers(Qt::NoModifier)
{
}

QWebPage *HelpPage::createWindow(QWebPage::WebWindowType)
{
    HelpPage* newPage = static_cast<HelpPage*>(centralWidget->newEmptyTab()->page());
    if (newPage)
        newPage->closeNewTabIfNeeded = closeNewTabIfNeeded;
    closeNewTabIfNeeded = false;
    return newPage;
}

static bool isLocalUrl(const QUrl &url)
{
    const QString scheme = url.scheme();
    if (scheme.isEmpty()
        || scheme == QLatin1String("file")
        || scheme == QLatin1String("qrc")
        || scheme == QLatin1String("data")
        || scheme == QLatin1String("qthelp")
        || scheme == QLatin1String("about"))
        return true;
    return false;
}

void HelpPage::triggerAction(WebAction action, bool checked)
{
    switch (action) {
        case OpenLinkInNewWindow:
            closeNewTabIfNeeded = true;
        default:        // fall through
            QWebPage::triggerAction(action, checked);
            break;
    }
}

bool HelpPage::acceptNavigationRequest(QWebFrame *,
    const QNetworkRequest &request, QWebPage::NavigationType type)
{
    const QUrl &url = request.url();
    const bool closeNewTab = closeNewTabIfNeeded;
    closeNewTabIfNeeded = false;

    if (isLocalUrl(url)) {
        const QString& path = url.path();
        if (path.endsWith(QLatin1String(".pdf"))) {
            const int lastDash = path.lastIndexOf(QChar('/'));
            QString fileName = QDir::tempPath() + QDir::separator();
            if (lastDash < 0)
                fileName += path;
            else
                fileName += path.mid(lastDash + 1, path.length());

            QFile tmpFile(QDir::cleanPath(fileName));
            if (tmpFile.open(QIODevice::ReadWrite)) {
                tmpFile.write(helpEngine->fileData(url));
                tmpFile.close();
            }
            QDesktopServices::openUrl(QUrl(tmpFile.fileName()));

            if (closeNewTab)
                QMetaObject::invokeMethod(CentralWidget::instance(), "closeTab");
            return false;
        }

        if (type == QWebPage::NavigationTypeLinkClicked
            && (m_keyboardModifiers & Qt::ControlModifier
            || m_pressedButtons == Qt::MidButton)) {
                HelpViewer* viewer = centralWidget->newEmptyTab();
                if (viewer)
                    CentralWidget::instance()->setSource(url);
                m_pressedButtons = Qt::NoButton;
                m_keyboardModifiers = Qt::NoModifier;
                return false;
        }
        return true;
    }

    QDesktopServices::openUrl(url);
    return false;
}

HelpViewer::HelpViewer(QHelpEngine *engine, CentralWidget *parent)
    : QWebView(parent)
    , helpEngine(engine)
    , parentWidget(parent)
    , loadFinished(false)
{
    setAcceptDrops(false);

    setPage(new HelpPage(parent, helpEngine, this));

    page()->setNetworkAccessManager(new HelpNetworkAccessManager(engine, this));

    QAction* action = pageAction(QWebPage::OpenLinkInNewWindow);
    action->setText(tr("Open Link in New Tab"));
    if (!parent)
        action->setVisible(false);

    pageAction(QWebPage::DownloadLinkToDisk)->setVisible(false);
    pageAction(QWebPage::DownloadImageToDisk)->setVisible(false);
    pageAction(QWebPage::OpenImageInNewWindow)->setVisible(false);

    connect(pageAction(QWebPage::Copy), SIGNAL(changed()), this,
        SLOT(actionChanged()));
    connect(pageAction(QWebPage::Back), SIGNAL(changed()), this,
        SLOT(actionChanged()));
    connect(pageAction(QWebPage::Forward), SIGNAL(changed()), this,
        SLOT(actionChanged()));
    connect(page(), SIGNAL(linkHovered(QString,QString,QString)), this,
        SIGNAL(highlighted(QString)));
    connect(this, SIGNAL(urlChanged(QUrl)), this, SIGNAL(sourceChanged(QUrl)));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(setLoadFinished(bool)));
}

void HelpViewer::setSource(const QUrl &url)
{
    loadFinished = false;
    if (url.toString() == QLatin1String("help")) {
        load(QUrl(QLatin1String("qthelp://com.trolltech.com."
            "assistantinternal-1.0.0/assistant/assistant.html")));
    } else {
        load(url);
    }
}

void HelpViewer::resetZoom()
{
    setTextSizeMultiplier(1.0);
}

void HelpViewer::zoomIn(qreal range)
{
    setTextSizeMultiplier(textSizeMultiplier() + range / 10.0);
}

void HelpViewer::zoomOut(qreal range)
{
    setTextSizeMultiplier(qMax(0.0, textSizeMultiplier() - range / 10.0));
}

void HelpViewer::home()
{
    QString homepage = helpEngine->customValue(QLatin1String("homepage"),
        QLatin1String("")).toString();

    if (homepage.isEmpty()) {
        homepage = helpEngine->customValue(QLatin1String("defaultHomepage"),
            QLatin1String("help")).toString();
    }

    setSource(homepage);
}

void HelpViewer::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        const int delta = e->delta();
        if (delta > 0)
            zoomIn(delta / 120);
        else if (delta < 0)
            zoomOut(-delta / 120);
        e->accept();
        return;
    }
    QWebView::wheelEvent(e);
}

void HelpViewer::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::XButton1) {
        triggerPageAction(QWebPage::Back);
        return;
    }

    if (e->button() == Qt::XButton2) {
        triggerPageAction(QWebPage::Forward);
        return;
    }

    QWebView::mouseReleaseEvent(e);
}

void HelpViewer::actionChanged()
{
    QAction *a = qobject_cast<QAction *>(sender());
    if (a == pageAction(QWebPage::Copy))
        emit copyAvailable(a->isEnabled());
    else if (a == pageAction(QWebPage::Back))
        emit backwardAvailable(a->isEnabled());
    else if (a == pageAction(QWebPage::Forward))
        emit forwardAvailable(a->isEnabled());
}

void HelpViewer::mousePressEvent(QMouseEvent *event)
{
    HelpPage *currentPage = static_cast<HelpPage*>(page());
    if (currentPage) {
        currentPage->m_pressedButtons = event->buttons();
        currentPage->m_keyboardModifiers = event->modifiers();
    }
    QWebView::mousePressEvent(event);
}

void HelpViewer::setLoadFinished(bool ok)
{
    loadFinished = ok;
    emit sourceChanged(url());
}

#else  // !defined(QT_NO_WEBKIT)

HelpViewer::HelpViewer(QHelpEngine *engine, CentralWidget *parent)
    : QTextBrowser(parent)
    , zoomCount(0)
    , controlPressed(false)
    , lastAnchor(QString())
    , helpEngine(engine)
    , parentWidget(parent)
{
   document()->setDocumentMargin(8);
}

void HelpViewer::setSource(const QUrl &url)
{
    bool help = url.toString() == QLatin1String("help");
    if (url.isValid() && !help) {
        if (launchedWithExternalApp(url))
            return;

        QUrl u = helpEngine->findFile(url);
        if (u.isValid()) {
            QTextBrowser::setSource(u);
            return;
        }
    }

    if (help) {
        QTextBrowser::setSource(QUrl(QLatin1String("qthelp://com.trolltech.com."
            "assistantinternal-1.0.0/assistant/assistant.html")));
    } else {
        QTextBrowser::setSource(url);
        setHtml(tr("<title>Error 404...</title><div align=\"center\"><br><br>"
            "<h1>The page could not be found</h1><br><h3>'%1'</h3></div>")
            .arg(url.toString()));
        emit sourceChanged(url);
    }
}

void HelpViewer::resetZoom()
{
    if (zoomCount == 0)
        return;

    QTextBrowser::zoomOut(zoomCount);
    zoomCount = 0;
}

void HelpViewer::zoomIn(int range)
{
    if (zoomCount == 10)
        return;

    QTextBrowser::zoomIn(range);
    zoomCount++;
}

void HelpViewer::zoomOut(int range)
{
    if (zoomCount == -5)
        return;

    QTextBrowser::zoomOut(range);
    zoomCount--;
}

bool HelpViewer::launchedWithExternalApp(const QUrl &url)
{
    bool isPdf = url.path().endsWith(QLatin1String(".pdf"));
    if (url.scheme() == QLatin1String("http")
        || url.scheme() == QLatin1String("ftp")
        || url.scheme() == QLatin1String("mailto") || isPdf) {
        bool launched = false;
        if (isPdf && url.scheme() == QLatin1String("qthelp")) {
            const QString& path = url.path();
            const int lastDash = path.lastIndexOf(QChar('/'));
            QString fileName = QDir::tempPath() + QDir::separator();
            if (lastDash < 0)
                fileName += path;
            else
                fileName += path.mid(lastDash + 1, path.length());

            QFile tmpFile(QDir::cleanPath(fileName));
            if (tmpFile.open(QIODevice::ReadWrite)) {
                tmpFile.write(helpEngine->fileData(url));
                tmpFile.close();
            }
            launched = QDesktopServices::openUrl(QUrl(tmpFile.fileName()));
        } else {
            launched = QDesktopServices::openUrl(url);
        }

        if (!launched) {
            QMessageBox::information(this, tr("Help"),
                tr("Unable to launch external application.\n"), tr("OK"));
        }
        return true;
    }
    return false;
}

QVariant HelpViewer::loadResource(int type, const QUrl &name)
{
    QByteArray ba;
    if (type < 4) {
        ba = helpEngine->fileData(name);
        if (name.toString().endsWith(QLatin1String(".svg"), Qt::CaseInsensitive)) {
            QImage image;
            image.loadFromData(ba, "svg");
            if (!image.isNull())
                return image;
        }
    }
    return ba;
}

void HelpViewer::openLinkInNewTab()
{
    if(lastAnchor.isEmpty())
        return;

    parentWidget->setSourceInNewTab(QUrl(lastAnchor));
    lastAnchor.clear();
}

void HelpViewer::openLinkInNewTab(const QString &link)
{
    lastAnchor = link;
    openLinkInNewTab();
}

bool HelpViewer::hasAnchorAt(const QPoint& pos)
{
    lastAnchor = anchorAt(pos);
    if (lastAnchor.isEmpty())
        return false;

    lastAnchor = source().resolved(lastAnchor).toString();
    if (lastAnchor.at(0) == QLatin1Char('#')) {
        QString src = source().toString();
        int hsh = src.indexOf(QLatin1Char('#'));
        lastAnchor = (hsh>=0 ? src.left(hsh) : src) + lastAnchor;
    }

    return true;
}

void HelpViewer::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu(QLatin1String(""), 0);

    QUrl link;
    QAction *copyAnchorAction = 0;
    if (hasAnchorAt(e->pos())) {
        link = anchorAt(e->pos());
        if (link.isRelative())
            link = source().resolved(link);
        copyAnchorAction = menu.addAction(tr("Copy &Link Location"));
        copyAnchorAction->setEnabled(!link.isEmpty() && link.isValid());

        menu.addAction(tr("Open Link in New Tab\tCtrl+LMB"), this,
            SLOT(openLinkInNewTab()));
        menu.addSeparator();
    }
    menu.addActions(parentWidget->globalActions());
    QAction *action = menu.exec(e->globalPos());
    if (action == copyAnchorAction)
        QApplication::clipboard()->setText(link.toString());
}

void HelpViewer::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::XButton1) {
        QTextBrowser::backward();
        return;
    }

    if (e->button() == Qt::XButton2) {
        QTextBrowser::forward();
        return;
    }

    controlPressed = e->modifiers() & Qt::ControlModifier;
    if ((controlPressed && hasAnchorAt(e->pos())) ||
        (e->button() == Qt::MidButton && hasAnchorAt(e->pos()))) {
        openLinkInNewTab();
        return;
    }

    QTextBrowser::mouseReleaseEvent(e);
}

void HelpViewer::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Home && e->modifiers() != Qt::NoModifier)
        || (e->key() == Qt::Key_End && e->modifiers() != Qt::NoModifier)) {
        QKeyEvent* event = new QKeyEvent(e->type(), e->key(), Qt::NoModifier,
            e->text(), e->isAutoRepeat(), e->count());
        e = event;
    }
    QTextBrowser::keyPressEvent(e);
}

void HelpViewer::home()
{
    QString homepage = helpEngine->customValue(QLatin1String("homepage"),
        QLatin1String("")).toString();

    if (homepage.isEmpty()) {
        homepage = helpEngine->customValue(QLatin1String("defaultHomepage"),
            QLatin1String("help")).toString();
    }

    setSource(homepage);
}

void HelpViewer::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() == Qt::CTRL) {
        e->accept();
        (e->delta() > 0) ? zoomIn() : zoomOut();
    } else {
        e->ignore();
        QTextBrowser::wheelEvent(e);
    }
}

#endif  // !defined(QT_NO_WEBKIT)

QT_END_NAMESPACE
