/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qwaylandclipboard.h"
#include "qwaylanddisplay.h"
#include "qwaylandinputdevice.h"
#include <QtGui/QPlatformNativeInterface>
#include <QtGui/QApplication>
#include <QtCore/QMimeData>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QtDebug>

static QWaylandClipboard *clipboard;

class QWaylandSelection
{
public:
    QWaylandSelection(QWaylandDisplay *display, QMimeData *data);
    ~QWaylandSelection();

private:
    static uint32_t getTime();
    static void send(void *data, struct wl_selection *selection, const char *mime_type, int fd);
    static void cancelled(void *data, struct wl_selection *selection);
    static const struct wl_selection_listener selectionListener;

    QMimeData *mMimeData;
    struct wl_selection *mSelection;
};

const struct wl_selection_listener QWaylandSelection::selectionListener = {
    QWaylandSelection::send,
    QWaylandSelection::cancelled
};

uint32_t QWaylandSelection::getTime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

QWaylandSelection::QWaylandSelection(QWaylandDisplay *display, QMimeData *data)
    : mMimeData(data), mSelection(0)
{
    struct wl_shell *shell = display->wl_shell();
    mSelection = wl_shell_create_selection(shell);
    wl_selection_add_listener(mSelection, &selectionListener, this);
    foreach (const QString &format, data->formats())
        wl_selection_offer(mSelection, format.toLatin1().constData());
    wl_selection_activate(mSelection,
                          display->inputDevices().at(0)->wl_input_device(),
                          getTime());
}

QWaylandSelection::~QWaylandSelection()
{
    if (mSelection) {
        clipboard->unregisterSelection(this);
        wl_selection_destroy(mSelection);
    }
    delete mMimeData;
}

void QWaylandSelection::send(void *data,
                             struct wl_selection *selection,
                             const char *mime_type,
                             int fd)
{
    Q_UNUSED(selection);
    QWaylandSelection *self = static_cast<QWaylandSelection *>(data);
    QString mimeType = QString::fromLatin1(mime_type);
    QByteArray content = self->mMimeData->data(mimeType);
    if (!content.isEmpty()) {
        QFile f;
        if (f.open(fd, QIODevice::WriteOnly))
            f.write(content);
    }
    close(fd);
}

void QWaylandSelection::cancelled(void *data, struct wl_selection *selection)
{
    Q_UNUSED(selection);
    delete static_cast<QWaylandSelection *>(data);
}

QWaylandClipboard::QWaylandClipboard(QWaylandDisplay *display)
    : mDisplay(display), mSelection(0), mMimeDataIn(0), mOffer(0)
{
    clipboard = this;
}

QWaylandClipboard::~QWaylandClipboard()
{
    if (mOffer)
        wl_selection_offer_destroy(mOffer);
    delete mMimeDataIn;
    qDeleteAll(mSelections);
}

void QWaylandClipboard::unregisterSelection(QWaylandSelection *selection)
{
    mSelections.removeOne(selection);
}

void QWaylandClipboard::syncCallback(void *data)
{
    *static_cast<bool *>(data) = true;
}

void QWaylandClipboard::forceRoundtrip(struct wl_display *display)
{
    bool done = false;
    wl_display_sync_callback(display, syncCallback, &done);
    wl_display_iterate(display, WL_DISPLAY_WRITABLE);
    while (!done)
        wl_display_iterate(display, WL_DISPLAY_READABLE);
}

const QMimeData *QWaylandClipboard::mimeData(QClipboard::Mode mode) const
{
    Q_ASSERT(mode == QClipboard::Clipboard);
    if (!mMimeDataIn)
        mMimeDataIn = new QMimeData;
    mMimeDataIn->clear();
    if (!mOfferedMimeTypes.isEmpty() && mOffer) {
        foreach (const QString &mimeType, mOfferedMimeTypes) {
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                qWarning("QWaylandClipboard::mimedata: pipe() failed");
                break;
            }
            QByteArray mimeTypeBa = mimeType.toLatin1();
            wl_selection_offer_receive(mOffer, mimeTypeBa.constData(), pipefd[1]);
            QByteArray content;
            forceRoundtrip(mDisplay->wl_display());
            char buf[256];
            int n;
            close(pipefd[1]);
            while ((n = read(pipefd[0], &buf, sizeof buf)) > 0)
                content.append(buf, n);
            close(pipefd[0]);
            mMimeDataIn->setData(mimeType, content);
        }
    }
    return mMimeDataIn;
}

void QWaylandClipboard::setMimeData(QMimeData *data, QClipboard::Mode mode)
{
    Q_ASSERT(mode == QClipboard::Clipboard);
    if (!mDisplay->inputDevices().isEmpty()) {
        if (!data)
            data = new QMimeData;
        mSelection = new QWaylandSelection(mDisplay, data);
    } else {
        qWarning("QWaylandClipboard::setMimeData: No input devices");
    }
}

bool QWaylandClipboard::supportsMode(QClipboard::Mode mode) const
{
    return mode == QClipboard::Clipboard;
}

const struct wl_selection_offer_listener QWaylandClipboard::selectionOfferListener = {
    QWaylandClipboard::offer,
    QWaylandClipboard::keyboardFocus
};

void QWaylandClipboard::createSelectionOffer(uint32_t id)
{
    mOfferedMimeTypes.clear();
    if (mOffer)
        wl_selection_offer_destroy(mOffer);
    mOffer = 0;
    struct wl_selection_offer *offer = wl_selection_offer_create(mDisplay->wl_display(), id, 1);
    wl_selection_offer_add_listener(offer, &selectionOfferListener, this);
}

void QWaylandClipboard::offer(void *data,
                              struct wl_selection_offer *selection_offer,
                              const char *type)
{
    Q_UNUSED(selection_offer);
    QWaylandClipboard *self = static_cast<QWaylandClipboard *>(data);
    self->mOfferedMimeTypes.append(QString::fromLatin1(type));
}

void QWaylandClipboard::keyboardFocus(void *data,
                                      struct wl_selection_offer *selection_offer,
                                      wl_input_device *input_device)
{
    QWaylandClipboard *self = static_cast<QWaylandClipboard *>(data);
    if (!input_device) {
        wl_selection_offer_destroy(selection_offer);
        self->mOffer = 0;
        return;
    }
    self->mOffer = selection_offer;
    self->emitChanged(QClipboard::Clipboard);
}
