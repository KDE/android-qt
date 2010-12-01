#ifndef QANDROIDPLATFORMEVENTLOOPINTEGRATION_H
#define QANDROIDPLATFORMEVENTLOOPINTEGRATION_H

#include <QPlatformEventLoopIntegration>
#include <QSemaphore>
class QThread;
class QAndroidPlatformEventLoopIntegration : public QPlatformEventLoopIntegration
{
public:
    virtual void startEventLoop();
    virtual void quitEventLoop();
    virtual void qtNeedsToProcessEvents();
private:
    QSemaphore m_mainMethodSemaphore;
    QThread*   m_mainMethodThread;
};

#endif // QANDROIDPLATFORMEVENTLOOPINTEGRATION_H
