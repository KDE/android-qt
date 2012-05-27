/******************************************************************************
* Copyright 2012  BogDan Vatra <bog_dan_ro@yahoo.com>                         *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) version 3, or any           *
* later version accepted by the membership of KDE e.V. (or its                *
* successor approved by the membership of KDE e.V.), which shall              *
* act as a proxy defined in Section 6 of version 3 of the license.            *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public            *
* License along with this library. If not, see <http://www.gnu.org/licenses/>.*
******************************************************************************/

#ifndef QANDROIDSTYLE_H
#define QANDROIDSTYLE_H

#include <QList>
#include <QMargins>
#include <QCommonStyle>
#include <QHash>
#include <QVariantMap>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#if !defined(QT_NO_STYLE_ANDROID)

class Q_GUI_EXPORT QAndroidStyle : public QCommonStyle
{
    Q_OBJECT

public:
    enum ItemType
    {
        QC_UnknownType = -1,
        QC_View,
        QC_GroupBox,
        QC_Button,
        QC_Checkbox,
        QC_RadioButton,
        QC_Slider,
        QC_Switch,
        QC_EditText,
        QC_Combobox,
        QC_BusyIndicator,
        QC_ProgressBar,
        QC_Tab,
        QC_TabButton,
        QC_RatingIndicator,
        QC_SearchBox,
        QC_CustomCOntrol=0xf00,
        QC_ControlMask=0xfff
    };

    struct Android9PatchChunk
    {
        QVector<int> xDivs;
        QVector<int> yDivs;
        QVector<int> colors;
    };

    struct AndroidItemStateInfo
    {
        AndroidItemStateInfo():state(0){}
        int state;
        QByteArray filePath;
        QByteArray hashKey;
        Android9PatchChunk chunkData;
        QSize size;
        QMargins padding;
    };

    enum AndroidDrawableType
    {
        Color,
        Image,
        Clip,
        NinePatch,
        Gradient,
        State,
        Layer
    };

    class AndroidDrawable
    {
    public:
        AndroidDrawable(const QVariantMap &drawable, ItemType itemType);
        virtual ~AndroidDrawable();
        virtual void initPadding(const QVariantMap &drawable);
        virtual AndroidDrawableType type() const = 0;
        virtual void draw(QPainter * painter,const QStyleOption *opt) const = 0;
        const QMargins & padding() const;
        virtual QSize size() const;
        static AndroidDrawable * fromMap(const QVariantMap &drawable, ItemType itemType);
        static QMargins extractMargins(const QVariantMap &value);
    protected:
        ItemType m_itemType;
        QMargins m_padding;
    };

    class AndroidColorDrawable: public AndroidDrawable
    {
    public:
        AndroidColorDrawable(const QVariantMap &drawable, ItemType itemType);
        virtual AndroidDrawableType type() const;
        virtual void draw(QPainter * painter,const QStyleOption *opt) const;

    protected:
        QColor m_color;
    };

    class AndroidImageDrawable: public AndroidDrawable
    {
    public:
        AndroidImageDrawable(const QVariantMap &drawable, ItemType itemType);
        virtual AndroidDrawableType type() const;
        virtual void draw(QPainter * painter,const QStyleOption *opt) const;
        virtual QSize size() const;

    protected:
        QString m_filePath;
        mutable QString m_hashKey;
        QSize m_size;
    };

    class Android9PatchDrawable: public AndroidImageDrawable
    {
    public:
        Android9PatchDrawable(const QVariantMap &drawable, ItemType itemType);
        virtual AndroidDrawableType type() const;
        virtual void draw(QPainter * painter, const QStyleOption *opt) const;
    private:
        static int calculateStretch(int boundsLimit, int startingPoint,
                                  int srcSpace, int numStrechyPixelsRemaining,
                                  int numFixedPixelsRemaining);
        void extractIntArray(const QVariantList &values, QVector<int> & array);
    private:
        Android9PatchChunk m_chunkData;
    };

    class AndroidGradientDrawable: public AndroidDrawable
    {
    public:
        enum GradientOrientation
        {
            TOP_BOTTOM,
            TR_BL,
            RIGHT_LEFT,
            BR_TL,
            BOTTOM_TOP,
            BL_TR,
            LEFT_RIGHT,
            TL_BR
        };

    public:
        AndroidGradientDrawable(const QVariantMap &drawable, ItemType itemType);
        virtual AndroidDrawableType type() const;
        virtual void draw(QPainter * painter, const QStyleOption *opt) const;
        QSize size();
    private:
        mutable QLinearGradient m_gradient;
        GradientOrientation m_orientation;
        int m_radius;
    };

    class AndroidClipDrawable: public AndroidDrawable
    {
    public:
        AndroidClipDrawable(const QVariantMap &drawable, ItemType itemType);
        ~AndroidClipDrawable();
        virtual AndroidDrawableType type() const;
        virtual void setFactor(double factor, Qt::Orientation orientation);
        virtual void draw(QPainter * painter, const QStyleOption *opt) const;

    private:
        double m_factor;
        Qt::Orientation m_orientation;
        const AndroidDrawable * m_drawable;
    };

    class AndroidStateDrawable: public AndroidDrawable
    {
    public:
        AndroidStateDrawable(const QVariantMap &drawable, ItemType itemType);
        ~AndroidStateDrawable();
        virtual AndroidDrawableType type() const;
        virtual void draw(QPainter * painter, const QStyleOption *opt) const;
        inline const AndroidDrawable* bestAndroidStateMatch(const QStyleOption *opt) const;
        static int extractState(const QVariantMap & value);

    private:
        typedef QPair<int, const AndroidDrawable*> StateType;
        QList< StateType > m_states;
    };

    class AndroidLayerDrawable: public AndroidDrawable
    {
    public:
        AndroidLayerDrawable(const QVariantMap &drawable, QAndroidStyle::ItemType itemType);
        ~AndroidLayerDrawable();
        virtual AndroidDrawableType type() const;
        virtual void draw(QPainter * painter, const QStyleOption *opt) const;
        AndroidDrawable * layer(int id) const;
        QSize size();
    private:
        typedef QPair<int, AndroidDrawable*> LayerType;
        QList<LayerType> m_layers;
    };

    class AndroidControl
    {
    public:
        AndroidControl(const QVariantMap &control, ItemType itemType);
        virtual ~AndroidControl();
        virtual void drawControl(const QStyleOption *opt, QPainter *p, const QWidget *w);
        virtual QRect subElementRect(SubElement subElement, const QStyleOption *option,
                                     const QWidget *widget = 0) const;
        virtual QRect subControlRect(const QStyleOptionComplex *option, SubControl sc,
                                     const QWidget *widget = 0) const;
        virtual QSize sizeFromContents(const QStyleOption *opt,
                                       const QSize &contentsSize, const QWidget *w) const;
        virtual QMargins padding();
    protected:
        const AndroidDrawable * m_background;
        QSize m_minSize;
        QSize m_maxSize;
    };

    class AndroidCompoundButtonControl : public AndroidControl
    {
    public:
        AndroidCompoundButtonControl(const QVariantMap &control, ItemType itemType);
        virtual ~AndroidCompoundButtonControl();
        virtual void drawControl(const QStyleOption *opt, QPainter *p, const QWidget *w);

    protected:
        const AndroidDrawable * m_button;
    };

    class AndroidProgressBarControl: public AndroidControl
    {
    public:
        AndroidProgressBarControl(const QVariantMap &control, ItemType itemType);
        virtual ~AndroidProgressBarControl();
        virtual void drawControl(const QStyleOption *option, QPainter *p, const QWidget *w);
        virtual QRect subElementRect(SubElement subElement, const QStyleOption *option,
                                     const QWidget *widget = 0) const;

        QSize sizeFromContents(const QStyleOption *opt,
                                       const QSize &contentsSize, const QWidget *w) const;
    protected:
        AndroidDrawable * m_progressDrawable;
        AndroidDrawable * m_indeterminateDrawable;
        int m_secondaryProgress_id;
        int m_progressId;
    };

    class AndroidSeekBarControl: public AndroidProgressBarControl
    {
    public:
        AndroidSeekBarControl(const QVariantMap &control, ItemType itemType);
        virtual ~AndroidSeekBarControl();
        virtual void drawControl(const QStyleOption *option, QPainter *p, const QWidget *w);
        QSize sizeFromContents(const QStyleOption *opt,
                                       const QSize &contentsSize, const QWidget *w) const;
        QRect subControlRect(const QStyleOptionComplex *option, SubControl sc,
                                     const QWidget *widget = 0) const;
    private:
        AndroidDrawable * m_seekBarThumb;
    };

    typedef QList<AndroidItemStateInfo *> AndroidItemStateInfoList;

public:
    QAndroidStyle();
    ~QAndroidStyle();

    virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                               const QWidget *w = 0) const;

    virtual void drawControl(QStyle::ControlElement element, const QStyleOption *opt, QPainter *p,
                             const QWidget *w = 0) const;

    virtual QRect subElementRect(SubElement subElement, const QStyleOption *option,
                                 const QWidget *widget = 0) const;
    virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                                    const QWidget *widget = 0) const;
    virtual SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                                             const QPoint &pt, const QWidget *widget = 0) const;
    virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                                 SubControl sc, const QWidget *widget = 0) const;

    virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
                            const QWidget *widget = 0) const;

    virtual QSize sizeFromContents(ContentsType ct, const QStyleOption *opt,
                                   const QSize &contentsSize, const QWidget *w = 0) const;

    virtual int styleHint(StyleHint stylehint, const QStyleOption *opt = 0,
                          const QWidget *widget = 0, QStyleHintReturn* returnData = 0) const;

    virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt = 0,
                                   const QWidget *widget = 0) const;

    virtual QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                        const QStyleOption *opt) const;
signals:

public slots:

private:
    Q_DISABLE_COPY(QAndroidStyle)
    static ItemType qtControl(QStyle::ComplexControl control);
    static ItemType qtControl(QStyle::ContentsType contentsType);
    static ItemType qtControl(QStyle::ControlElement controlElement);
    static ItemType qtControl(QStyle::PrimitiveElement primitiveElement);
    static ItemType qtControl(QStyle::SubElement subElement);
    static ItemType qtControl(const QString & android);

    static void setPaletteColor(const QVariantMap & object, QPalette & palette
                         , QPalette::ColorRole role);
private:
    typedef QHash<int, AndroidControl * > AndroidControlsHash;
    AndroidControlsHash m_androidControlsHash;
};

#endif // QT_NO_STYLE_ANDROID

QT_END_NAMESPACE

QT_END_HEADER

#endif // QANDROIDSTYLE_H
