#include "qmlviewer.h"
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDir>
#include <QTimer>

QmlViewer::QmlViewer(QWidget *parent) :
    QDeclarativeView(parent)
{
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    connect(engine(), SIGNAL(quit()), SLOT(showBrowser()));
    launch("qrc:/browser/Browser.qml");
}

QmlViewer::QmlViewer(const QString &source, QWidget *parent):
    QDeclarativeView(parent)
{
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    connect(engine(), SIGNAL(quit()), SLOT(showBrowser()));
    launch(source);
}

void QmlViewer::showBrowser()
{
    QMetaObject::invokeMethod(this, "launch", Qt::QueuedConnection, Q_ARG(QString, QString("qrc:/browser/Browser.qml")));
}

void QmlViewer::launch(const QString &url)
{
    QDeclarativeContext *ctxt = rootContext();
    ctxt->setContextProperty("qmlViewer", this);
    ctxt->setContextProperty("qmlViewerFolder", QDir::currentPath());
    setSource(url);
    QTimer::singleShot(10, this, SLOT(showMaximized()));
}
