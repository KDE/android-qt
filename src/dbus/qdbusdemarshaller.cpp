/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#include "qdbusargument_p.h"
#include <stdlib.h>

QT_BEGIN_NAMESPACE

template <typename T>
static inline T qIterGet(DBusMessageIter *it)
{
    T t;
    q_dbus_message_iter_get_basic(it, &t);
    q_dbus_message_iter_next(it);
    return t;
}

QDBusDemarshaller::~QDBusDemarshaller()
{
}

inline QString QDBusDemarshaller::currentSignature()
{
    char *sig = q_dbus_message_iter_get_signature(&iterator);
    QString retval = QString::fromUtf8(sig);
    q_dbus_free(sig);

    return retval;
}

inline uchar QDBusDemarshaller::toByte()
{
    return qIterGet<uchar>(&iterator);
}

inline bool QDBusDemarshaller::toBool()
{
    return bool(qIterGet<dbus_bool_t>(&iterator));
}

inline ushort QDBusDemarshaller::toUShort()
{
    return qIterGet<dbus_uint16_t>(&iterator);
}

inline short QDBusDemarshaller::toShort()
{
    return qIterGet<dbus_int16_t>(&iterator);
}

inline int QDBusDemarshaller::toInt()
{
    return qIterGet<dbus_int32_t>(&iterator);
}

inline uint QDBusDemarshaller::toUInt()
{
    return qIterGet<dbus_uint32_t>(&iterator);
}

inline qlonglong QDBusDemarshaller::toLongLong()
{
    return qIterGet<qlonglong>(&iterator);
}

inline qulonglong QDBusDemarshaller::toULongLong()
{
    return qIterGet<qulonglong>(&iterator);
}

inline double QDBusDemarshaller::toDouble()
{
    return qIterGet<double>(&iterator);
}

inline QString QDBusDemarshaller::toString()
{
    return QString::fromUtf8(qIterGet<char *>(&iterator));
}

inline QDBusObjectPath QDBusDemarshaller::toObjectPath()
{
    return QDBusObjectPath(QString::fromUtf8(qIterGet<char *>(&iterator)));
}

inline QDBusSignature QDBusDemarshaller::toSignature()
{
    return QDBusSignature(QString::fromUtf8(qIterGet<char *>(&iterator)));
}

inline QDBusVariant QDBusDemarshaller::toVariant()
{
    QDBusDemarshaller sub;
    sub.message = q_dbus_message_ref(message);
    q_dbus_message_iter_recurse(&iterator, &sub.iterator);
    q_dbus_message_iter_next(&iterator);

    return QDBusVariant( sub.toVariantInternal() );
}

QDBusArgument::ElementType QDBusDemarshaller::currentType()
{
    switch (q_dbus_message_iter_get_arg_type(&iterator)) {
    case DBUS_TYPE_BYTE:
    case DBUS_TYPE_INT16:
    case DBUS_TYPE_UINT16:
    case DBUS_TYPE_INT32:
    case DBUS_TYPE_UINT32:
    case DBUS_TYPE_INT64:
    case DBUS_TYPE_UINT64:
    case DBUS_TYPE_BOOLEAN:
    case DBUS_TYPE_DOUBLE:
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_SIGNATURE:
        return QDBusArgument::BasicType;

    case DBUS_TYPE_VARIANT:
        return QDBusArgument::VariantType;

    case DBUS_TYPE_ARRAY:
        switch (q_dbus_message_iter_get_element_type(&iterator)) {
        case DBUS_TYPE_BYTE:
        case DBUS_TYPE_STRING:
            // QByteArray and QStringList
            return QDBusArgument::BasicType;
        case DBUS_TYPE_DICT_ENTRY:
            return QDBusArgument::MapType;
        default:
            return QDBusArgument::ArrayType;
        }

    case DBUS_TYPE_STRUCT:
        return QDBusArgument::StructureType;
    case DBUS_TYPE_DICT_ENTRY:
        return QDBusArgument::MapEntryType;

    case DBUS_TYPE_INVALID:
        return QDBusArgument::UnknownType;

    default:
        qWarning("QDBusDemarshaller: Found unknown D-Bus type %d '%c'",
                 q_dbus_message_iter_get_arg_type(&iterator),
                 q_dbus_message_iter_get_arg_type(&iterator));
    }
    return QDBusArgument::UnknownType;
}

QVariant QDBusDemarshaller::toVariantInternal()
{
    switch (q_dbus_message_iter_get_arg_type(&iterator)) {
    case DBUS_TYPE_BYTE:
        return QVariant::fromValue(toByte());
    case DBUS_TYPE_INT16:
	return QVariant::fromValue(toShort());
    case DBUS_TYPE_UINT16:
	return QVariant::fromValue(toUShort());
    case DBUS_TYPE_INT32:
        return toInt();
    case DBUS_TYPE_UINT32:
        return toUInt();
    case DBUS_TYPE_DOUBLE:
        return toDouble();
    case DBUS_TYPE_BOOLEAN:
        return toBool();
    case DBUS_TYPE_INT64:
        return toLongLong();
    case DBUS_TYPE_UINT64:
        return toULongLong();
    case DBUS_TYPE_STRING:
        return toString();
    case DBUS_TYPE_OBJECT_PATH:
        return QVariant::fromValue(toObjectPath());
    case DBUS_TYPE_SIGNATURE:
        return QVariant::fromValue(toSignature());
    case DBUS_TYPE_VARIANT:
        return QVariant::fromValue(toVariant());

    case DBUS_TYPE_ARRAY:
        switch (q_dbus_message_iter_get_element_type(&iterator)) {
        case DBUS_TYPE_BYTE:
            // QByteArray
            return toByteArray();
        case DBUS_TYPE_STRING:
            return toStringList();
        case DBUS_TYPE_DICT_ENTRY:
            return QVariant::fromValue(duplicate());

        default:
            return QVariant::fromValue(duplicate());
        }

    case DBUS_TYPE_STRUCT:
        return QVariant::fromValue(duplicate());

    default:
        qWarning("QDBusDemarshaller: Found unknown D-Bus type %d '%c'",
                 q_dbus_message_iter_get_arg_type(&iterator),
                 q_dbus_message_iter_get_arg_type(&iterator));
        return QVariant();
        break;
    };
}

QStringList QDBusDemarshaller::toStringList()
{
    QStringList list;

    QDBusDemarshaller sub;
    q_dbus_message_iter_recurse(&iterator, &sub.iterator);
    q_dbus_message_iter_next(&iterator);
    while (!sub.atEnd())
        list.append(sub.toString());

    return list;
}

QByteArray QDBusDemarshaller::toByteArray()
{
    DBusMessageIter sub;
    q_dbus_message_iter_recurse(&iterator, &sub);
    q_dbus_message_iter_next(&iterator);
    int len;
    char* data;
    q_dbus_message_iter_get_fixed_array(&sub,&data,&len);
    return QByteArray(data,len);
}

bool QDBusDemarshaller::atEnd()
{
    // dbus_message_iter_has_next is broken if the list has one single element
    return q_dbus_message_iter_get_arg_type(&iterator) == DBUS_TYPE_INVALID;
}

inline QDBusDemarshaller *QDBusDemarshaller::beginStructure()
{
    return beginCommon();
}

inline QDBusDemarshaller *QDBusDemarshaller::beginArray()
{
    return beginCommon();
}

inline QDBusDemarshaller *QDBusDemarshaller::beginMap()
{
    return beginCommon();
}

inline QDBusDemarshaller *QDBusDemarshaller::beginMapEntry()
{
    return beginCommon();
}

QDBusDemarshaller *QDBusDemarshaller::beginCommon()
{
    QDBusDemarshaller *d = new QDBusDemarshaller;
    d->parent = this;
    d->message = q_dbus_message_ref(message);

    // recurse
    q_dbus_message_iter_recurse(&iterator, &d->iterator);
    q_dbus_message_iter_next(&iterator);
    return d;
}

inline QDBusDemarshaller *QDBusDemarshaller::endStructure()
{
    return endCommon();
}

inline QDBusDemarshaller *QDBusDemarshaller::endArray()
{
    return endCommon();
}

inline QDBusDemarshaller *QDBusDemarshaller::endMap()
{
    return endCommon();
}

inline QDBusDemarshaller *QDBusDemarshaller::endMapEntry()
{
    return endCommon();
}

QDBusDemarshaller *QDBusDemarshaller::endCommon()
{
    QDBusDemarshaller *retval = parent;
    delete this;
    return retval;
}

QDBusArgument QDBusDemarshaller::duplicate()
{
    QDBusDemarshaller *d = new QDBusDemarshaller;
    d->iterator = iterator;
    d->message = q_dbus_message_ref(message);

    q_dbus_message_iter_next(&iterator);
    return QDBusArgumentPrivate::create(d);
}

QT_END_NAMESPACE
