#include "qandroidinputcontext.h"
#include "androidjnimain.h"
#include <QDebug>

QAndroidInputContext::QAndroidInputContext(QObject *parent) :
    QInputContext(parent)
{
}

QAndroidInputContext::~QAndroidInputContext()
{
    qDebug()<<"~QAndroidInputContext";
}


QString QAndroidInputContext::identifierName()
{
    return "QAndroidInputContext";
}

bool QAndroidInputContext::isComposing() const
{
    return false;
}

QString QAndroidInputContext::language()
{
    return "";
}

void QAndroidInputContext::reset()
{

}

void QAndroidInputContext::update()
{
    // QWidget * w= focusWidget();
    /// try to move the window to a new position when this widget is visible
}

bool QAndroidInputContext::filterEvent( const QEvent * event )
{
    qDebug()<<"QAndroidInputContext::filterEvent"<<event->type();
    switch (event->type())
    {
        case QEvent::RequestSoftwareInputPanel:
                QtAndroid::showSoftwareKeyboard();
            break;
        case QEvent::CloseSoftwareInputPanel:
                QtAndroid::hideSoftwareKeyboard();
            break;
        default:
            return QInputContext::filterEvent(event);
    }
    return true;
}
