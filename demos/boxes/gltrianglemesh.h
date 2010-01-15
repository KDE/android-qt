/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#ifndef GLTRIANGLEMESH_H
#define GLTRIANGLEMESH_H

//#include <GL/glew.h>
#include "glextensions.h"

#include <QtGui>
#include <QtOpenGL>

#include "glbuffers.h"

template<class TVertex, class TIndex>
class GLTriangleMesh
{
public:
    GLTriangleMesh(int vertexCount, int indexCount) : m_vb(vertexCount), m_ib(indexCount)
    {
    }

    virtual ~GLTriangleMesh()
    {
    }

    virtual void draw()
    {
        if (failed())
            return;

        int type = GL_UNSIGNED_INT;
        if (sizeof(TIndex) == sizeof(char)) type = GL_UNSIGNED_BYTE;
        if (sizeof(TIndex) == sizeof(short)) type = GL_UNSIGNED_SHORT;

        m_vb.bind();
        m_ib.bind();
        glDrawElements(GL_TRIANGLES, m_ib.length(), type, BUFFER_OFFSET(0));
        m_vb.unbind();
        m_ib.unbind();
    }

    bool failed()
    {
        return m_vb.failed() || m_ib.failed();
    }
protected:
    GLVertexBuffer<TVertex> m_vb;
    GLIndexBuffer<TIndex> m_ib;
};


#endif
