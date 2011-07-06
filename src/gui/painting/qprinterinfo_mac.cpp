/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qprinterinfo.h"
#include "qprinterinfo_p.h"

#include "private/qt_mac_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PRINTER

extern QPrinter::PaperSize qSizeFTopaperSize(const QSizeF &size);

QList<QPrinterInfo> QPrinterInfo::availablePrinters()
{
    QList<QPrinterInfo> printers;

    QCFType<CFArrayRef> array;
    if (PMServerCreatePrinterList(kPMServerLocal, &array) == noErr) {
        CFIndex count = CFArrayGetCount(array);
        for (int i = 0; i < count; ++i) {
            PMPrinter printer = static_cast<PMPrinter>(const_cast<void *>(CFArrayGetValueAtIndex(array, i)));
            QString printerName = QCFString::toQString(PMPrinterGetName(printer));

            QPrinterInfo printerInfo(printerName);
            if (PMPrinterIsDefault(printer))
                printerInfo.d_ptr->isDefault = true;
            printers.append(printerInfo);
        }
    }

    return printers;
}

QPrinterInfo QPrinterInfo::defaultPrinter()
{
    QList<QPrinterInfo> printers = availablePrinters();
    foreach (const QPrinterInfo &printerInfo, printers) {
        if (printerInfo.isDefault())
            return printerInfo;
    }

    return printers.value(0);
}

QList<QPrinter::PaperSize> QPrinterInfo::supportedPaperSizes() const
{
    const Q_D(QPrinterInfo);

    QList<QPrinter::PaperSize> paperSizes;
    if (isNull())
        return paperSizes;

    PMPrinter cfPrn = PMPrinterCreateFromPrinterID(QCFString::toCFStringRef(d->name));
    if (!cfPrn)
        return paperSizes;

    CFArrayRef array;
    if (PMPrinterGetPaperList(cfPrn, &array) != noErr) {
        PMRelease(cfPrn);
        return paperSizes;
    }

    int count = CFArrayGetCount(array);
    for (int i = 0; i < count; ++i) {
        PMPaper paper = static_cast<PMPaper>(const_cast<void *>(CFArrayGetValueAtIndex(array, i)));
        double width, height;
        if (PMPaperGetWidth(paper, &width) == noErr && PMPaperGetHeight(paper, &height) == noErr) {
            QSizeF size(width * 0.3527, height * 0.3527);
            paperSizes.append(qSizeFTopaperSize(size));
        }
    }

    PMRelease(cfPrn);

    return paperSizes;
}

#endif // QT_NO_PRINTER

QT_END_NAMESPACE
