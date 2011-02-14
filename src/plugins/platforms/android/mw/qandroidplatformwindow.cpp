/*
    Copyright (c) 2009-2011, BogDan Vatra <bog_dan_ro@yahoo.com>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the  BogDan Vatra <bog_dan_ro@yahoo.com> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY  BogDan Vatra <bog_dan_ro@yahoo.com> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "androidjnimain.h"
#include "qandroidplatformwindow.h"
#include "qandroidplatformscreen.h"
#include <QWidget>
#include <QDebug>

QT_BEGIN_NAMESPACE

QAndroidPlatformWindow::QAndroidPlatformWindow(QWidget *tlw):
        QPlatformWindow(tlw)
{
    static QAtomicInt winIdGenerator(1);
    m_windowId = winIdGenerator.fetchAndAddRelaxed(1);
    m_window = tlw;
    QtAndroid::createWindow(false, tlw, m_windowId, m_window->geometry().left(), m_window->geometry().top(),
                                            m_window->geometry().right(), m_window->geometry().bottom());
}

QAndroidPlatformWindow::~QAndroidPlatformWindow()
{
    QtAndroid::destroyWindow(m_windowId);
}

void QAndroidPlatformWindow::setGeometry(const QRect &rect)
{
    m_window->setGeometry(rect);
    QtAndroid::resizeWindow(m_windowId, rect.left(), rect.top(),
                                            rect.right(), rect.bottom());
}


void QAndroidPlatformWindow::setVisible(bool visible)
{
    QtAndroid::setWindowVisiblity(winId(), visible);
}


void QAndroidPlatformWindow::setOpacity(qreal level)
{
    QtAndroid::setWindowOpacity(winId(), level);
}

Qt::WindowFlags QAndroidPlatformWindow::setWindowFlags(Qt::WindowFlags flags)
{
   qDebug()<<"QAndroidPlatformWindow::setWindowFlags"<<flags;
   if (flags != Qt::Popup && flags != Qt::Tool && flags != Qt::ToolTip && flags != Qt::SplashScreen)
        m_window->setWindowState(m_window->windowState() | Qt::WindowMaximized);
    return flags;
}

void QAndroidPlatformWindow::setWindowTitle(const QString & title)
{
    QtAndroid::setWindowTitle(winId(), title);
}

void QAndroidPlatformWindow::raise()
{
    QtAndroid::raiseWindow(winId());
}

void QAndroidPlatformWindow::lower()
{
    QtAndroid::lowerWindow(winId());
}

QT_END_NAMESPACE
