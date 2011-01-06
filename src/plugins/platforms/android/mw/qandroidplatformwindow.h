#ifndef QANDROIDPLATFORMWINDOW_H
#define QANDROIDPLATFORMWINDOW_H

#include <QPlatformWindow>
#include <QObject>

QT_BEGIN_NAMESPACE

class QAndroidPlatformScreen;
class QAndroidPlatformWindow : public QObject, public QPlatformWindow
{
    Q_OBJECT

public:
    QAndroidPlatformWindow(QWidget *tlw);

    ~QAndroidPlatformWindow();

    virtual void setVisible(bool visible);
    virtual void setOpacity(qreal level);
    void setGeometry(const QRect &rect);

    virtual Qt::WindowFlags setWindowFlags(Qt::WindowFlags flags);
    virtual void setWindowTitle(const QString &title);
    virtual void raise();
    virtual void lower();

    WId winId() const { return m_windowId; }


private:
    QWidget *m_window;
    WId     m_windowId;
};
QT_END_NAMESPACE


#endif // QANDROIDPLATFORMWINDOW_H
