/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BRUSHMANAGERPROXY_H
#define BRUSHMANAGERPROXY_H

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;

namespace qdesigner_internal {

class QtBrushManager;
class BrushManagerProxyPrivate;

class BrushManagerProxy : public QObject
{
    Q_OBJECT
public:
    explicit BrushManagerProxy(QDesignerFormEditorInterface *core, QObject *parent = 0);
    ~BrushManagerProxy();

    void setBrushManager(QtBrushManager *manager);

private:
    QScopedPointer<BrushManagerProxyPrivate> d_ptr;
    Q_DECLARE_PRIVATE(BrushManagerProxy)
    Q_DISABLE_COPY(BrushManagerProxy)
    Q_PRIVATE_SLOT(d_func(), void brushAdded(const QString &, const QBrush &))
    Q_PRIVATE_SLOT(d_func(), void brushRemoved(const QString &name))
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif
