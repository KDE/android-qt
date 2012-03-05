/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QAbstractTextDocumentLayout>
#include <private/qtextdocument_p.h>

#ifndef COMMON_H
#define COMMON_H

class QTestDocumentLayout : public QAbstractTextDocumentLayout
{
    Q_OBJECT
public:
    QTestDocumentLayout(QTextDocument *doc) : QAbstractTextDocumentLayout(doc), f(-1), called(false) {}
    virtual void draw(QPainter *, const PaintContext &)  {}
    virtual int hitTest(const QPointF &, Qt::HitTestAccuracy ) const { return 0; }

    virtual void documentChanged(int from, int oldLength, int length)
    {
        called = true;
        lastDocumentLengths.append(document()->docHandle()->length());

        if (f < 0)
            return;

        if(from != f ||
                o != oldLength ||
                l != length) {
            qDebug("checkDocumentChanged: got %d %d %d, expected %d %d %d", from, oldLength, length, f, o, l);
            error = true;
        }
    }

    virtual int pageCount() const { return 1; }
    virtual QSizeF documentSize() const { return QSizeF(); }

    virtual QRectF frameBoundingRect(QTextFrame *) const { return QRectF(); }
    virtual QRectF blockBoundingRect(const QTextBlock &) const { return QRectF(); }

    int f;
    int o;
    int l;

    void expect(int from, int oldLength, int length) {
	f = from;
	o = oldLength;
	l = length;
	error = false;
	called = false;
    }
    bool error;
    bool called;
    QList<int> lastDocumentLengths;
};

#endif
