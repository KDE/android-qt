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

#include "qfontengine_coretext_p.h"

#include <QtCore/qendian.h>
#include <QtCore/qsettings.h>

#include <private/qimage_p.h>

#if !defined(Q_WS_MAC) || (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5)

QT_BEGIN_NAMESPACE

static float SYNTHETIC_ITALIC_SKEW = tanf(14 * acosf(0) / 90);

QCoreTextFontEngineMulti::QCoreTextFontEngineMulti(const QCFString &name, const QFontDef &fontDef, bool kerning)
    : QFontEngineMulti(0)
{
    this->fontDef = fontDef;
    CTFontSymbolicTraits symbolicTraits = 0;
    if (fontDef.weight >= QFont::Bold)
        symbolicTraits |= kCTFontBoldTrait;
    switch (fontDef.style) {
    case QFont::StyleNormal:
        break;
    case QFont::StyleItalic:
    case QFont::StyleOblique:
        symbolicTraits |= kCTFontItalicTrait;
        break;
    }

    transform = CGAffineTransformIdentity;
    if (fontDef.stretch != 100) {
        transform = CGAffineTransformMakeScale(float(fontDef.stretch) / float(100), 1);
    }

    QCFType<CTFontDescriptorRef> descriptor = CTFontDescriptorCreateWithNameAndSize(name, fontDef.pixelSize);
    QCFType<CTFontRef> baseFont = CTFontCreateWithFontDescriptor(descriptor, fontDef.pixelSize, &transform);
    ctfont = CTFontCreateCopyWithSymbolicTraits(baseFont, fontDef.pixelSize, &transform, symbolicTraits, symbolicTraits);

    // CTFontCreateCopyWithSymbolicTraits returns NULL if we ask for a trait that does
    // not exist for the given font. (for example italic)
    if (ctfont == 0) {
        ctfont = baseFont;
        CFRetain(ctfont);
    }

    attributeDict = CFDictionaryCreateMutable(0, 2,
                                       &kCFTypeDictionaryKeyCallBacks,
                                       &kCFTypeDictionaryValueCallBacks);
    CFDictionaryAddValue(attributeDict, NSFontAttributeName, ctfont);
    if (!kerning) {
        float zero = 0.0;
        QCFType<CFNumberRef> noKern = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &zero);
        CFDictionaryAddValue(attributeDict, kCTKernAttributeName, noKern);
    }

    QCoreTextFontEngine *fe = new QCoreTextFontEngine(ctfont, fontDef, this);
    fe->ref.ref();
    engines.append(fe);

}

QCoreTextFontEngineMulti::~QCoreTextFontEngineMulti()
{
    CFRelease(ctfont);
}

uint QCoreTextFontEngineMulti::fontIndexForFont(CTFontRef id) const
{
    for (int i = 0; i < engines.count(); ++i) {
        if (CFEqual(engineAt(i)->ctfont, id))
            return i;
    }

    QCoreTextFontEngineMulti *that = const_cast<QCoreTextFontEngineMulti *>(this);
    QCoreTextFontEngine *fe = new QCoreTextFontEngine(id, fontDef, that);
    fe->ref.ref();
    that->engines.append(fe);
    return engines.count() - 1;
}

bool QCoreTextFontEngineMulti::stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags,
                  unsigned short *logClusters, const HB_CharAttributes *) const
{
    QCFType<CFStringRef> cfstring = CFStringCreateWithCharactersNoCopy(0,
                                                               reinterpret_cast<const UniChar *>(str),
                                                               len, kCFAllocatorNull);
    QCFType<CFAttributedStringRef> attributedString = CFAttributedStringCreate(0, cfstring, attributeDict);
    QCFType<CTTypesetterRef> typeSetter = CTTypesetterCreateWithAttributedString(attributedString);
    CFRange range = {0, 0};
    QCFType<CTLineRef> line = CTTypesetterCreateLine(typeSetter, range);
    CFArrayRef array = CTLineGetGlyphRuns(line);
    uint arraySize = CFArrayGetCount(array);
    glyph_t *outGlyphs = glyphs->glyphs;
    HB_GlyphAttributes *outAttributes = glyphs->attributes;
    QFixed *outAdvances_x = glyphs->advances_x;
    QFixed *outAdvances_y = glyphs->advances_y;
    glyph_t *initialGlyph = outGlyphs;

    if (arraySize == 0) {
        // CoreText failed to shape the text we gave it, so we assume one glyph
        // per character and build a list of invalid glyphs with zero advance
        *nglyphs = len;
        for (int i = 0; i < len; ++i) {
            outGlyphs[i] = 0;
            if (logClusters)
                logClusters[i] = i;
            outAdvances_x[i] = QFixed();
            outAdvances_y[i] = QFixed();
            outAttributes[i].clusterStart = true;
        }
        return true;
    }

    const bool rtl = (CTRunGetStatus(static_cast<CTRunRef>(CFArrayGetValueAtIndex(array, 0))) & kCTRunStatusRightToLeft);

    bool outOBounds = false;
    for (uint i = 0; i < arraySize; ++i) {
        CTRunRef run = static_cast<CTRunRef>(CFArrayGetValueAtIndex(array, rtl ? (arraySize - 1 - i) : i));
        CFIndex glyphCount = CTRunGetGlyphCount(run);
        if (glyphCount == 0)
            continue;

        Q_ASSERT((CTRunGetStatus(run) & kCTRunStatusRightToLeft) == rtl);
        CFRange stringRange = CTRunGetStringRange(run);
        UniChar endGlyph = CFStringGetCharacterAtIndex(cfstring, stringRange.location + stringRange.length - 1);
        bool endWithPDF = QChar::direction(endGlyph) == QChar::DirPDF;
        if (endWithPDF)
            glyphCount++;

        if (!outOBounds && outGlyphs + glyphCount - initialGlyph > *nglyphs) {
            outOBounds = true;
        }
        if (!outOBounds) {
            CFDictionaryRef runAttribs = CTRunGetAttributes(run);
            //NSLog(@"Dictionary %@", runAttribs);
            if (!runAttribs)
                runAttribs = attributeDict;
            CTFontRef runFont = static_cast<CTFontRef>(CFDictionaryGetValue(runAttribs, NSFontAttributeName));
            const uint fontIndex = (fontIndexForFont(runFont) << 24);
            //NSLog(@"Run Font Name = %@", CTFontCopyFamilyName(runFont));
            if (endWithPDF)
                glyphCount--;

            QVarLengthArray<CGGlyph, 512> cgglyphs(0);
            const CGGlyph *tmpGlyphs = CTRunGetGlyphsPtr(run);
            if (!tmpGlyphs) {
                cgglyphs.resize(glyphCount);
                CTRunGetGlyphs(run, range, cgglyphs.data());
                tmpGlyphs = cgglyphs.constData();
            }
            QVarLengthArray<CGPoint, 512> cgpoints(0);
            const CGPoint *tmpPoints = CTRunGetPositionsPtr(run);
            if (!tmpPoints) {
                cgpoints.resize(glyphCount);
                CTRunGetPositions(run, range, cgpoints.data());
                tmpPoints = cgpoints.constData();
            }

            const int rtlOffset = rtl ? (glyphCount - 1) : 0;
            const int rtlSign = rtl ? -1 : 1;

            if (logClusters) {
                CFRange stringRange = CTRunGetStringRange(run);
                QVarLengthArray<CFIndex, 512> stringIndices(0);
                const CFIndex *tmpIndices = CTRunGetStringIndicesPtr(run);
                if (!tmpIndices) {
                    stringIndices.resize(glyphCount);
                    CTRunGetStringIndices(run, range, stringIndices.data());
                    tmpIndices = stringIndices.constData();
                }

                const int firstGlyphIndex = outGlyphs - initialGlyph;
                outAttributes[0].clusterStart = true;

                CFIndex k = 0;
                CFIndex i = 0;
                for (i = stringRange.location;
                     (i < stringRange.location + stringRange.length) && (k < glyphCount); ++i) {
                    if (tmpIndices[k * rtlSign + rtlOffset] == i || i == stringRange.location) {
                        logClusters[i] = k + firstGlyphIndex;
                        outAttributes[k].clusterStart = true;
                        ++k;
                    } else {
                        logClusters[i] = k + firstGlyphIndex - 1;
                    }
                }
                // in case of a ligature at the end, fill the remaining logcluster entries
                for (;i < stringRange.location + stringRange.length; i++) {
                    logClusters[i] = k + firstGlyphIndex - 1;
                }
            }
            for (CFIndex i = 0; i < glyphCount - 1; ++i) {
                int idx = rtlOffset + rtlSign * i;
                outGlyphs[idx] = tmpGlyphs[i] | fontIndex;
                outAdvances_x[idx] = QFixed::fromReal(tmpPoints[i + 1].x - tmpPoints[i].x);
                outAdvances_y[idx] = QFixed::fromReal(tmpPoints[i + 1].y - tmpPoints[i].y);

                if (fontDef.styleStrategy & QFont::ForceIntegerMetrics) {
                    outAdvances_x[idx] = outAdvances_x[idx].round();
                    outAdvances_y[idx] = outAdvances_y[idx].round();
                }
            }
            CGSize lastGlyphAdvance;
            CTFontGetAdvancesForGlyphs(runFont, kCTFontHorizontalOrientation, tmpGlyphs + glyphCount - 1, &lastGlyphAdvance, 1);

            outGlyphs[rtl ? 0 : (glyphCount - 1)] = tmpGlyphs[glyphCount - 1] | fontIndex;
            outAdvances_x[rtl ? 0 : (glyphCount - 1)] =
                    (fontDef.styleStrategy & QFont::ForceIntegerMetrics)
                    ? QFixed::fromReal(lastGlyphAdvance.width).round()
                    : QFixed::fromReal(lastGlyphAdvance.width);

            if (endWithPDF) {
                logClusters[stringRange.location + stringRange.length - 1] = glyphCount;
                outGlyphs[glyphCount] = 0xFFFF;
                outAdvances_x[glyphCount] = 0;
                outAdvances_y[glyphCount] = 0;
                outAttributes[glyphCount].clusterStart = true;
                outAttributes[glyphCount].dontPrint = true;
                glyphCount++;
            }
        }
        outGlyphs += glyphCount;
        outAttributes += glyphCount;
        outAdvances_x += glyphCount;
        outAdvances_y += glyphCount;
    }
    *nglyphs = (outGlyphs - initialGlyph);
    return !outOBounds;
}

bool QCoreTextFontEngineMulti::stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs,
                                            int *nglyphs, QTextEngine::ShaperFlags flags) const
{
    *nglyphs = len;
    QCFType<CFStringRef> cfstring;

    QVarLengthArray<CGGlyph> cgGlyphs(len);
    CTFontGetGlyphsForCharacters(ctfont, (const UniChar*)str, cgGlyphs.data(), len);

    for (int i = 0; i < len; ++i) {
        if (cgGlyphs[i]) {
            glyphs->glyphs[i] = cgGlyphs[i];
        } else {
            if (!cfstring)
                cfstring = CFStringCreateWithCharactersNoCopy(0, reinterpret_cast<const UniChar *>(str), len, kCFAllocatorNull);
            QCFType<CTFontRef> substituteFont = CTFontCreateForString(ctfont, cfstring, CFRangeMake(i, 1));
            CGGlyph substituteGlyph = 0;
            CTFontGetGlyphsForCharacters(substituteFont, (const UniChar*)str + i, &substituteGlyph, 1);
            if (substituteGlyph) {
                const uint fontIndex = (fontIndexForFont(substituteFont) << 24);
                glyphs->glyphs[i] = substituteGlyph | fontIndex;
                if (!(flags & QTextEngine::GlyphIndicesOnly)) {
                    CGSize advance;
                    CTFontGetAdvancesForGlyphs(substituteFont, kCTFontHorizontalOrientation, &substituteGlyph, &advance, 1);
                    glyphs->advances_x[i] = QFixed::fromReal(advance.width);
                    glyphs->advances_y[i] = QFixed::fromReal(advance.height);
                }
            }
        }
    }

    if (flags & QTextEngine::GlyphIndicesOnly)
        return true;

    QVarLengthArray<CGSize> advances(len);
    CTFontGetAdvancesForGlyphs(ctfont, kCTFontHorizontalOrientation, cgGlyphs.data(), advances.data(), len);

    for (int i = 0; i < len; ++i) {
        if (glyphs->glyphs[i] & 0xff000000)
            continue;
        glyphs->advances_x[i] = QFixed::fromReal(advances[i].width);
        glyphs->advances_y[i] = QFixed::fromReal(advances[i].height);
    }

    if (fontDef.styleStrategy & QFont::ForceIntegerMetrics) {
        for (int i = 0; i < len; ++i) {
            glyphs->advances_x[i] = glyphs->advances_x[i].round();
            glyphs->advances_y[i] = glyphs->advances_y[i].round();
        }
    }

    return true;
}

void QCoreTextFontEngineMulti::recalcAdvances(int , QGlyphLayout *, QTextEngine::ShaperFlags) const
{
}
void QCoreTextFontEngineMulti::doKerning(int , QGlyphLayout *, QTextEngine::ShaperFlags) const
{
}

void QCoreTextFontEngineMulti::loadEngine(int)
{
    // Do nothing
    Q_ASSERT(false);
}



QCoreTextFontEngine::QCoreTextFontEngine(CTFontRef font, const QFontDef &def,
                                         QCoreTextFontEngineMulti *multiEngine)
{
    fontDef = def;
    parentEngine = multiEngine;
    synthesisFlags = 0;
    ctfont = font;
    CFRetain(ctfont);
    cgFont = CTFontCopyGraphicsFont(ctfont, NULL);
    CTFontSymbolicTraits traits = CTFontGetSymbolicTraits(ctfont);
    if (fontDef.weight >= QFont::Bold && !(traits & kCTFontBoldTrait)) {
         synthesisFlags |= SynthesizedBold;
    }

    if (fontDef.style != QFont::StyleNormal && !(traits & kCTFontItalicTrait)) {
        synthesisFlags |= SynthesizedItalic;
    }
    transform = CGAffineTransformIdentity;
    if (fontDef.stretch != 100) {
        transform = CGAffineTransformMakeScale(float(fontDef.stretch) / float(100), 1);
    }
    QByteArray os2Table = getSfntTable(MAKE_TAG('O', 'S', '/', '2'));
    if (os2Table.size() >= 10)
        fsType = qFromBigEndian<quint16>(reinterpret_cast<const uchar *>(os2Table.constData() + 8));

     QSettings appleSettings(QLatin1String("apple.com"));
     QVariant appleValue = appleSettings.value(QLatin1String("AppleAntiAliasingThreshold"));
     if (appleValue.isValid())
         antialiasing_threshold = appleValue.toInt();
     else
         antialiasing_threshold = -1;
}

QCoreTextFontEngine::~QCoreTextFontEngine()
{
    CFRelease(cgFont);
    CFRelease(ctfont);
}

bool QCoreTextFontEngine::stringToCMap(const QChar *, int, QGlyphLayout *, int *, QTextEngine::ShaperFlags) const
{
    return false;
}

glyph_metrics_t QCoreTextFontEngine::boundingBox(const QGlyphLayout &glyphs)
{
    QFixed w;
    bool round = fontDef.styleStrategy & QFont::ForceIntegerMetrics;

    for (int i = 0; i < glyphs.numGlyphs; ++i) {
        w += round ? glyphs.effectiveAdvance(i).round()
                   : glyphs.effectiveAdvance(i);
    }
    return glyph_metrics_t(0, -(ascent()), w - lastRightBearing(glyphs, round), ascent()+descent(), w, 0);
}
glyph_metrics_t QCoreTextFontEngine::boundingBox(glyph_t glyph)
{
    glyph_metrics_t ret;
    CGGlyph g = glyph;
    CGRect rect = CTFontGetBoundingRectsForGlyphs(ctfont, kCTFontHorizontalOrientation, &g, 0, 1);
    if (synthesisFlags & QFontEngine::SynthesizedItalic) {
        rect.size.width += rect.size.height * SYNTHETIC_ITALIC_SKEW;
    }
    ret.width = QFixed::fromReal(rect.size.width);
    ret.height = QFixed::fromReal(rect.size.height);
    ret.x = QFixed::fromReal(rect.origin.x);
    ret.y = -QFixed::fromReal(rect.origin.y) - ret.height;
    CGSize advances[1];
    CTFontGetAdvancesForGlyphs(ctfont, kCTFontHorizontalOrientation, &g, advances, 1);
    ret.xoff = QFixed::fromReal(advances[0].width);
    ret.yoff = QFixed::fromReal(advances[0].height);

    if (fontDef.styleStrategy & QFont::ForceIntegerMetrics) {
        ret.xoff = ret.xoff.round();
        ret.yoff = ret.yoff.round();
    }

    return ret;
}

QFixed QCoreTextFontEngine::ascent() const
{
    return (fontDef.styleStrategy & QFont::ForceIntegerMetrics)
            ? QFixed::fromReal(CTFontGetAscent(ctfont)).round()
            : QFixed::fromReal(CTFontGetAscent(ctfont));
}
QFixed QCoreTextFontEngine::descent() const
{
    QFixed d = QFixed::fromReal(CTFontGetDescent(ctfont));
    if (fontDef.styleStrategy & QFont::ForceIntegerMetrics)
        d = d.round();

    // subtract a pixel to even out the historical +1 in QFontMetrics::height().
    // Fix in Qt 5.
    return d - 1;
}
QFixed QCoreTextFontEngine::leading() const
{
    return (fontDef.styleStrategy & QFont::ForceIntegerMetrics)
            ? QFixed::fromReal(CTFontGetLeading(ctfont)).round()
            : QFixed::fromReal(CTFontGetLeading(ctfont));
}
QFixed QCoreTextFontEngine::xHeight() const
{
    return (fontDef.styleStrategy & QFont::ForceIntegerMetrics)
            ? QFixed::fromReal(CTFontGetXHeight(ctfont)).round()
            : QFixed::fromReal(CTFontGetXHeight(ctfont));
}
QFixed QCoreTextFontEngine::averageCharWidth() const
{
    // ### Need to implement properly and get the information from the OS/2 Table.
    return (fontDef.styleStrategy & QFont::ForceIntegerMetrics)
            ? QFontEngine::averageCharWidth().round()
            : QFontEngine::averageCharWidth();
}

qreal QCoreTextFontEngine::maxCharWidth() const
{
    // ### Max Help!
    return 0;

}
qreal QCoreTextFontEngine::minLeftBearing() const
{
    // ### Min Help!
    return 0;

}
qreal QCoreTextFontEngine::minRightBearing() const
{
    // ### Max Help! (even thought it's right)
    return 0;

}

void QCoreTextFontEngine::draw(CGContextRef ctx, qreal x, qreal y, const QTextItemInt &ti, int paintDeviceHeight)
{
    QVarLengthArray<QFixedPoint> positions;
    QVarLengthArray<glyph_t> glyphs;
    QTransform matrix;
    matrix.translate(x, y);
    getGlyphPositions(ti.glyphs, matrix, ti.flags, glyphs, positions);
    if (glyphs.size() == 0)
        return;

    CGContextSetFontSize(ctx, fontDef.pixelSize);

    CGAffineTransform oldTextMatrix = CGContextGetTextMatrix(ctx);

    CGAffineTransform cgMatrix = CGAffineTransformMake(1, 0, 0, -1, 0, -paintDeviceHeight);

    CGAffineTransformConcat(cgMatrix, oldTextMatrix);

    if (synthesisFlags & QFontEngine::SynthesizedItalic)
        cgMatrix = CGAffineTransformConcat(cgMatrix, CGAffineTransformMake(1, 0, -SYNTHETIC_ITALIC_SKEW, 1, 0, 0));

    cgMatrix = CGAffineTransformConcat(cgMatrix, transform);

    CGContextSetTextMatrix(ctx, cgMatrix);

    CGContextSetTextDrawingMode(ctx, kCGTextFill);


    QVarLengthArray<CGSize> advances(glyphs.size());
    QVarLengthArray<CGGlyph> cgGlyphs(glyphs.size());

    for (int i = 0; i < glyphs.size() - 1; ++i) {
        advances[i].width = (positions[i + 1].x - positions[i].x).toReal();
        advances[i].height = (positions[i + 1].y - positions[i].y).toReal();
        cgGlyphs[i] = glyphs[i];
    }
    advances[glyphs.size() - 1].width = 0;
    advances[glyphs.size() - 1].height = 0;
    cgGlyphs[glyphs.size() - 1] = glyphs[glyphs.size() - 1];

    CGContextSetFont(ctx, cgFont);
    //NSLog(@"Font inDraw %@  ctfont %@", CGFontCopyFullName(cgFont), CTFontCopyFamilyName(ctfont));

    CGContextSetTextPosition(ctx, positions[0].x.toReal(), positions[0].y.toReal());

    CGContextShowGlyphsWithAdvances(ctx, cgGlyphs.data(), advances.data(), glyphs.size());

    if (synthesisFlags & QFontEngine::SynthesizedBold) {
        CGContextSetTextPosition(ctx, positions[0].x.toReal() + 0.5 * lineThickness().toReal(),
                                 positions[0].y.toReal());

        CGContextShowGlyphsWithAdvances(ctx, cgGlyphs.data(), advances.data(), glyphs.size());
    }

    CGContextSetTextMatrix(ctx, oldTextMatrix);
}

struct ConvertPathInfo
{
    ConvertPathInfo(QPainterPath *newPath, const QPointF &newPos) : path(newPath), pos(newPos) {}
    QPainterPath *path;
    QPointF pos;
};

static void convertCGPathToQPainterPath(void *info, const CGPathElement *element)
{
    ConvertPathInfo *myInfo = static_cast<ConvertPathInfo *>(info);
    switch(element->type) {
        case kCGPathElementMoveToPoint:
            myInfo->path->moveTo(element->points[0].x + myInfo->pos.x(),
                                 element->points[0].y + myInfo->pos.y());
            break;
        case kCGPathElementAddLineToPoint:
            myInfo->path->lineTo(element->points[0].x + myInfo->pos.x(),
                                 element->points[0].y + myInfo->pos.y());
            break;
        case kCGPathElementAddQuadCurveToPoint:
            myInfo->path->quadTo(element->points[0].x + myInfo->pos.x(),
                                 element->points[0].y + myInfo->pos.y(),
                                 element->points[1].x + myInfo->pos.x(),
                                 element->points[1].y + myInfo->pos.y());
            break;
        case kCGPathElementAddCurveToPoint:
            myInfo->path->cubicTo(element->points[0].x + myInfo->pos.x(),
                                  element->points[0].y + myInfo->pos.y(),
                                  element->points[1].x + myInfo->pos.x(),
                                  element->points[1].y + myInfo->pos.y(),
                                  element->points[2].x + myInfo->pos.x(),
                                  element->points[2].y + myInfo->pos.y());
            break;
        case kCGPathElementCloseSubpath:
            myInfo->path->closeSubpath();
            break;
        default:
            qDebug() << "Unhandled path transform type: " << element->type;
    }

}

void QCoreTextFontEngine::addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int nGlyphs,
                                          QPainterPath *path, QTextItem::RenderFlags)
{
    CGAffineTransform cgMatrix = CGAffineTransformIdentity;
    cgMatrix = CGAffineTransformScale(cgMatrix, 1, -1);

    if (synthesisFlags & QFontEngine::SynthesizedItalic)
        cgMatrix = CGAffineTransformConcat(cgMatrix, CGAffineTransformMake(1, 0, -SYNTHETIC_ITALIC_SKEW, 1, 0, 0));

    for (int i = 0; i < nGlyphs; ++i) {
        QCFType<CGPathRef> cgpath = CTFontCreatePathForGlyph(ctfont, glyphs[i], &cgMatrix);
        ConvertPathInfo info(path, positions[i].toPointF());
        CGPathApply(cgpath, &info, convertCGPathToQPainterPath);
    }
}

QFont QCoreTextFontEngine::createExplicitFont() const
{
    QString familyName = QCFString::toQString(CTFontCopyFamilyName(ctfont));
    return createExplicitFontWithName(familyName);
}

QImage QCoreTextFontEngine::imageForGlyph(glyph_t glyph, QFixed subPixelPosition, int /*margin*/, bool aa)
{
    const glyph_metrics_t br = boundingBox(glyph);
    QImage im(qRound(br.width)+2, qRound(br.height)+2, QImage::Format_RGB32);
    im.fill(0);

    CGColorSpaceRef colorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    uint cgflags = kCGImageAlphaNoneSkipFirst;
#ifdef kCGBitmapByteOrder32Host //only needed because CGImage.h added symbols in the minor version
    cgflags |= kCGBitmapByteOrder32Host;
#endif
    CGContextRef ctx = CGBitmapContextCreate(im.bits(), im.width(), im.height(),
                                             8, im.bytesPerLine(), colorspace,
                                             cgflags);
    CGContextSetFontSize(ctx, fontDef.pixelSize);
    CGContextSetShouldAntialias(ctx, aa ||
                                (fontDef.pointSize > antialiasing_threshold
                                 && !(fontDef.styleStrategy & QFont::NoAntialias)));
    CGContextSetShouldSmoothFonts(ctx, aa);
    CGAffineTransform oldTextMatrix = CGContextGetTextMatrix(ctx);
    CGAffineTransform cgMatrix = CGAffineTransformMake(1, 0, 0, 1, 0, 0);

    CGAffineTransformConcat(cgMatrix, oldTextMatrix);

    if (synthesisFlags & QFontEngine::SynthesizedItalic)
        cgMatrix = CGAffineTransformConcat(cgMatrix, CGAffineTransformMake(1, 0, SYNTHETIC_ITALIC_SKEW, 1, 0, 0));

    cgMatrix = CGAffineTransformConcat(cgMatrix, transform);

    CGContextSetTextMatrix(ctx, cgMatrix);
    CGContextSetRGBFillColor(ctx, 1, 1, 1, 1);
    CGContextSetTextDrawingMode(ctx, kCGTextFill);

    CGContextSetFont(ctx, cgFont);

    qreal pos_x = -br.x.toReal() + subPixelPosition.toReal();
    qreal pos_y = im.height() + br.y.toReal() - 1;
    CGContextSetTextPosition(ctx, pos_x, pos_y);

    CGSize advance;
    advance.width = 0;
    advance.height = 0;
    CGGlyph cgGlyph = glyph;
    CGContextShowGlyphsWithAdvances(ctx, &cgGlyph, &advance, 1);

    if (synthesisFlags & QFontEngine::SynthesizedBold) {
        CGContextSetTextPosition(ctx, pos_x + 0.5 * lineThickness().toReal(), pos_y);
        CGContextShowGlyphsWithAdvances(ctx, &cgGlyph, &advance, 1);
    }

    CGContextRelease(ctx);

    return im;
}

QImage QCoreTextFontEngine::alphaMapForGlyph(glyph_t glyph, QFixed subPixelPosition)
{
    QImage im = imageForGlyph(glyph, subPixelPosition, 0, false);

    QImage indexed(im.width(), im.height(), QImage::Format_Indexed8);
    QVector<QRgb> colors(256);
    for (int i=0; i<256; ++i)
        colors[i] = qRgba(0, 0, 0, i);
    indexed.setColorTable(colors);

    for (int y=0; y<im.height(); ++y) {
        uint *src = (uint*) im.scanLine(y);
        uchar *dst = indexed.scanLine(y);
        for (int x=0; x<im.width(); ++x) {
            *dst = qGray(*src);
            ++dst;
            ++src;
        }
    }

    return indexed;
}

QImage QCoreTextFontEngine::alphaRGBMapForGlyph(glyph_t glyph, QFixed subPixelPosition, int margin, const QTransform &x)
{
    if (x.type() >= QTransform::TxScale)
        return QFontEngine::alphaRGBMapForGlyph(glyph, subPixelPosition, margin, x);

    QImage im = imageForGlyph(glyph, subPixelPosition, margin, true);
    qGamma_correct_back_to_linear_cs(&im);
    return im;
}

void QCoreTextFontEngine::recalcAdvances(int numGlyphs, QGlyphLayout *glyphs, QTextEngine::ShaperFlags flags) const
{
    Q_ASSERT(false);
    Q_UNUSED(numGlyphs);
    Q_UNUSED(glyphs);
    Q_UNUSED(flags);
}

QFontEngine::FaceId QCoreTextFontEngine::faceId() const
{
    return QFontEngine::FaceId();
}

bool QCoreTextFontEngine::canRender(const QChar *string, int len)
{
    QCFType<CTFontRef> retFont = CTFontCreateForString(ctfont,
                  QCFType<CFStringRef>(CFStringCreateWithCharactersNoCopy(0,
                                                              reinterpret_cast<const UniChar *>(string),
                                                                          len, kCFAllocatorNull)),
                          CFRangeMake(0, len));
    return retFont != 0;
    return false;
}

 bool QCoreTextFontEngine::getSfntTableData(uint tag, uchar *buffer, uint *length) const
 {
     QCFType<CFDataRef> table = CTFontCopyTable(ctfont, tag, 0);
     if (!table || !length)
         return false;
     CFIndex tableLength = CFDataGetLength(table);
     int availableLength = *length;
     *length = tableLength;
     if (buffer) {
         if (tableLength > availableLength)
             return false;
         CFDataGetBytes(table, CFRangeMake(0, tableLength), buffer);
     }
     return true;
 }

void QCoreTextFontEngine::getUnscaledGlyph(glyph_t, QPainterPath *, glyph_metrics_t *)
{
    // ###
}

QT_END_NAMESPACE

#endif// !defined(Q_WS_MAC) || (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5)

