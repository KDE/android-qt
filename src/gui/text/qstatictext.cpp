/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include "qstatictext.h"
#include "qstatictext_p.h"
#include <private/qtextengine_p.h>
#include <private/qfontengine_p.h>

#include <QtGui/qapplication.h>

QT_BEGIN_NAMESPACE

/*!
    \class QStaticText
    \brief The QStaticText class enables optimized drawing of text when the text and its layout
    is updated rarely.
    \since 4.7

    \ingroup multimedia
    \ingroup text
    \mainclass

    QStaticText provides a way to cache layout data for a block of text so that it can be drawn
    more efficiently than by using QPainter::drawText() in which the layout information is 
    recalculated with every call. 

    The class primarily provides an optimization for cases where the text, its font and the
    transformations on the painter are static over several paint events. If the text or its layout
    is changed for every iteration, QPainter::drawText() is the more efficient alternative, since
    the static text's layout would have to be recalculated to take the new state into consideration.

    Translating the painter will not cause the layout of the text to be recalculated, but will cause
    a very small performance impact on drawStaticText(). Altering any other parts of the painter's
    transformation or the painter's font will cause the layout of the static text to be
    recalculated. This should be avoided as often as possible to maximize the performance
    benefit of using QStaticText.

    In addition, only affine transformations are supported by drawStaticText(). Calling
    drawStaticText() on a projected painter will perform slightly worse than using the regular
    drawText() call, so this should be avoided.

    \code
    class MyWidget: public QWidget
    {
    public:
        MyWidget(QWidget *parent = 0) : QWidget(parent), m_staticText("This is static text")

    protected:
        void paintEvent(QPaintEvent *)
        {
            QPainter painter(this);
            painter.drawStaticText(0, 0, m_staticText);
        }

    private:
        QStaticText m_staticText;
    };
    \endcode

    The QStaticText class can be used to mimic the behavior of QPainter::drawText() to a specific
    point with no boundaries, and also when QPainter::drawText() is called with a bounding 
    rectangle. 

    If a bounding rectangle is not required, create a QStaticText object without setting a maximum 
    size. The text will then occupy a single line. 

    If you set a maximum size on the QStaticText object, this will bound the text. The text will
    be formatted so that no line exceeds the given width. When the object is painted, it will 
    be clipped at the given size. The position of the text is decided by the argument
    passed to QPainter::drawStaticText() and can change from call to call with a minimal impact
    on performance.

    QStaticText will attempt to guess the format of the input text using Qt::mightBeRichText().
    To force QStaticText to display its contents as either plain text or rich text, use the
    function QStaticText::setTextFormat() and pass in, respectively, Qt::PlainText and
    Qt::RichText.

    \sa QPainter::drawText(), QPainter::drawStaticText(), QTextLayout, QTextDocument
*/

/*!
    \enum QStaticText::PerformanceHint

    This enum the different performance hints that can be set on the QStaticText. These hints
    can be used to indicate that the QStaticText should use additional caches, if possible,
    to improve performance at the expense of memory. In particular, setting the performance hint
    AggressiveCaching on the QStaticText will improve performance when using the OpenGL graphics
    system or when drawing to a QGLWidget.

    \value ModerateCaching Do basic caching for high performance at a low memory cost.
    \value AggressiveCaching Use additional caching when available. This may improve performance
           at a higher memory cost.
*/

/*!
    Constructs an empty QStaticText
*/
QStaticText::QStaticText()    
    : data(new QStaticTextPrivate)
{
}

/*!
    Constructs a QStaticText object with the given \a text and bounded by the given \a size.

    If an invalid size is passed for \a size the text will be unbounded.
*/
QStaticText::QStaticText(const QString &text, const QSizeF &size)
    : data(new QStaticTextPrivate)
{    
    data->text = text;
    data->maximumSize = size;
    data->init();
}

/*!
    Constructs a QStaticText object which is a copy of \a other.
*/
QStaticText::QStaticText(const QStaticText &other)    
{
    data = other.data;
}

/*!
    Destroys the QStaticText.
*/
QStaticText::~QStaticText()
{
    Q_ASSERT(!data || data->ref >= 1);
}

/*!
    \internal
*/
void QStaticText::detach()
{    
    if (data->ref != 1)
        data.detach();
}

/*!
  Prepares the QStaticText object for being painted with the given \a matrix and the given
  \a font to avoid overhead when the actual drawStaticText() call is made.

  When drawStaticText() is called, the layout of the QStaticText will be recalculated if the
  painter's font or matrix is different from the one used for the currently cached layout. By
  default, QStaticText will use a default constructed QFont and an identity matrix to create
  its layout.

  To avoid the overhead of creating the layout the first time you draw the QStaticText with
  a painter whose matrix or font are different from the defaults, you can use the prepare()
  function and pass in the matrix and font you expect to use when drawing the text.

  \sa QPainter::setFont(), QPainter::setMatrix()
*/
void QStaticText::prepare(const QTransform &matrix, const QFont &font)
{
    data->matrix = matrix;
    data->font = font;
    data->init();
}


/*!
    Assigns \a other to this QStaticText.
*/
QStaticText &QStaticText::operator=(const QStaticText &other)
{    
    data = other.data;
    return *this;
}

/*!
    Compares \a other to this QStaticText. Returns true if the texts, fonts and maximum sizes
    are equal.
*/
bool QStaticText::operator==(const QStaticText &other) const
{
    return (data == other.data
            || (data->text == other.data->text
                && data->font == other.data->font
                && data->maximumSize == other.data->maximumSize));
}

/*!
    Compares \a other to this QStaticText. Returns true if the texts, fonts or maximum sizes
    are different.
*/
bool QStaticText::operator!=(const QStaticText &other) const
{
    return !(*this == other);
}

/*!
    Sets the text of the QStaticText to \a text.

    \note This function will cause the layout of the text to be recalculated.

    \sa text()
*/
void QStaticText::setText(const QString &text)
{
    detach();
    data->text = text;
    data->init();
}

/*!
   Sets the text format of the QStaticText to \a textFormat. If \a textFormat is set to
   Qt::AutoText (the default), the format of the text will try to be determined using the
   function Qt::mightBeRichText(). If the text format is Qt::PlainText, then the text will be
   displayed as is, whereas it will be interpreted as HTML if the format is Qt::RichText. HTML tags
   that alter the font of the text, its color, or its layout are supported by QStaticText.

   \note This function will cause the layout of the text to be recalculated.

   \sa textFormat(), setText(), text()
*/
void QStaticText::setTextFormat(Qt::TextFormat textFormat)
{
    detach();
    data->textFormat = textFormat;
    data->init();
}

/*!
  Returns the text format of the QStaticText.

  \sa setTextFormat(), setText(), text()
*/
Qt::TextFormat QStaticText::textFormat() const
{
    return Qt::TextFormat(data->textFormat);
}

/*!
    Returns the text of the QStaticText.

    \sa setText()
*/
QString QStaticText::text() const 
{
    return data->text;
}

/*!
  Sets the performance hint of the QStaticText according to the \a
  performanceHint provided. The \a performanceHint is used to
  customize how much caching is done internally to improve
  performance.

  The default is QStaticText::ModerateCaching.

  \note This function will cause the layout of the text to be recalculated.

  \sa performanceHint()
*/
void QStaticText::setPerformanceHint(PerformanceHint performanceHint)
{
    if ((performanceHint == ModerateCaching && !data->useBackendOptimizations)
        || (performanceHint == AggressiveCaching && data->useBackendOptimizations)) {
        return;
    }
    detach();
    data->useBackendOptimizations = (performanceHint == AggressiveCaching);
    data->init();
}

/*!
  Returns which performance hint is set for the QStaticText.

  \sa setPerformanceHint()
*/
QStaticText::PerformanceHint QStaticText::performanceHint() const
{
    return data->useBackendOptimizations ? AggressiveCaching : ModerateCaching;
}

/*!
    Sets the maximum size of the QStaticText to \a size.

    \note This function will cause the layout of the text to be recalculated.

    \sa maximumSize(), size()
*/
void QStaticText::setMaximumSize(const QSizeF &size)
{
    detach();
    data->maximumSize = size;
    data->init();
}

/*!
    Returns the maximum size of the QStaticText.

    \sa setMaximumSize()
*/
QSizeF QStaticText::maximumSize() const
{
    return data->maximumSize;
}

/*!
  Returns the size of the bounding rect for this QStaticText.

  \sa maximumSize()
*/
QSizeF QStaticText::size() const
{
    return data->actualSize;
}

QStaticTextPrivate::QStaticTextPrivate()
        : items(0), itemCount(0), glyphPool(0), positionPool(0), needsClipRect(false),
          useBackendOptimizations(false), textFormat(Qt::AutoText)
{
}

QStaticTextPrivate::QStaticTextPrivate(const QStaticTextPrivate &other)
    : text(other.text), font(other.font), maximumSize(other.maximumSize), matrix(other.matrix),
      items(0), itemCount(0), glyphPool(0), positionPool(0), needsClipRect(false),
      useBackendOptimizations(other.useBackendOptimizations), textFormat(other.textFormat)
{
}

QStaticTextPrivate::~QStaticTextPrivate()
{
    delete[] items;    
    delete[] glyphPool;
    delete[] positionPool;
}

QStaticTextPrivate *QStaticTextPrivate::get(const QStaticText *q)
{
    return q->data.data();
}

extern int qt_defaultDpiX();
extern int qt_defaultDpiY();

namespace {

    class DrawTextItemRecorder: public QPaintEngine
    {
    public:
        DrawTextItemRecorder(int expectedItemCount, QStaticTextItem *items,
                             int expectedGlyphCount, QFixedPoint *positionPool, glyph_t *glyphPool)
                : m_items(items),
                  m_itemCount(0), m_glyphCount(0),
                  m_expectedItemCount(expectedItemCount),
                  m_expectedGlyphCount(expectedGlyphCount),
                  m_glyphPool(glyphPool),
                  m_positionPool(positionPool)
        {
        }

        virtual void drawTextItem(const QPointF &position, const QTextItem &textItem)
        {
            const QTextItemInt &ti = static_cast<const QTextItemInt &>(textItem);          

            m_itemCount++;
            m_glyphCount += ti.glyphs.numGlyphs;
            if (m_items == 0)
                return;

            Q_ASSERT(m_itemCount <= m_expectedItemCount);
            Q_ASSERT(m_glyphCount <= m_expectedGlyphCount);

            QStaticTextItem *currentItem = (m_items + (m_itemCount - 1));
            currentItem->fontEngine = ti.fontEngine;
            currentItem->font = ti.font();
            currentItem->chars = ti.chars;
            currentItem->numChars = ti.num_chars;
            currentItem->numGlyphs = ti.glyphs.numGlyphs;
            currentItem->glyphs = m_glyphPool;
            currentItem->glyphPositions = m_positionPool;
            currentItem->color = state->pen().color();

            QTransform matrix = state->transform();
            matrix.translate(position.x(), position.y());

            QVarLengthArray<glyph_t> glyphs;
            QVarLengthArray<QFixedPoint> positions;
            ti.fontEngine->getGlyphPositions(ti.glyphs, matrix, ti.flags, glyphs, positions);

            int size = glyphs.size();
            Q_ASSERT(size == ti.glyphs.numGlyphs);
            Q_ASSERT(size == positions.size());

            memmove(currentItem->glyphs, glyphs.constData(), sizeof(glyph_t) * size);
            memmove(currentItem->glyphPositions, positions.constData(), sizeof(QFixedPoint) * size);

            m_glyphPool += size;
            m_positionPool += size;
        }                


        virtual bool begin(QPaintDevice *)  { return true; }
        virtual bool end() { return true; }
        virtual void updateState(const QPaintEngineState &) {}
        virtual void drawPixmap(const QRectF &, const QPixmap &, const QRectF &) {}
        virtual Type type() const
        {
            return User;
        }

        int itemCount() const
        {
            return m_itemCount;
        }

        int glyphCount() const
        {
            return m_glyphCount;
        }

    private:
        QStaticTextItem *m_items;
        int m_itemCount;
        int m_glyphCount;
        int m_expectedItemCount;
        int m_expectedGlyphCount;

        glyph_t *m_glyphPool;
        QFixedPoint *m_positionPool;
    };

    class DrawTextItemDevice: public QPaintDevice
    {
    public:
        DrawTextItemDevice(int expectedItemCount = -1,  QStaticTextItem *items = 0,
                           int expectedGlyphCount = -1, QFixedPoint *positionPool = 0,
                           glyph_t *glyphPool = 0)
        {
            m_paintEngine = new DrawTextItemRecorder(expectedItemCount, items,
                                                     expectedGlyphCount, positionPool, glyphPool);
        }

        ~DrawTextItemDevice()
        {
            delete m_paintEngine;
        }

        int metric(PaintDeviceMetric m) const
        {
            int val;
            switch (m) {
            case PdmWidth:
            case PdmHeight:
            case PdmWidthMM:
            case PdmHeightMM:
                val = 0;
                break;
            case PdmDpiX:
            case PdmPhysicalDpiX:
                val = qt_defaultDpiX();
                break;
            case PdmDpiY:
            case PdmPhysicalDpiY:
                val = qt_defaultDpiY();
                break;
            case PdmNumColors:
                val = 16777216;
                break;
            case PdmDepth:
                val = 24;
                break;
            default:
                val = 0;
                qWarning("DrawTextItemDevice::metric: Invalid metric command");
            }
            return val;
        }

        virtual QPaintEngine *paintEngine() const
        {
            return m_paintEngine;
        }

        int itemCount() const
        {
            return m_paintEngine->itemCount();
        }

        int glyphCount() const
        {
            return m_paintEngine->glyphCount();
        }

    private:
        DrawTextItemRecorder *m_paintEngine;
    };
}

void QStaticTextPrivate::paintText(const QPointF &pos, QPainter *p)
{
    bool preferRichText = textFormat == Qt::RichText
                          || (textFormat == Qt::AutoText && Qt::mightBeRichText(text));

    if (!preferRichText) {
        if (maximumSize.isValid()) {
            QRectF boundingRect;
            p->drawText(QRectF(pos, maximumSize), Qt::TextWordWrap, text, &boundingRect);

            actualSize = boundingRect.size();
            needsClipRect = boundingRect.width() > maximumSize.width()
                            || boundingRect.height() > maximumSize.height();
        } else {
            p->drawText(pos, text);
            needsClipRect = false;

            QFontMetrics fm(font);
            actualSize = fm.boundingRect(text).size();
        }
    } else {
        QTextDocument document;
        document.setDefaultFont(font);
        document.setHtml(text);

        QRectF rect = maximumSize.isValid() ? QRectF(pos, maximumSize) : QRectF();
        document.adjustSize();
        p->save();
        p->translate(pos);
        document.drawContents(p, rect);        
        p->restore();
        actualSize = document.size();
        needsClipRect = maximumSize.isValid()
                        && (actualSize.width() > maximumSize.width()
                            || actualSize.height() > maximumSize.height());
    }
}

void QStaticTextPrivate::init()
{
    delete[] items;
    delete[] glyphPool;
    delete[] positionPool;

    position = QPointF(0, 0);

    // Draw once to count number of items and glyphs, so that we can use as little memory
    // as possible to store the data
    DrawTextItemDevice counterDevice;
    {
        QPainter painter(&counterDevice);
        painter.setFont(font);
        painter.setTransform(matrix);

        paintText(QPointF(0, 0), &painter);

    }

    itemCount = counterDevice.itemCount();    
    items = new QStaticTextItem[itemCount];

    if (useBackendOptimizations) {
        for (int i=0; i<itemCount; ++i)
            items[i].useBackendOptimizations = true;
    }


    int glyphCount = counterDevice.glyphCount();
    glyphPool = new glyph_t[glyphCount];
    positionPool = new QFixedPoint[glyphCount];

    // Draw again to actually record the items and glyphs
    DrawTextItemDevice recorderDevice(itemCount, items, glyphCount, positionPool, glyphPool);
    {
        QPainter painter(&recorderDevice);
        painter.setFont(font);
        painter.setTransform(matrix);

        paintText(QPointF(0, 0), &painter);
    }

}

QT_END_NAMESPACE
