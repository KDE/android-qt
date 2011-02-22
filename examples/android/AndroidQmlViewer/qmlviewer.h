#ifndef QMLVIEWER_H
#define QMLVIEWER_H

#include <QDeclarativeView>

class QmlViewer : public QDeclarativeView
{
    Q_OBJECT
public:
    explicit QmlViewer(QWidget *parent = 0);
    QmlViewer(const QString& source, QWidget *parent = 0);

public slots:
    void showBrowser();
    void launch(const QString &url);

};

#endif // QMLVIEWER_H
