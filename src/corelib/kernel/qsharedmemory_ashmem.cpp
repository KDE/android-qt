/*
I BogDan Vatra < bog_dan_ro@yahoo.com >, the copyright holder of this work,
hereby release it into the public domain. This applies worldwide.

In case this is not legally possible, I grant any entity the right to use
this work for any purpose, without any conditions, unless such conditions
are required by law.
*/

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
#include <sys/mman.h>

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
    if (ashmem_fd<0)
	return false;
    munmap(memory, size);
    close(ashmem_fd);
    return true;
}

bool QSharedMemoryPrivate::create(int sz)
{
    ashmem_fd=ashmem_create_region(makePlatformSafeKey(key).toLatin1(),sz);
    if (ashmem_fd<0)
    {
        QString function = QLatin1String("QSharedMemory::create");
        setErrorString(function);
	return false;
    }
    size=sz;
    return true;
}

bool QSharedMemoryPrivate::attach(QSharedMemory::AccessMode mode)
{
    if (ashmem_fd<0)
        return false;
    int prot=(mode == QSharedMemory::ReadOnly)?PROT_READ:PROT_READ | PROT_WRITE;

    if (old_prot != prot)
    {
        if(memory && memory != MAP_FAILED)
            munmap(memory, size);

        if (ashmem_set_prot_region(ashmem_fd,prot)<0)
            return false;
        
        if ((memory = mmap(0, size, prot, MAP_SHARED, ashmem_fd, 0))==MAP_FAILED)
            return false;
        
        old_prot = prot;
    }
    else
        if (ashmem_pin_region(ashmem_fd, 0, 0)<0)
            return false;

    return true;
}

bool QSharedMemoryPrivate::detach()
{
    if (ashmem_unpin_region(ashmem_fd, 0, 0)<0)
	return false;
    return true;
}


QT_END_NAMESPACE

#endif // QT_NO_SHAREDMEMORY
