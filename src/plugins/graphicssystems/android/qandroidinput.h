#ifndef QDIRECTFBINPUT_H
#define QDIRECTFBINPUT_H

#include <QVector>
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
    void registerWindow(QWidget * window);
    void unregisterWindow(QWidget * window);

    void addMouseEvent(QMouseEvent * event);
    void addKeyEvent(QKeyEvent * event);

private:
    QAndroidInput(QObject *parent = 0);
    friend class QAndroidGraphicsSystem;

private slots:
    void consumeEvents();

private:
    static QAndroidInput * m_androidInput;
    QMutex mMutex;
    QTimer mTimer;
    QVector< QWidget* > mWindows;
    QQueue< QMouseEvent * > mMouseEvents;
    QQueue< QKeyEvent * > mKeyEvents;
};


#endif // QDIRECTFBINPUT_H
