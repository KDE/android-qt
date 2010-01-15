/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QtGui>

#include "imagemodel.h"

//! [0]
ImageModel::ImageModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}
//! [0]

//! [1]
void ImageModel::setImage(const QImage &image)
{
    modelImage = image;
    reset();
}
//! [1]

//! [2]
int ImageModel::rowCount(const QModelIndex & /* parent */) const
{
    return modelImage.height();
}

int ImageModel::columnCount(const QModelIndex & /* parent */) const
//! [2] //! [3]
{
    return modelImage.width();
}
//! [3]

//! [4]
QVariant ImageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    return qGray(modelImage.pixel(index.column(), index.row()));
}
//! [4]

//! [5]
QVariant ImageModel::headerData(int /* section */,
                                Qt::Orientation /* orientation */,
                                int role) const
{
    if (role == Qt::SizeHintRole)
        return QSize(1, 1);
    return QVariant();
}
//! [5]
