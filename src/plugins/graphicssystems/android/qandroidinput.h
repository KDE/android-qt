#ifndef QDIRECTFBINPUT_H
#define QDIRECTFBINPUT_H

#include <QHash>
#include <QMouseEvent>
#include <QEvent>
#include <QPair>
#include <QQueue>
#include <QTimer>
#include <QMutex>

class QAndroidGraphicsSystem;

class QAndroidInput : public QObject
{
    Q_OBJECT
public:
    static QAndroidInput * androidInput(){return QAndroidInput::m_androidInput;}
    void registerWindow(long mWindowId, QWidget * window);
    void unregisterWindow(long mWindowId);

    void addMouseEvent(long mWindowId, const QMouseEvent & event);
    void addKeyEvent(long mWindowId, const QKeyEvent & event);

private:
    QAndroidInput(QObject *parent = 0);
    friend class QAndroidGraphicsSystem;

private slots:
    void consumeEvents();

private:
    static QAndroidInput * m_androidInput;
    QMutex mMutex;
    QTimer mTimer;
    QHash<long, QWidget*> mWindows;
    QQueue< QPair<QWidget*, QMouseEvent> > mMouseEvents;
    QQueue< QPair<QWidget*, QKeyEvent> > mKeyEvents;
};


#endif // QDIRECTFBINPUT_H
