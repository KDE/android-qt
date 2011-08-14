/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the config.tests of the Qt Toolkit.
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

/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <linux/ashmem.h>

#include <semaphore.h>

#define ASHMEM_DEVICE   "/dev/ashmem"

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

int main(int, char **)
{
    sem_t *semaphore = sem_open("test", O_CREAT | O_EXCL, 0666, 0);
    if (semaphore != SEM_FAILED)
        sem_close(semaphore);

    int shmemfd = ashmem_create_region("test", 64);
    if (shmemfd >= 0)
        close(shmemfd);

    return 0;
}
