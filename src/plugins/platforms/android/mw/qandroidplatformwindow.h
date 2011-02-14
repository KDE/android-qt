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
