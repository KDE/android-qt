/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "serenum.h"
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

#include <usb.h>

QList<SerialPortId> enumerateSerialPorts()
{
    QList<QString> eligableInterfaces;
    QList<SerialPortId> list;

    usb_init();
    usb_find_busses();
    usb_find_devices();

    for (struct usb_bus *bus = usb_get_busses(); bus; bus = bus->next) {
        for (struct usb_device *device = bus->devices; device; device = device->next) {
            for (int n = 0; n < device->descriptor.bNumConfigurations; ++n) {
                struct usb_config_descriptor &usbConfig =device->config[n];
                QList<int> usableInterfaces;
                for (int m = 0; m < usbConfig.bNumInterfaces; ++m) {
                    for (int o = 0; o < usbConfig.interface[m].num_altsetting; ++o) {
                        struct usb_interface_descriptor &descriptor = usbConfig.interface[m].altsetting[o];
                        if (descriptor.bInterfaceClass != 2 // "Communication"
                                || descriptor.bInterfaceSubClass != 2 // Abstract (modem)
                                || descriptor.bInterfaceProtocol != 255) // Vendor Specific
                            continue;

                        unsigned char *buf = descriptor.extra;
                        unsigned int size = descriptor.extralen;
                        while (size >= 2 * sizeof(u_int8_t)) {
                            // for Communication devices there is a slave interface for the actual
                            // data transmission.
                            // the extra info stores that as a index for the interface
                            if (buf[0] >= 5 && buf[1] == 36 && buf[2] == 6) { // CDC Union
                                for (int i = 4; i < buf[0]; i++)
                                    usableInterfaces.append((int) buf[i]);
                            }
                            size -= buf[0];
                            buf += buf[0];
                        }
                    }
                }

                // second loop to find the actual data interface.
                foreach (int i, usableInterfaces) {
                    for (int m = 0; m < usbConfig.bNumInterfaces; ++m) {
                        for (int o = 0; o < usbConfig.interface[m].num_altsetting; ++o) {
                            struct usb_interface_descriptor &descriptor = usbConfig.interface[m].altsetting[o];
                            if (descriptor.bInterfaceNumber != i)
                                continue;
                            if (descriptor.bInterfaceClass == 10) { // "CDC Data"
                                // qDebug() << "      found the data port"
                                //     << "bus:" << bus->dirname
                                //     << "device" << device->filename
                                //     << "interface" << descriptor.bInterfaceNumber;
                                eligableInterfaces << QString("if%1").arg(QString::number(i), 2, QChar('0')); // fix!
                            }
                        }
                    }
                }
            }
        }
    }

    QDir dir("/dev/serial/by-id/");
    foreach (const QFileInfo &info, dir.entryInfoList()) {
        if (!info.isDir()) {
            bool usable = eligableInterfaces.isEmpty();
            foreach (const QString &iface, eligableInterfaces) {
                if (info.fileName().contains(iface)) {
                    usable = true;
                    break;
                }
            }
            if (!usable)
                continue;

            SerialPortId id;
            id.friendlyName = info.fileName();
            id.portName = info.canonicalFilePath();
            list << id;
        }
    }
    return list;
}

