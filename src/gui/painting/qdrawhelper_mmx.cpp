/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <private/qdrawhelper_p.h>

#if defined(QT_HAVE_MMX)

#include <private/qdrawhelper_mmx_p.h>

QT_BEGIN_NAMESPACE

CompositionFunctionSolid qt_functionForModeSolid_MMX[numCompositionFunctions] = {
    comp_func_solid_SourceOver<QMMXIntrinsics>,
    comp_func_solid_DestinationOver<QMMXIntrinsics>,
    comp_func_solid_Clear<QMMXIntrinsics>,
    comp_func_solid_Source<QMMXIntrinsics>,
    0,
    comp_func_solid_SourceIn<QMMXIntrinsics>,
    comp_func_solid_DestinationIn<QMMXIntrinsics>,
    comp_func_solid_SourceOut<QMMXIntrinsics>,
    comp_func_solid_DestinationOut<QMMXIntrinsics>,
    comp_func_solid_SourceAtop<QMMXIntrinsics>,
    comp_func_solid_DestinationAtop<QMMXIntrinsics>,
    comp_func_solid_XOR<QMMXIntrinsics>,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // svg 1.2 modes
    rasterop_solid_SourceOrDestination<QMMXIntrinsics>,
    rasterop_solid_SourceAndDestination<QMMXIntrinsics>,
    rasterop_solid_SourceXorDestination<QMMXIntrinsics>,
    rasterop_solid_NotSourceAndNotDestination<QMMXIntrinsics>,
    rasterop_solid_NotSourceOrNotDestination<QMMXIntrinsics>,
    rasterop_solid_NotSourceXorDestination<QMMXIntrinsics>,
    rasterop_solid_NotSource<QMMXIntrinsics>,
    rasterop_solid_NotSourceAndDestination<QMMXIntrinsics>,
    rasterop_solid_SourceAndNotDestination<QMMXIntrinsics>
};

CompositionFunction qt_functionForMode_MMX[numCompositionFunctions] = {
    comp_func_SourceOver<QMMXIntrinsics>,
    comp_func_DestinationOver<QMMXIntrinsics>,
    comp_func_Clear<QMMXIntrinsics>,
    comp_func_Source<QMMXIntrinsics>,
    0,
    comp_func_SourceIn<QMMXIntrinsics>,
    comp_func_DestinationIn<QMMXIntrinsics>,
    comp_func_SourceOut<QMMXIntrinsics>,
    comp_func_DestinationOut<QMMXIntrinsics>,
    comp_func_SourceAtop<QMMXIntrinsics>,
    comp_func_DestinationAtop<QMMXIntrinsics>,
    comp_func_XOR<QMMXIntrinsics>,
};

void qt_blend_color_argb_mmx(int count, const QSpan *spans, void *userData)
{
    qt_blend_color_argb_x86<QMMXIntrinsics>(count, spans, userData,
                                            (CompositionFunctionSolid*)qt_functionForModeSolid_MMX);
}


void qt_blend_argb32_on_argb32_mmx(uchar *destPixels, int dbpl,
                                   const uchar *srcPixels, int sbpl,
                                   int w, int h,
                                   int const_alpha)
{
    const uint *src = (const uint *) srcPixels;
    uint *dst = (uint *) destPixels;

    uint ca = const_alpha - 1;

    for (int y=0; y<h; ++y) {
        comp_func_SourceOver<QMMXIntrinsics>(dst, src, w, ca);
        dst = (quint32 *)(((uchar *) dst) + dbpl);
        src = (const quint32 *)(((const uchar *) src) + sbpl);
    }
}

void qt_blend_rgb32_on_rgb32_mmx(uchar *destPixels, int dbpl,
                                 const uchar *srcPixels, int sbpl,
                                 int w, int h,
                                 int const_alpha)
{
    const uint *src = (const uint *) srcPixels;
    uint *dst = (uint *) destPixels;

    uint ca = const_alpha - 1;

    for (int y=0; y<h; ++y) {
        comp_func_Source<QMMXIntrinsics>(dst, src, w, ca);
        dst = (quint32 *)(((uchar *) dst) + dbpl);
        src = (const quint32 *)(((const uchar *) src) + sbpl);
    }
}

QT_END_NAMESPACE

#endif // QT_HAVE_MMX

