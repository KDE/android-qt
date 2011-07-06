/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "window.h"

void SlowWidget::setGeometry(const QRectF &rect)
{
    bool reiterate = false;
    Statistics &stats = *m_stats;
    if (stats.relayoutClicked) {
        ++(stats.setGeometryTracker[this]);
        ++stats.setGeometryCount;
        qDebug() << "setGeometryCount:" << stats.setGeometryCount;
        if (stats.setGeometryTracker.count() == m_window->m_depthSpinBox->value()) {
            ++stats.currentBenchmarkIteration;
            qDebug() << "currentBenchmarkIteration:" << stats.currentBenchmarkIteration;
            if (stats.currentBenchmarkIteration == m_window->m_benchmarkIterationsSpinBox->value()) {
                if (stats.output)
                    stats.output->setText(tr("DONE. Elapsed: %1, setGeometryCount: %2").arg(stats.time.elapsed()).arg(stats.setGeometryCount));
            } else {
                reiterate = true;
            }
            stats.setGeometryTracker.clear();

        }
    }

    QGraphicsWidget::setGeometry(rect);

    if (reiterate) {
        m_window->doAgain();
        //QTimer::singleShot(0, m_window, SLOT(doAgain()));
    }
}

