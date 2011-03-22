/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __RESOURCEMONINTERFACE_H__
#define __RESOURCEMONINTERFACE_H__

class ResourceMonitorInterface
{
public:
    struct MemoryAllocation
    {
    int allocatedInAppThread;
    int numberOfAllocatedCellsInAppThread;
    int availableMemoryInAppThreadHeap;
    qint64 availableMemoryInSystem;
    qint64 totalMemoryInSystem;
    MemoryAllocation() : 
        allocatedInAppThread(0),
        numberOfAllocatedCellsInAppThread(0),
        availableMemoryInAppThreadHeap(0),
        availableMemoryInSystem(0),
        totalMemoryInSystem(0)
        {}
    };
    
    struct CpuUsage
    {
    qreal systemUsage;
    qreal appTreadUsage;
    CpuUsage() : 
        systemUsage(0.0),
        appTreadUsage(0.0)
        {}
    };
    
public:
    virtual ~ResourceMonitorInterface() {}

public:
    //for symbian, prepares the resource monitor for data capture, opens handle to ekern null
    //thread and sets initial values
    virtual bool Prepare(QString applicationThreadName) = 0;
    
    //functions for CPU load and memory - Call Prepare before calling these
    virtual CpuUsage CPULoad()=0;
    virtual MemoryAllocation MemoryLoad()=0;
    
    virtual void BeginMeasureMemoryLoad()=0;
    virtual MemoryAllocation EndMeasureMemoryLoad()=0;
    
    virtual void BeginMeasureCPULoad()=0;
    virtual CpuUsage EndMeasureCPULoad()=0;
    
};

Q_DECLARE_INTERFACE(ResourceMonitorInterface,
                     "com.trolltech.Plugin.ResourceMonitorInterface/1.0");

#endif //__RESOURCEMONINTERFACE_H__
