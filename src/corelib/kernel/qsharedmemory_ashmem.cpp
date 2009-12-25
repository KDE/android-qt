/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qplatformdefs.h"

#include "qsharedmemory.h"
#include "qsharedmemory_p.h"
#include "qsystemsemaphore.h"
#include <qdir.h>
#include <qdebug.h>

#include <errno.h>

#ifndef QT_NO_SHAREDMEMORY
#include <sys/types.h>
#include <sys/ipc.h>
#include <linux/ashmem.h>

#define ASHMEM_DEVICE	"/"ASHMEM_NAME_DEF

/*
 * ashmem_create_region - creates a new ashmem region and returns the file
 * descriptor, or <0 on error
 *
 * `name' is an optional label to give the region (visible in /proc/pid/maps)
 * `size' is the size of the region, in page-aligned bytes
 */
int ashmem_create_region(const char *name, size_t size)
{
	int fd, ret;

	fd = open(ASHMEM_DEVICE, O_RDWR);
	if (fd < 0)
		return fd;

	if (name) {
		char buf[ASHMEM_NAME_LEN];

		strlcpy(buf, name, sizeof(buf));
		ret = ioctl(fd, ASHMEM_SET_NAME, buf);
		if (ret < 0)
			goto error;
	}

	ret = ioctl(fd, ASHMEM_SET_SIZE, size);
	if (ret < 0)
		goto error;

	return fd;

error:
	close(fd);
	return ret;
}
//PROT_READ | PROT_WRITE
int ashmem_set_prot_region(int fd, int prot)
{
	return ioctl(fd, ASHMEM_SET_PROT_MASK, prot);
}

int ashmem_pin_region(int fd, size_t offset, size_t len)
{
	struct ashmem_pin pin = { offset, len };
	return ioctl(fd, ASHMEM_PIN, &pin);
}

int ashmem_unpin_region(int fd, size_t offset, size_t len)
{
	struct ashmem_pin pin = { offset, len };
	return ioctl(fd, ASHMEM_UNPIN, &pin);
}

int ashmem_get_size_region(int fd)
{
  return ioctl(fd, ASHMEM_GET_SIZE, NULL);
}


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif //QT_NO_SHAREDMEMORY

#include "private/qcore_unix_p.h"

#ifndef QT_NO_SHAREDMEMORY
QT_BEGIN_NAMESPACE

QSharedMemoryPrivate::QSharedMemoryPrivate()
    : QObjectPrivate(), memory(0), size(0), error(QSharedMemory::NoError),
#ifndef QT_NO_SYSTEMSEMAPHORE
      systemSemaphore(QString()), lockedByMe(false),
#endif
      ashmem_fd(-1)
{
}

void QSharedMemoryPrivate::setErrorString(const QString &function)
{
    // EINVAL is handled in functions so they can give better error strings
    switch (errno) {
    case EACCES:
        errorString = QSharedMemory::tr("%1: permission denied").arg(function);
        error = QSharedMemory::PermissionDenied;
        break;
    case EEXIST:
        errorString = QSharedMemory::tr("%1: already exists").arg(function);
        error = QSharedMemory::AlreadyExists;
        break;
    case ENOENT:
        errorString = QSharedMemory::tr("%1: doesn't exist").arg(function);
        error = QSharedMemory::NotFound;
        break;
    case EMFILE:
    case ENOMEM:
    case ENOSPC:
        errorString = QSharedMemory::tr("%1: out of resources").arg(function);
        error = QSharedMemory::OutOfResources;
        break;
    default:
        errorString = QSharedMemory::tr("%1: unknown error %2").arg(function).arg(errno);
        error = QSharedMemory::UnknownError;
#if defined QSHAREDMEMORY_DEBUG
        qDebug() << errorString << "key" << key << "errno" << errno << EINVAL;
#endif
    }
}


#endif // QT_NO_SHAREDMEMORY

#ifndef QT_NO_SHAREDMEMORY

bool QSharedMemoryPrivate::cleanHandle()
{
    if (-1!=ashmem_fd)
    {
        close(ashmem_fd);
        return true;
    }
    return false;
}

bool QSharedMemoryPrivate::create(int size)
{
#ifndef Q_OS_ANDROID
    // build file if needed
    bool createdFile = false;
    int built = createUnixKeyFile(makePlatformSafeKey(key));
    if (built == -1) {
        errorString = QSharedMemory::tr("%1: unable to make key").arg(QLatin1String("QSharedMemory::handle:"));
        error = QSharedMemory::KeyError;
        return false;
    }
    if (built == 1) {
        createdFile = true;
    }

    // get handle
    if (!handle()) {
        if (createdFile)
            QFile::remove(makePlatformSafeKey(key));
        return false;
    }

    // create
    if (-1 == shmget(handle(), size, 0666 | IPC_CREAT | IPC_EXCL)) {
        QString function = QLatin1String("QSharedMemory::create");
        switch (errno) {
        case EINVAL:
            errorString = QSharedMemory::tr("%1: system-imposed size restrictions").arg(QLatin1String("QSharedMemory::handle"));
            error = QSharedMemory::InvalidSize;
            break;
        default:
            setErrorString(function);
        }
        if (createdFile && error != QSharedMemory::AlreadyExists)
            QFile::remove(makePlatformSafeKey(key));
        return false;
    }

    return true;
#else
#warning PLEASE IMPLEMENT !!!
    return false;
#endif
}

bool QSharedMemoryPrivate::attach(QSharedMemory::AccessMode mode)
{
#ifndef Q_OS_ANDROID
    // grab the shared memory segment id
    if (!handle())
        return false;

    int id = shmget(handle(), 0, (mode == QSharedMemory::ReadOnly ? 0444 : 0660));
    if (-1 == id) {
        setErrorString(QLatin1String("QSharedMemory::attach (shmget)"));
        return false;
    }

    // grab the memory
    memory = shmat(id, 0, (mode == QSharedMemory::ReadOnly ? SHM_RDONLY : 0));
    if ((void*) - 1 == memory) {
        memory = 0;
        setErrorString(QLatin1String("QSharedMemory::attach (shmat)"));
        return false;
    }

    // grab the size
    shmid_ds shmid_ds;
    if (!shmctl(id, IPC_STAT, &shmid_ds)) {
        size = (int)shmid_ds.shm_segsz;
    } else {
        setErrorString(QLatin1String("QSharedMemory::attach (shmctl)"));
        return false;
    }

    return true;
#else
#warning PLEASE IMPLEMENT !!!
    return false;
#endif
}

bool QSharedMemoryPrivate::detach()
{
#ifndef Q_OS_ANDROID
    // detach from the memory segment
    if (-1 == shmdt(memory)) {
        QString function = QLatin1String("QSharedMemory::detach");
        switch (errno) {
        case EINVAL:
            errorString = QSharedMemory::tr("%1: not attached").arg(function);
            error = QSharedMemory::NotFound;
            break;
        default:
            setErrorString(function);
        }
        return false;
    }
    memory = 0;

    // Get the number of current attachments
    if (!handle())
        return false;
    int id = shmget(handle(), 0, 0444);
    unix_key = 0;

    struct shmid_ds shmid_ds;
    if (0 != shmctl(id, IPC_STAT, &shmid_ds)) {
        switch (errno) {
        case EINVAL:
            return true;
        default:
            return false;
        }
    }
    // If there are no attachments then remove it.
    if (shmid_ds.shm_nattch == 0) {
        // mark for removal
        if (-1 == shmctl(id, IPC_RMID, &shmid_ds)) {
            setErrorString(QLatin1String("QSharedMemory::remove"));
            switch (errno) {
            case EINVAL:
                return true;
            default:
                return false;
            }
        }

        // remove file
        if (!QFile::remove(makePlatformSafeKey(key)))
            return false;
    }
    return true;
#else
#warning PLEASE IMPLEMENT !!!
    return false;
#endif
}


QT_END_NAMESPACE

#endif // QT_NO_SHAREDMEMORY
