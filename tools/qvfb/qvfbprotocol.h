/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
#ifndef QVFBPROTOCOL_H
#define QVFBPROTOCOL_H

#include <QImage>
#include <QVector>
#include <QColor>

QT_BEGIN_NAMESPACE

class QVFbKeyProtocol;
class QVFbMouseProtocol;
class QVFbViewProtocol : public QObject
{
    Q_OBJECT
public:
    QVFbViewProtocol(int display_id, QObject *parent = 0);

    virtual ~QVFbViewProtocol();

    int id() const { return mDisplayId; }

    void sendKeyboardData(QString unicode, int keycode,
            int modifiers, bool press, bool repeat);
    void sendMouseData(const QPoint &pos, int buttons, int wheel);

    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual int depth() const = 0;
    virtual int linestep() const = 0;
    virtual int  numcols() const = 0;
    virtual QVector<QRgb> clut() const = 0;
    virtual unsigned char *data() const = 0;
    virtual int brightness() const = 0;

    virtual void setRate(int) {}
public slots:
    virtual void flushChanges();

signals:
    void displayDataChanged(const QRect &);
    void displayEmbedRequested(WId windowId);

protected:
    virtual QVFbKeyProtocol *keyHandler() const = 0;
    virtual QVFbMouseProtocol *mouseHandler() const = 0;

private:
    int mDisplayId;
};

class QVFbKeyProtocol
{
public:
    QVFbKeyProtocol(int display_id) : mDisplayId(display_id) {}
    virtual ~QVFbKeyProtocol() {}

    int id() const { return mDisplayId; }

    virtual void sendKeyboardData(QString unicode, int keycode,
            int modifiers, bool press, bool repeat) = 0;

private:
    int mDisplayId;
};

class QVFbMouseProtocol
{
public:
    QVFbMouseProtocol(int display_id) : mDisplayId(display_id) {}
    virtual ~QVFbMouseProtocol() {}

    int id() const { return mDisplayId; }

    virtual void sendMouseData(const QPoint &pos, int buttons, int wheel) = 0;

private:
    int mDisplayId;
};

/* since there is very little variation in input protocols defaults are
   provided */

class QVFbKeyPipeProtocol : public QVFbKeyProtocol
{
public:
    QVFbKeyPipeProtocol(int display_id);
    ~QVFbKeyPipeProtocol();

    void sendKeyboardData(QString unicode, int keycode,
            int modifiers, bool press, bool repeat);

    QString pipeName() const { return fileName; }
private:
    int fd;
    QString fileName;
};

class QVFbMousePipe: public QVFbMouseProtocol
{
public:
    QVFbMousePipe(int display_id);
    ~QVFbMousePipe();

    void sendMouseData(const QPoint &pos, int buttons, int wheel);

    QString pipeName() const { return fileName; }
protected:
    int fd;
    QString fileName;
};

class QTimer;
class QVFbMouseLinuxTP : public QObject, public QVFbMousePipe
{
    Q_OBJECT
public:
    QVFbMouseLinuxTP(int display_id);
    ~QVFbMouseLinuxTP();

    void sendMouseData(const QPoint &pos, int buttons, int wheel);

protected slots:
    void repeatLastPress();

protected:
    void writeToPipe(const QPoint &pos, ushort pressure);
    QPoint lastPos;
    QTimer *repeater;
};

QT_END_NAMESPACE

#endif
