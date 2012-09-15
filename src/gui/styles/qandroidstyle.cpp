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

#include "qandroidstyle.h"

#if !defined(QT_NO_STYLE_ANDROID) || defined(QT_PLUGIN)

#include <QFile>
#include <QFont>
#include <QApplication>
#include <QTime>
#include <qdrawutil.h>
#include <QPixmapCache>
#include <QFileInfo>
#include <QStyleOption>
#include <QPainter>

#include <QDebug>
#include "json.h"

QT_BEGIN_NAMESPACE

namespace {
    const int textStyle_bold = 1;
    const int textStyle_italic = 2;

    const int typeface_sans = 1;
    const int typeface_serif = 2;
    const int typeface_monospace = 3;

    const quint32 NO_COLOR = 1;
    const quint32 TRANSPARENT_COLOR = 0;
}


QAndroidStyle::QAndroidStyle() :
    QCommonStyle()
{
    QTime t;
    t.start();
    QString stylePath(QLatin1String(qgetenv("MINISTRO_ANDROID_STYLE_PATH")));
    if (stylePath.isEmpty())
        stylePath=QLatin1String("/data/data/org.kde.necessitas.ministro/files/qt/style/");
    Q_ASSERT(!stylePath.isEmpty());
    QFile f(stylePath+QLatin1String("style.json"));
    if (!f.open(QIODevice::ReadOnly))
        return;
    JsonReader reader;
    if (!reader.parse(f.readAll()))
    {
        qCritical()<<reader.errorString();
        return;
    }
    qDebug()<<"Reading json file took"<<t.elapsed()<<"ms";
    t.restart();
    QVariantMap jsDocument = reader.result().toMap();
    foreach(QString key, jsDocument.keys())
    {
        QVariantMap item=jsDocument.value(key).toMap();
        if(item.contains(QLatin1String("qtClass")))
        { // the item has palette and font informations for a specifi Qt Class (e.g. QWidget, QPushButton, etc.)
            const QString qtClassName=item.value(QLatin1String("qtClass")).toString();

            // extract font information
            QFont font;

            // font size (in pixels)
            QVariantMap::const_iterator it=item.find(QLatin1String("TextAppearance_textSize"));
            if (it!=item.end())
                font.setPixelSize(it.value().toInt());

            // font style
            it=item.find(QLatin1String("TextAppearance_textStyle"));
            if (it!=item.end())
            {
                const int style=it.value().toInt();
                font.setBold(style & textStyle_bold);
                font.setItalic(style & textStyle_italic);
            }

            // font typefac
            it=item.find(QLatin1String(QLatin1String("TextAppearance_typeface")));
            if (it!=item.end())
            {
                QFont::StyleHint styleHint=QFont::AnyStyle;
                switch(it.value().toInt())
                {
                    case typeface_sans:
                        styleHint=QFont::SansSerif;
                        break;
                    case typeface_serif:
                        styleHint=QFont::Serif;
                        break;
                    case typeface_monospace:
                        styleHint=QFont::Monospace;
                        break;
                }
                font.setStyleHint(styleHint, QFont::PreferMatch);
            }
            QApplication::setFont(font, qtClassName.toUtf8());
            // extract font information

            // extract palette information
            QPalette palette;
            it=item.find(QLatin1String("TextAppearance_textColor"));
            if (it!=item.end())
                setPaletteColor(it.value().toMap(), palette, QPalette::WindowText);

            it=item.find(QLatin1String("TextAppearance_textColorLink"));
            if (it!=item.end())
                setPaletteColor(it.value().toMap(), palette, QPalette::Link);

            it=item.find(QLatin1String("TextAppearance_textColorHighlight"));
            if (it!=item.end())
                palette.setColor(QPalette::Highlight, (QRgb)it.value().toInt());
            palette.setColor(QPalette::Window, Qt::black);
            QApplication::setPalette(palette, qtClassName.toUtf8());
            // extract palette information
        }
        QApplication::setPalette(QApplication::palette("simple_list_item"), "QListView");
        QApplication::setFont(QApplication::font("simple_list_item"), "QListView");
        QApplication::setPalette(QApplication::palette("simple_list_item"), "QAbstractItemView");
        QApplication::setFont(QApplication::font("simple_list_item"), "QAbstractItemView");
        QAndroidStyle::ItemType itemType=qtControl(key);
        if (QC_UnknownType==itemType)
            continue;

        switch(itemType)
        {
        case QC_Checkbox:
        case QC_RadioButton:
            m_androidControlsHash[(int)itemType] = new AndroidCompoundButtonControl(item, itemType);
            break;

        case QC_ProgressBar:
            m_androidControlsHash[(int)itemType] = new AndroidProgressBarControl(item, itemType);
            break;

        case QC_Slider:
            m_androidControlsHash[(int)itemType] = new AndroidSeekBarControl(item, itemType);
            break;

        case QC_Combobox:
            m_androidControlsHash[(int)itemType] = new AndroidSpinnerControl(item, itemType);
            break;

        default:
            m_androidControlsHash[(int)itemType] = new AndroidControl(item, itemType);
            break;
        }
    }
    qDebug()<<"Parsing json file took"<<t.elapsed()<<"ms";
}

QAndroidStyle::~QAndroidStyle()
{
    foreach(AndroidControl * control, m_androidControlsHash)
        delete control;
}


void QAndroidStyle::setPaletteColor(const QVariantMap &object, QPalette & palette
                                   , QPalette::ColorRole role)
{
    // QPalette::Active -> ENABLED_FOCUSED_WINDOW_FOCUSED_STATE_SET
    palette.setColor(QPalette::Active, role
                     , (QRgb)object.value(QLatin1String("ENABLED_FOCUSED_WINDOW_FOCUSED_STATE_SET")).toInt());

    // QPalette::Inactive -> ENABLED_STATE_SET
    palette.setColor(QPalette::Inactive, role
                     , (QRgb)object.value(QLatin1String("ENABLED_STATE_SET")).toInt());

    // QPalette::Disabled -> EMPTY_STATE_SET
    palette.setColor(QPalette::Disabled, role
                     , (QRgb)object.value(QLatin1String("EMPTY_STATE_SET")).toInt());

    palette.setColor(QPalette::Current, role, palette.color(QPalette::Active, role));

    if (role == QPalette::WindowText)
    {
        // QPalette::BrightText -> PRESSED
        // QPalette::Active -> PRESSED_ENABLED_FOCUSED_WINDOW_FOCUSED_STATE_SET
        palette.setColor(QPalette::Active, QPalette::BrightText
                         , (QRgb)object.value(QLatin1String("PRESSED_ENABLED_FOCUSED_WINDOW_FOCUSED_STATE_SET")).toInt());

        // QPalette::Inactive -> PRESSED_ENABLED_STATE_SET
        palette.setColor(QPalette::Inactive, QPalette::BrightText
                         , (QRgb)object.value(QLatin1String("PRESSED_ENABLED_STATE_SET")).toInt());

        // QPalette::Disabled -> PRESSED_STATE_SET
        palette.setColor(QPalette::Disabled, QPalette::BrightText
                         , (QRgb)object.value(QLatin1String("PRESSED_STATE_SET")).toInt());

        palette.setColor(QPalette::Current, QPalette::BrightText, palette.color(QPalette::Active, QPalette::BrightText));

        // QPalette::HighlightedText -> SELECTED
        // QPalette::Active -> ENABLED_SELECTED_WINDOW_FOCUSED_STATE_SET
        palette.setColor(QPalette::Active, QPalette::HighlightedText
                         , (QRgb)object.value(QLatin1String("ENABLED_SELECTED_WINDOW_FOCUSED_STATE_SET")).toInt());

        // QPalette::Inactive -> ENABLED_SELECTED_STATE_SET
        palette.setColor(QPalette::Inactive, QPalette::HighlightedText
                         , (QRgb)object.value(QLatin1String("ENABLED_SELECTED_STATE_SET")).toInt());

        // QPalette::Disabled -> SELECTED_STATE_SET
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText
                         , (QRgb)object.value(QLatin1String("SELECTED_STATE_SET")).toInt());

        palette.setColor(QPalette::Current, QPalette::HighlightedText, palette.color(QPalette::Active, QPalette::HighlightedText));

        // Same colors for Text
        palette.setColor(QPalette::Active, QPalette::Text, palette.color(QPalette::Active, role));
        palette.setColor(QPalette::Inactive, QPalette::Text, palette.color(QPalette::Inactive, role));
        palette.setColor(QPalette::Disabled, QPalette::Text, palette.color(QPalette::Disabled, role));
        palette.setColor(QPalette::Current, QPalette::Text, palette.color(QPalette::Current, role));

        // And for ButtonText
        palette.setColor(QPalette::Active, QPalette::ButtonText, palette.color(QPalette::Active, role));
        palette.setColor(QPalette::Inactive, QPalette::ButtonText, palette.color(QPalette::Inactive, role));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, palette.color(QPalette::Disabled, role));
        palette.setColor(QPalette::Current, QPalette::ButtonText, palette.color(QPalette::Current, role));
    }
}

QAndroidStyle::ItemType QAndroidStyle::qtControl(const QString & android)
{
    if (android == QLatin1String("buttonStyle"))
        return QC_Button;
    if (android == QLatin1String("editTextStyle"))
        return QC_EditText;
    if (android == QLatin1String("radioButtonStyle"))
        return QC_RadioButton;
    if (android == QLatin1String("checkboxStyle"))
        return QC_Checkbox;
    if (android == QLatin1String("textViewStyle"))
        return QC_View;
    if (android == QLatin1String("buttonStyleToggle"))
        return QC_Switch;
    if (android == QLatin1String("spinnerStyle"))
        return QC_Combobox;
    if (android == QLatin1String("progressBarStyleHorizontal"))
        return QC_ProgressBar;
    if (android == QLatin1String("seekBarStyle"))
        return QC_Slider;

    return QC_UnknownType;
}

QAndroidStyle::ItemType QAndroidStyle::qtControl(QStyle::ComplexControl control)
{
    switch(control)
    {
        case CC_ComboBox:
            return QC_Combobox;
        case CC_Slider:
            return QC_Slider;
        case CC_GroupBox:
            return QC_View;
        default:
            return QC_UnknownType;
    }
}

QAndroidStyle::ItemType QAndroidStyle::qtControl(QStyle::ContentsType contentsType)
{
    switch(contentsType)
    {
        case CT_PushButton:
            return QC_Button;
        case CT_CheckBox:
            return QC_Checkbox;
        case CT_RadioButton:
            return QC_RadioButton;
        case CT_ComboBox:
            return QC_Combobox;
        case CT_ProgressBar:
            return QC_ProgressBar;
        case CT_Slider:
            return QC_Slider;
        case CT_TabWidget:
            return QC_Tab;
        case CT_TabBarTab:
            return QC_TabButton;
        case CT_LineEdit:
            return QC_EditText;
        case CT_GroupBox:
            return QC_GroupBox;
        default:
            return QC_UnknownType;
    }
}

QAndroidStyle::ItemType QAndroidStyle::qtControl(QStyle::ControlElement controlElement)
{
    switch(controlElement)
    {
        case CE_PushButton:
        case CE_PushButtonBevel:
        case CE_PushButtonLabel:
            return QC_Button;

        case CE_CheckBox:
        case CE_CheckBoxLabel:
            return QC_Checkbox;

        case CE_RadioButton:
        case CE_RadioButtonLabel:
            return QC_RadioButton;

        case CE_TabBarTab:
        case CE_TabBarTabShape:
        case CE_TabBarTabLabel:
            return QC_Tab;

        case CE_ProgressBar:
        case CE_ProgressBarGroove:
        case CE_ProgressBarContents:
        case CE_ProgressBarLabel:
            return QC_ProgressBar;

        case CE_ComboBoxLabel:
            return QC_Combobox;

        default:
            return QC_UnknownType;
    }
}

QAndroidStyle::ItemType QAndroidStyle::qtControl(QStyle::PrimitiveElement primitiveElement)
{
    switch(primitiveElement)
    {
        case QStyle::PE_PanelLineEdit:
        case QStyle::PE_FrameLineEdit:
            return QC_EditText;

        case QStyle::PE_FrameWindow:
        case QStyle::PE_Widget:
        case QStyle::PE_Frame:
        case QStyle::PE_FrameFocusRect:
            return QC_View;
        default:
            return QC_UnknownType;
    }
}

QAndroidStyle::ItemType QAndroidStyle::qtControl(QStyle::SubElement subElement)
{
    switch(subElement)
    {
        case QStyle::SE_LineEditContents:
            return QC_EditText;

        case QStyle::SE_PushButtonContents:
        case QStyle::SE_PushButtonFocusRect:
            return QC_Button;

        case SE_RadioButtonContents:
            return QC_RadioButton;

        case SE_CheckBoxContents:
            return QC_Checkbox;

        default:
            return QC_UnknownType;
    }
}

void QAndroidStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                           const QWidget *w) const
{
    const ItemType itemType=qtControl(pe);
    AndroidControlsHash::const_iterator it;
    if ( itemType != QC_UnknownType
            && (it=m_androidControlsHash.find(itemType))!=m_androidControlsHash.end() )
        it.value()->drawControl(opt, p, w);
    else
        QCommonStyle::drawPrimitive(pe, opt, p, w);
}


void QAndroidStyle::drawControl(QStyle::ControlElement element, const QStyleOption *opt, QPainter *p,
                         const QWidget *w) const
{
    const ItemType itemType=qtControl(element);
    AndroidControlsHash::const_iterator it;
    if ( itemType != QC_UnknownType
            && (it=m_androidControlsHash.find(itemType))!=m_androidControlsHash.end() )
    {
        it.value()->drawControl(opt, p, w);

        switch(itemType)
        {
            case QC_Button:
                if (const QStyleOptionButton *buttonOption =
                        qstyleoption_cast<const QStyleOptionButton *>(opt))
                {
                    QMargins padding=it.value()->padding();
                    QStyleOptionButton copy (*buttonOption);
                    copy.rect.adjust(padding.left(), padding.top(), -padding.right(), -padding.bottom());
                    QCommonStyle::drawControl(CE_PushButtonLabel, &copy, p, w);
                }
                break;
            case QC_Checkbox:
            case QC_RadioButton:
                if (const QStyleOptionButton *btn =
                        qstyleoption_cast<const QStyleOptionButton *>(opt))
                {
                    const bool isRadio = (element == CE_RadioButton);
                    QStyleOptionButton subopt(*btn);
                    subopt.rect = subElementRect(isRadio ? SE_RadioButtonContents
                                                         : SE_CheckBoxContents, btn, w);
                    QCommonStyle::drawControl(isRadio ? CE_RadioButtonLabel : CE_CheckBoxLabel, &subopt, p, w);
                }
                break;
        case QC_Combobox:
            if (const QStyleOptionComboBox *comboboxOption =
                    qstyleoption_cast<const QStyleOptionComboBox *>(opt))
            {
                QMargins padding=it.value()->padding();
                QStyleOptionComboBox copy (*comboboxOption);
                copy.rect.adjust(padding.left(), padding.top(), -padding.right(), -padding.bottom());
                p->setFont(QApplication::font("simple_spinner_item"));
                p->setPen(QApplication::palette("QPushButton").color(QPalette::Active, QPalette::Text));
                QCommonStyle::drawControl(CE_ComboBoxLabel, comboboxOption, p, w);
            }
            break;
        default:
            break;
        }
    }
    else
        QCommonStyle::drawControl(element, opt, p, w);
}

QRect QAndroidStyle::subElementRect(SubElement subElement, const QStyleOption *option,
                             const QWidget *widget) const
{
    const ItemType itemType=qtControl(subElement);
    AndroidControlsHash::const_iterator it;
    if ( itemType != QC_UnknownType
            && (it=m_androidControlsHash.find(itemType))!=m_androidControlsHash.end() )
        return it.value()->subElementRect(subElement, option, widget);
    return QCommonStyle::subElementRect(subElement, option, widget);
}

void QAndroidStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                                const QWidget *widget) const
{
    const ItemType itemType=qtControl(cc);
    AndroidControlsHash::const_iterator it;
    if ( itemType != QC_UnknownType
            && (it=m_androidControlsHash.find(itemType))!=m_androidControlsHash.end() )
        it.value()->drawControl(opt, p, widget);
    else
        QCommonStyle::drawComplexControl(cc, opt, p, widget);
}

QStyle::SubControl QAndroidStyle::hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                                         const QPoint &pt, const QWidget *widget) const
{
    const ItemType itemType=qtControl(cc);
    AndroidControlsHash::const_iterator it;
    if ( itemType != QC_UnknownType
            && (it=m_androidControlsHash.find(itemType))!=m_androidControlsHash.end() )
    {
        switch(cc)
        {
            case CC_Slider:
            if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
                QRect r = it.value()->subControlRect(slider, SC_SliderHandle, widget);
                if (r.isValid() && r.contains(pt)) {
                    return SC_SliderHandle;
                } else {
                    r = it.value()->subControlRect(slider, SC_SliderGroove ,widget);
                    if (r.isValid() && r.contains(pt))
                        return SC_SliderGroove;
                }
            }
            break;
        default:
            break;
        }
    }
    return QCommonStyle::hitTestComplexControl(cc, opt, pt, widget);
}

QRect QAndroidStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                             SubControl sc, const QWidget *widget) const
{
    const ItemType itemType=qtControl(cc);
    AndroidControlsHash::const_iterator it;
    if ( itemType != QC_UnknownType
            && (it=m_androidControlsHash.find(itemType))!=m_androidControlsHash.end() )
        return it.value()->subControlRect(opt, sc, widget);
    return QCommonStyle::subControlRect(cc, opt, sc, widget);
}

int QAndroidStyle::pixelMetric(PixelMetric metric, const QStyleOption *option,
                        const QWidget *widget) const
{
    switch(metric)
    {
    case PM_ButtonMargin:
    case PM_FocusFrameVMargin:
    case PM_FocusFrameHMargin:
    case PM_ComboBoxFrameWidth:
    case PM_SpinBoxFrameWidth:
        return 0;
    default:
        return QCommonStyle::pixelMetric(metric, option, widget);
    }

}

QSize QAndroidStyle::sizeFromContents(ContentsType ct, const QStyleOption *opt,
                               const QSize &contentsSize, const QWidget *w) const
{
    QSize sz=QCommonStyle::sizeFromContents(ct, opt, contentsSize, w);
    const ItemType itemType=qtControl(ct);
    AndroidControlsHash::const_iterator it;
    if ( itemType != QC_UnknownType
            && (it=m_androidControlsHash.find(itemType))!=m_androidControlsHash.end() )
        return it.value()->sizeFromContents(opt, sz, w);
    return sz;
}

QPixmap QAndroidStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                               const QWidget *widget) const
{
    return QCommonStyle::standardPixmap(standardPixmap, opt, widget);
//    return QPixmap(); // should we support any standard pixmaps ?
}

QPixmap QAndroidStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                    const QStyleOption *opt) const
{
    return QCommonStyle::generatedIconPixmap(iconMode, pixmap, opt);
}



QAndroidStyle::AndroidDrawable::AndroidDrawable(const QVariantMap &drawable, QAndroidStyle::ItemType itemType)
{
    initPadding(drawable);
    m_itemType = itemType;
}

QAndroidStyle::AndroidDrawable::~AndroidDrawable()
{
}

void QAndroidStyle::AndroidDrawable::initPadding(const QVariantMap &drawable)
{
    QVariantMap::const_iterator it = drawable.find(QLatin1String("padding"));
    if (it!=drawable.end())
        m_padding = extractMargins(it.value().toMap());
}

const QMargins & QAndroidStyle::AndroidDrawable::padding() const
{
    return m_padding;
}

QSize QAndroidStyle::AndroidDrawable::size() const
{
    if (type() == Image || type() == NinePatch)
        return static_cast<const QAndroidStyle::AndroidImageDrawable*>(this)->size();
    return QSize();
}

QAndroidStyle::AndroidDrawable * QAndroidStyle::AndroidDrawable::fromMap(const QVariantMap &drawable, ItemType itemType)
{
    const QString type=drawable.value(QLatin1String("type")).toString();
    if (type == QLatin1String("image"))
    {
        return new QAndroidStyle::AndroidImageDrawable(drawable, itemType);
    }
    if (type == QLatin1String("9patch"))
    {
        return new QAndroidStyle::Android9PatchDrawable(drawable, itemType);
    }
    if (type == QLatin1String("stateslist"))
    {
        return new QAndroidStyle::AndroidStateDrawable(drawable, itemType);
    }
    if (type == QLatin1String("layer"))
    {
        return new QAndroidStyle::AndroidLayerDrawable(drawable, itemType);
    }
    if (type == QLatin1String("gradient"))
    {
        return new QAndroidStyle::AndroidGradientDrawable(drawable, itemType);
    }
    if (type == QLatin1String("clipDrawable"))
    {
        return new QAndroidStyle::AndroidClipDrawable(drawable, itemType);
    }
    if (type == QLatin1String("color"))
    {
        return new QAndroidStyle::AndroidColorDrawable(drawable, itemType);
    }
    return 0;
}

QMargins QAndroidStyle::AndroidDrawable::extractMargins(const QVariantMap &value)
{
    QMargins m;
    m.setLeft(value.value(QLatin1String("left")).toInt());
    m.setRight(value.value(QLatin1String("right")).toInt());
    m.setTop(value.value(QLatin1String("top")).toInt());
    m.setBottom(value.value(QLatin1String("bottom")).toInt());
    return m;
}


QAndroidStyle::AndroidImageDrawable::AndroidImageDrawable(const QVariantMap &drawable, QAndroidStyle::ItemType itemType)
    :AndroidDrawable(drawable, itemType)
{
    m_filePath=drawable.value(QLatin1String("path")).toString();
    m_size.setHeight(drawable.value(QLatin1String("height")).toInt());
    m_size.setWidth(drawable.value(QLatin1String("width")).toInt());
}

QAndroidStyle::AndroidDrawableType QAndroidStyle::AndroidImageDrawable::type() const
{
    return QAndroidStyle::Image;
}

void QAndroidStyle::AndroidImageDrawable::draw(QPainter * painter, const QStyleOption *opt) const
{
    if (m_hashKey.isEmpty())
        m_hashKey=QFileInfo(m_filePath).fileName();

    QPixmap pm;
    if (!QPixmapCache::find(m_hashKey, &pm))
    {
        pm.load(m_filePath);
        QPixmapCache::insert(m_hashKey, pm);
    }

    painter->drawPixmap(opt->rect.x(),(opt->rect.height()-pm.height())/2, pm);
}

QSize QAndroidStyle::AndroidImageDrawable::size() const
{
    return m_size;
}

QAndroidStyle::AndroidColorDrawable::AndroidColorDrawable(const QVariantMap &drawable, ItemType itemType)
    :AndroidDrawable(drawable, itemType)
{
    m_color.setRgba((QRgb)drawable.value(QLatin1String("color")).toInt());
}

QAndroidStyle::AndroidDrawableType QAndroidStyle::AndroidColorDrawable::type() const
{
    return QAndroidStyle::Color;
}

void QAndroidStyle::AndroidColorDrawable::draw(QPainter * painter,const QStyleOption *opt) const
{
    painter->fillRect(opt->rect, m_color);
}

QAndroidStyle::Android9PatchDrawable::Android9PatchDrawable(const QVariantMap &drawable, QAndroidStyle::ItemType itemType)
    :AndroidImageDrawable(drawable.value(QLatin1String("drawable")).toMap(), itemType)
{
    initPadding(drawable);
    QVariantMap chunk=drawable.value(QLatin1String("chunkInfo")).toMap();
    extractIntArray(chunk.value(QLatin1String("xdivs")).toList(), m_chunkData.xDivs);
    extractIntArray(chunk.value(QLatin1String("ydivs")).toList(), m_chunkData.yDivs);
    extractIntArray(chunk.value(QLatin1String("colors")).toList(), m_chunkData.colors);
}

QAndroidStyle::AndroidDrawableType QAndroidStyle::Android9PatchDrawable::type() const
{
    return QAndroidStyle::NinePatch;
}

int QAndroidStyle::Android9PatchDrawable::calculateStretch(int boundsLimit, int startingPoint,
                          int srcSpace, int numStrechyPixelsRemaining,
                          int numFixedPixelsRemaining)
{
    int spaceRemaining = boundsLimit - startingPoint;
    int stretchySpaceRemaining = spaceRemaining - numFixedPixelsRemaining;
    return ((float)srcSpace * stretchySpaceRemaining / numStrechyPixelsRemaining +.5);
}

void QAndroidStyle::Android9PatchDrawable::extractIntArray(const QVariantList &values, QVector<int> & array)
{
    foreach(QVariant value, values)
        array<<value.toInt();
}


void QAndroidStyle::Android9PatchDrawable::draw(QPainter * painter, const QStyleOption *opt) const
{
    if (m_hashKey.isEmpty())
        m_hashKey=QFileInfo(m_filePath).fileName();

    QPixmap pixmap;
    if (!QPixmapCache::find(m_hashKey, &pixmap))
    {
        pixmap.load(m_filePath);
        QPixmapCache::insert(m_hashKey, pixmap);
    }
    const QRect & bounds=opt->rect;

    // shamelessly stolen from Android's sources (NinepatchImpl.cpp) and adapted for Qt
    const int pixmapWidth = pixmap.width();
    const int pixmapHeight = pixmap.height();

    if (bounds.isNull() || !pixmapWidth || !pixmapHeight)
        return;

    QPainter::RenderHints savedHints=painter->renderHints();

    // The patchs doesn't need smooth transform !
    painter->setRenderHints(QPainter::SmoothPixmapTransform, false);


    QRectF dst;
    QRectF src;

    const int32_t x0 = m_chunkData.xDivs[0];
    const int32_t y0 = m_chunkData.yDivs[0];
    const quint8 numXDivs = m_chunkData.xDivs.size();
    const quint8 numYDivs = m_chunkData.yDivs.size();
    int i;
    int j;
    int colorIndex = 0;
    quint32 color;
    bool xIsStretchable;
    const bool initialXIsStretchable =  (x0 == 0);
    bool yIsStretchable = (y0 == 0);
    const int bitmapWidth = pixmap.width();
    const int bitmapHeight = pixmap.height();

    int * dstRights = (int*) alloca((numXDivs + 1) * sizeof(int));
    bool dstRightsHaveBeenCached = false;

    int numStretchyXPixelsRemaining = 0;
    for (i = 0; i < numXDivs; i += 2)
        numStretchyXPixelsRemaining += m_chunkData.xDivs[i + 1] - m_chunkData.xDivs[i];

    int numFixedXPixelsRemaining = bitmapWidth - numStretchyXPixelsRemaining;
    int numStretchyYPixelsRemaining = 0;
    for (i = 0; i < numYDivs; i += 2)
        numStretchyYPixelsRemaining += m_chunkData.yDivs[i + 1] - m_chunkData.yDivs[i];

    int numFixedYPixelsRemaining = bitmapHeight - numStretchyYPixelsRemaining;
    src.setTop(0);
    dst.setTop(bounds.top());
    // The first row always starts with the top being at y=0 and the bottom
    // being either yDivs[1] (if yDivs[0]=0) of yDivs[0].  In the former case
    // the first row is stretchable along the Y axis, otherwise it is fixed.
    // The last row always ends with the bottom being bitmap.height and the top
    // being either yDivs[numYDivs-2] (if yDivs[numYDivs-1]=bitmap.height) or
    // yDivs[numYDivs-1]. In the former case the last row is stretchable along
    // the Y axis, otherwise it is fixed.
    //
    // The first and last columns are similarly treated with respect to the X
    // axis.
    //
    // The above is to help explain some of the special casing that goes on the
    // code below.

    // The initial yDiv and whether the first row is considered stretchable or
    // not depends on whether yDiv[0] was zero or not.
    for (j = yIsStretchable ? 1 : 0;
         j <= numYDivs && src.top() < bitmapHeight;
          j++, yIsStretchable = !yIsStretchable) {
        src.setLeft(0);
        dst.setLeft(bounds.left());
        if (j == numYDivs) {
            src.setBottom(bitmapHeight);
            dst.setBottom(bounds.bottom());
        } else {
            src.setBottom(m_chunkData.yDivs[j]);
            const int srcYSize = src.bottom() - src.top();
            if (yIsStretchable) {
                dst.setBottom(dst.top() + calculateStretch(bounds.bottom(), dst.top(),
                                                          srcYSize,
                                                          numStretchyYPixelsRemaining,
                                                          numFixedYPixelsRemaining));
                numStretchyYPixelsRemaining -= srcYSize;
            } else {
                dst.setBottom(dst.top() + srcYSize);
                numFixedYPixelsRemaining -= srcYSize;
            }
        }

        xIsStretchable = initialXIsStretchable;
        // The initial xDiv and whether the first column is considered
        // stretchable or not depends on whether xDiv[0] was zero or not.
        for (i = xIsStretchable ? 1 : 0;
              i <= numXDivs && src.left() < bitmapWidth;
              i++, xIsStretchable = !xIsStretchable) {
            color = m_chunkData.colors[colorIndex++];
            if (i == numXDivs) {
                src.setRight(bitmapWidth);
                dst.setRight(bounds.right());
            } else {
                src.setRight(m_chunkData.xDivs[i]);
                if (dstRightsHaveBeenCached) {
                    dst.setRight(dstRights[i]);
                } else {
                    const int srcXSize = src.right() - src.left();
                    if (xIsStretchable) {
                        dst.setRight(dst.left() + calculateStretch(bounds.right(), dst.left(),
                                                                  srcXSize,
                                                                  numStretchyXPixelsRemaining,
                                                                  numFixedXPixelsRemaining));
                        numStretchyXPixelsRemaining -= srcXSize;
                    } else {
                        dst.setRight(dst.left() + srcXSize);
                        numFixedXPixelsRemaining -= srcXSize;
                    }
                    dstRights[i] = dst.right();
                }
            }
            // If this horizontal patch is too small to be displayed, leave
            // the destination left edge where it is and go on to the next patch
            // in the source.
            if (src.left() >= src.right()) {
                src.setLeft(src.right());
                continue;
            }
            // Make sure that we actually have room to draw any bits
            if (dst.right() <= dst.left() || dst.bottom() <= dst.top()) {
                goto nextDiv;
            }
            // If this patch is transparent, skip and don't draw.
            if (color == TRANSPARENT_COLOR)
                goto nextDiv;
            if (color != NO_COLOR)
                painter->fillRect(dst, (QRgb)color);
            else
                painter->drawPixmap(dst, pixmap, src);
nextDiv:
            src.setLeft(src.right());
            dst.setLeft(dst.right());
        }
        src.setTop(src.bottom());
        dst.setTop(dst.bottom());
        dstRightsHaveBeenCached = true;
    }
    painter->setRenderHints(savedHints);
}

QAndroidStyle::AndroidGradientDrawable::AndroidGradientDrawable(const QVariantMap &drawable, QAndroidStyle::ItemType itemType)
    :AndroidDrawable(drawable, itemType)
{
    m_radius = drawable.value(QLatin1String("radius")).toInt();
    if (m_radius<0)
        m_radius = 0;
    QVariantList colors=drawable.value(QLatin1String("colors")).toList();
    QVariantList positions=drawable.value(QLatin1String("positions")).toList();
    int min=colors.size()<positions.size()?colors.size():positions.size();
    for (int i=0;i<min;i++)
        m_gradient.setColorAt(positions.at(i).toDouble(), (QRgb)colors.at(i).toInt());
    QByteArray orientation=drawable.value(QLatin1String("orientation")).toByteArray();
    if (orientation == "TOP_BOTTOM")
    { // draw the gradient from the top to the bottom
        m_orientation = TOP_BOTTOM;
    }
    else if (orientation == "TR_BL")
    { // draw the gradient from the top-right to the bottom-left
        m_orientation = TR_BL;
    }
    else if (orientation == "RIGHT_LEFT")
    { // draw the gradient from the right to the left
        m_orientation = RIGHT_LEFT;
    }
    else if (orientation == "BR_TL")
    { // draw the gradient from the bottom-right to the top-left
        m_orientation = BR_TL;
    }
    else if (orientation == "BOTTOM_TOP")
    { // draw the gradient from the bottom to the top
        m_orientation = BOTTOM_TOP;
    }
    else if (orientation == "BL_TR")
    { // draw the gradient from the bottom-left to the top-right
        m_orientation = BL_TR;
    }
    else if (orientation == "LEFT_RIGHT")
    { // draw the gradient from the left to the right
        m_orientation = LEFT_RIGHT;
    }
    else if (orientation == "TL_BR")
    { // draw the gradient from the top-left to the bottom-right
        m_orientation = TL_BR;
    }
}

QAndroidStyle::AndroidDrawableType QAndroidStyle::AndroidGradientDrawable::type() const
{
    return QAndroidStyle::Gradient;
}

void QAndroidStyle::AndroidGradientDrawable::draw(QPainter * painter, const QStyleOption *opt) const
{
    const int width=opt->rect.width();
    const int height=opt->rect.height();
    switch (m_orientation)
    {
        case TOP_BOTTOM:
            // draw the gradient from the top to the bottom
            m_gradient.setStart(width/2,0);
            m_gradient.setFinalStop(width/2,height);
            break;
        case TR_BL:
            // draw the gradient from the top-right to the bottom-left
            m_gradient.setStart(width,0);
            m_gradient.setFinalStop(0,height);
        case RIGHT_LEFT:
            // draw the gradient from the right to the left
            m_gradient.setStart(width,height/2);
            m_gradient.setFinalStop(0,height/2);
        case BR_TL:
            // draw the gradient from the bottom-right to the top-left
            m_gradient.setStart(width,height);
            m_gradient.setFinalStop(0,0);
        case BOTTOM_TOP:
            // draw the gradient from the bottom to the top
            m_gradient.setStart(width/2,height);
            m_gradient.setFinalStop(width/2,0);
        case BL_TR:
            // draw the gradient from the bottom-left to the top-right
            m_gradient.setStart(0,height);
            m_gradient.setFinalStop(width,0);
        case LEFT_RIGHT:
            // draw the gradient from the left to the right
            m_gradient.setStart(0,height/2);
            m_gradient.setFinalStop(width,height/2);
        case TL_BR:
            // draw the gradient from the top-left to the bottom-right
            m_gradient.setStart(0,0);
            m_gradient.setFinalStop(width,height);
    }

    const QBrush & oldBrush = painter->brush();
    const QPen oldPen=painter->pen();
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_gradient);
    painter->drawRoundedRect(opt->rect, m_radius, m_radius);
    painter->setBrush(oldBrush);
    painter->setPen(oldPen);
}

QSize QAndroidStyle::AndroidGradientDrawable::size()
{
    return QSize(m_radius*2, m_radius*2);
}

QAndroidStyle::AndroidClipDrawable::AndroidClipDrawable(const QVariantMap &drawable, QAndroidStyle::ItemType itemType)
    :AndroidDrawable(drawable, itemType)
{
    m_drawable = fromMap(drawable.value(QLatin1String("drawable")).toMap(), itemType);
    m_factor = 0;
    m_orientation = Qt::Horizontal;
}

QAndroidStyle::AndroidClipDrawable::~AndroidClipDrawable()
{
    delete m_drawable;
}

QAndroidStyle::AndroidDrawableType QAndroidStyle::AndroidClipDrawable::type() const
{
    return QAndroidStyle::Clip;
}

void QAndroidStyle::AndroidClipDrawable::setFactor(double factor, Qt::Orientation orientation)
{
    m_factor = factor;
    m_orientation = orientation;
}

void QAndroidStyle::AndroidClipDrawable::draw(QPainter * painter, const QStyleOption *opt) const
{
    QStyleOption copy(*opt);
    if (m_orientation == Qt::Horizontal)
        copy.rect.setWidth(copy.rect.width()*m_factor);
    else
        copy.rect.setHeight(copy.rect.height()*m_factor);

    m_drawable->draw(painter, &copy);
}

QAndroidStyle::AndroidStateDrawable::AndroidStateDrawable(const QVariantMap &drawable, QAndroidStyle::ItemType itemType)
    :AndroidDrawable(drawable, itemType)
{
    QVariantList states=drawable.value(QLatin1String("stateslist")).toList();
    foreach(QVariant stateVariant, states)
    {
        QVariantMap state=stateVariant.toMap();
        const int s=extractState(state.value(QLatin1String("states")).toMap());
        if (-1 == s)
            continue;
        const AndroidDrawable * ad = fromMap(state.value(QLatin1String("drawable")).toMap(), itemType);
        if (!ad)
            continue;
        StateType item;
        item.first = s;
        item.second = ad;
        m_states<<item;
    }
}

QAndroidStyle::AndroidStateDrawable::~AndroidStateDrawable()
{
    foreach(const StateType type, m_states)
        delete type.second;
}

QAndroidStyle::AndroidDrawableType QAndroidStyle::AndroidStateDrawable::type() const
{
    return QAndroidStyle::State;
}

void QAndroidStyle::AndroidStateDrawable::draw(QPainter * painter, const QStyleOption *opt) const
{
    const AndroidDrawable* drawable=bestAndroidStateMatch(opt);
    if (drawable)
        drawable->draw(painter, opt);
}

const QAndroidStyle::AndroidDrawable* QAndroidStyle::AndroidStateDrawable::bestAndroidStateMatch(const QStyleOption *opt) const
{
    const AndroidDrawable* bestMatch = 0;
    if (!opt)
    {
        if (m_states.size())
            return m_states[0].second;
        return bestMatch;
    }
    uint bestCost=0xffff;
    foreach(const StateType & state, m_states)
    {
        if ((int)opt->state == state.first)
            return state.second;
        uint cost = 0;

        int difference=(int)opt->state^state.first;

        if ( difference & QStyle::State_Active )
            cost += 1000;

        if ( difference & QStyle::State_Enabled)
            cost += 1000;

        if ( (m_itemType == QC_Button || m_itemType == QC_EditText) && difference & QStyle::State_Raised )
            cost += 1000;

        if ( (m_itemType == QC_Button || m_itemType == QC_EditText) && (difference & QStyle::State_Sunken) )
            cost += 1000;

        if ( difference & QStyle::State_Off )
            cost += 1000;

        if ( difference & QStyle::State_On )
            cost += 1000;

        if ( difference & QStyle::State_HasFocus )
            cost += 1000;

        if ( difference & QStyle::State_Selected )
            cost += 1000;

        if (cost<bestCost)
        {
            bestCost = cost;
            bestMatch = state.second;
        }
    }
    return bestMatch;
}

int QAndroidStyle::AndroidStateDrawable::extractState(const QVariantMap & value)
{
    int state = QStyle::State_None;
    foreach (const QString key, value.keys())
    {
        bool val=value.value(key).toString()==QLatin1String("true")?true:false;
        if (key==QLatin1String("enabled") && val)
        {
            state |= QStyle::State_Enabled;
            continue;
        }

        if (key==QLatin1String("window_focused") && val)
        {
            state |= QStyle::State_Active;
            continue;
        }

        if (key==QLatin1String("focused") && val)
        {
            state |= QStyle::State_HasFocus;
            continue;
        }

        if (key==QLatin1String("checked"))
        {
            state |= (val?QStyle::State_On:QStyle::State_Off);
            continue;
        }

        if (key==QLatin1String("pressed"))
        {
            state |= (val?QStyle::State_Raised:QStyle::State_Sunken);
            state |= QStyle::State_Enabled | QStyle::State_HasFocus;
            continue;
        }

        if (key==QLatin1String("selected") && val)
        {
            state |= QStyle::State_Selected;
            state |= QStyle::State_Enabled | QStyle::State_HasFocus;
            continue;
        }

        if (key==QLatin1String("active") && val)
        {
            state |= QStyle::State_Active;
            continue;
        }

        if (key==QLatin1String("backgroud") && val)
            return -1;
    }
    return state;
}

QAndroidStyle::AndroidLayerDrawable::AndroidLayerDrawable(const QVariantMap &drawable, QAndroidStyle::ItemType itemType)
    :AndroidDrawable(drawable, itemType)
{
    QVariantList layers=drawable.value(QLatin1String("layers")).toList();
    foreach(QVariant layer, layers)
    {
        QVariantMap layerMap=layer.toMap();
        AndroidDrawable * ad = fromMap(layerMap, itemType);
        if (ad)
        {
            LayerType l;
            l.second =  ad;
            l.first = layerMap.value(QLatin1String("id")).toInt();
            m_layers<<l;
        }
    }
}

QAndroidStyle::AndroidLayerDrawable::~AndroidLayerDrawable()
{
    foreach(const LayerType & layer, m_layers)
        delete layer.second;
}

QAndroidStyle::AndroidDrawableType QAndroidStyle::AndroidLayerDrawable::type() const
{
    return QAndroidStyle::Layer;
}

void QAndroidStyle::AndroidLayerDrawable::draw(QPainter * painter, const QStyleOption *opt) const
{
    foreach(const LayerType & layer, m_layers)
        layer.second->draw(painter, opt);
}

QAndroidStyle::AndroidDrawable * QAndroidStyle::AndroidLayerDrawable::layer(int id) const
{
    foreach(const LayerType & layer, m_layers)
        if (layer.first == id)
            return layer.second;
    return 0;
}

QSize QAndroidStyle::AndroidLayerDrawable::size()
{
    QSize sz;
    foreach(const LayerType & layer, m_layers)
        sz=sz.expandedTo(layer.second->size());
    return sz;
}

QAndroidStyle::AndroidControl::AndroidControl(const QVariantMap &control, QAndroidStyle::ItemType itemType)
{
    QVariantMap::const_iterator it=control.find(QLatin1String("View_background"));
    if (it!=control.end())
        m_background = AndroidDrawable::fromMap(it.value().toMap(), itemType);
    else
        m_background = 0;

    it=control.find(QLatin1String("View_minWidth"));
    if (it!=control.end())
        m_minSize.setWidth(it.value().toInt());

    it=control.find(QLatin1String("View_minHeight"));
    if (it!=control.end())
        m_minSize.setHeight(it.value().toInt());

    it=control.find(QLatin1String("View_maxWidth"));
    if (it!=control.end())
        m_maxSize.setWidth(it.value().toInt());

    it=control.find(QLatin1String("View_maxHeight"));
    if (it!=control.end())
        m_maxSize.setHeight(it.value().toInt());
}

QAndroidStyle::AndroidControl::~AndroidControl()
{
    delete m_background;
}

void QAndroidStyle::AndroidControl::drawControl(const QStyleOption *opt, QPainter *p, const QWidget */*w*/)
{
    if (m_background)
        m_background->draw(p, opt);
}

QRect QAndroidStyle::AndroidControl::subElementRect(QStyle::SubElement /*subElement*/, const QStyleOption *option,
                             const QWidget */*widget*/) const
{
    if (const AndroidDrawable* drawable=m_background)
    {
        if (drawable->type() == State)
            drawable = static_cast<const AndroidStateDrawable*>(m_background)->bestAndroidStateMatch(option);

        const QMargins & padding=drawable->padding();

        QRect r = option->rect.adjusted(padding.left(), padding.top(), -padding.right(), -padding.bottom());

        if (r.width()<m_minSize.width())
            r.setWidth(m_minSize.width());

        if (r.height()<m_minSize.height())
            r.setHeight(m_minSize.height());

        return visualRect(option->direction, option->rect, r);
    }
    return option->rect;

}

QRect QAndroidStyle::AndroidControl::subControlRect(const QStyleOptionComplex *option, QStyle::SubControl /*sc*/,
                             const QWidget *widget) const
{
    return subElementRect(QStyle::SE_CustomBase, option, widget);
}

QSize QAndroidStyle::AndroidControl::sizeFromContents(const QStyleOption *opt,
                               const QSize &contentsSize, const QWidget */*w*/) const
{
    QSize sz;
    if (const AndroidDrawable* drawable=m_background)
    {

        if (drawable->type() == State)
            drawable=static_cast<const AndroidStateDrawable*>(m_background)->bestAndroidStateMatch(opt);
        const QMargins & padding = drawable->padding();
        sz.setWidth(padding.left()+padding.right());
        sz.setHeight(padding.top()+padding.bottom());
        if (sz.isEmpty())
            sz=drawable->size();
    }
    sz+=contentsSize;
    if (contentsSize.height()<opt->fontMetrics.height())
        sz.setHeight(sz.height()+(opt->fontMetrics.height()-contentsSize.height()));
    if (sz.height()<m_minSize.height())
        sz.setHeight(m_minSize.height());
    if (sz.width()<m_minSize.width())
        sz.setWidth(m_minSize.width());
    return sz;
}

QMargins QAndroidStyle::AndroidControl::padding()
{
    if (const AndroidDrawable* drawable=m_background)
    {
        if (drawable->type() == State)
            drawable=static_cast<const AndroidStateDrawable*>(m_background)->bestAndroidStateMatch(0);
        return drawable->padding();
    }
    return QMargins();
}


QAndroidStyle::AndroidCompoundButtonControl::AndroidCompoundButtonControl(const QVariantMap &control, ItemType itemType)
    :AndroidControl(control, itemType)
{
    QVariantMap::const_iterator it=control.find(QLatin1String("CompoundButton_button"));
    if (it!=control.end())
        m_button = AndroidDrawable::fromMap(it.value().toMap(), itemType);
    else
        m_button = 0;
}

QAndroidStyle::AndroidCompoundButtonControl::~AndroidCompoundButtonControl()
{
    delete m_button;
}

void QAndroidStyle::AndroidCompoundButtonControl::drawControl(const QStyleOption *opt, QPainter *p, const QWidget *w)
{
    AndroidControl::drawControl(opt, p, w);
    if (m_button)
        m_button->draw(p, opt);
}

QAndroidStyle::AndroidProgressBarControl::AndroidProgressBarControl(const QVariantMap &control, ItemType itemType)
    :AndroidControl(control, itemType)
{
    QVariantMap::const_iterator it=control.find(QLatin1String("ProgressBar_indeterminateDrawable"));
    if (it!=control.end())
        m_indeterminateDrawable = AndroidDrawable::fromMap(it.value().toMap(), itemType);
    else
        m_indeterminateDrawable = 0;

    it=control.find(QLatin1String("ProgressBar_progressDrawable"));
    if (it!=control.end())
        m_progressDrawable = AndroidDrawable::fromMap(it.value().toMap(), itemType);
    else
        m_progressDrawable = 0;

    it=control.find(QLatin1String("ProgressBar_progress_id"));
    if (it!=control.end())
        m_progressId= it.value().toInt();

    it=control.find(QLatin1String("ProgressBar_secondaryProgress_id"));
    if (it!=control.end())
        m_secondaryProgress_id= it.value().toInt();

    it=control.find(QLatin1String("ProgressBar_minWidth"));
    if (it!=control.end())
        m_minSize.setWidth(it.value().toInt());

    it=control.find(QLatin1String("ProgressBar_minHeight"));
    if (it!=control.end())
        m_minSize.setHeight(it.value().toInt());

    it=control.find(QLatin1String("ProgressBar_maxWidth"));
    if (it!=control.end())
        m_maxSize.setWidth(it.value().toInt());

    it=control.find(QLatin1String("ProgressBar_maxHeight"));
    if (it!=control.end())
        m_maxSize.setHeight(it.value().toInt());

}

QAndroidStyle::AndroidProgressBarControl::~AndroidProgressBarControl()
{
    delete m_progressDrawable;
    delete m_indeterminateDrawable;
}

void QAndroidStyle::AndroidProgressBarControl::drawControl(const QStyleOption *option, QPainter *p, const QWidget */*w*/)
{
    if (!m_progressDrawable)
        return;

    if (const QStyleOptionProgressBar *progressBarOption =
           qstyleoption_cast<const QStyleOptionProgressBar *>(option))
    {
        QStyleOptionProgressBarV2 progressBarV2(*progressBarOption);
        if (m_progressDrawable->type()==QAndroidStyle::Layer)
        {
            QAndroidStyle::AndroidDrawable * clipDrawable = static_cast<QAndroidStyle::AndroidLayerDrawable *>(m_progressDrawable)->layer(m_progressId);
            if (clipDrawable->type() == QAndroidStyle::Clip)
                static_cast<QAndroidStyle::AndroidClipDrawable *>(clipDrawable)->setFactor((double)progressBarV2.progress/(progressBarV2.maximum-progressBarV2.minimum), progressBarV2.orientation);
        }
        m_progressDrawable->draw(p, option);
    }
}

QRect QAndroidStyle::AndroidProgressBarControl::subElementRect(QStyle::SubElement subElement, const QStyleOption *option,
                             const QWidget *widget) const
{
    if (const QStyleOptionProgressBar *progressBarOption =
           qstyleoption_cast<const QStyleOptionProgressBar *>(option))
    {
        QStyleOptionProgressBarV2 progressBarV2(*progressBarOption);
        const bool horizontal = progressBarV2.orientation == Qt::Vertical;
        if (!m_background)
            return option->rect;

        QMargins padding = m_background->padding();
        QRect p(padding.left(), padding.top(), padding.right()-padding.left(), padding.bottom()-padding.top());
        padding=m_indeterminateDrawable->padding();
        p|=QRect(padding.left(), padding.top(), padding.right()-padding.left(), padding.bottom()-padding.top());
        padding=m_progressDrawable->padding();
        p|=QRect(padding.left(), padding.top(), padding.right()-padding.left(), padding.bottom()-padding.top());

        QRect r = option->rect.adjusted(p.left(), p.top(), -p.right(), -p.bottom());

        if (horizontal)
        {
            if (r.height()<m_minSize.height())
                r.setHeight(m_minSize.height());

            if (r.height()>m_maxSize.height())
                r.setHeight(m_maxSize.height());
        }
        else
        {
            if (r.width()<m_minSize.width())
                r.setWidth(m_minSize.width());

            if (r.width()>m_maxSize.width())
                r.setWidth(m_maxSize.width());
        }
        return visualRect(option->direction, option->rect, r);
    }
    return AndroidControl::subElementRect(subElement, option, widget);
}

QSize QAndroidStyle::AndroidProgressBarControl::sizeFromContents(const QStyleOption *opt,
                               const QSize &contentsSize, const QWidget */*w*/) const
{
    QSize sz(contentsSize);
    if (sz.height()<m_minSize.height())
        sz.setHeight(m_minSize.height());
    if (sz.width()<m_minSize.width())
        sz.setWidth(m_minSize.width());

    if (const QStyleOptionProgressBar *progressBarOption =
           qstyleoption_cast<const QStyleOptionProgressBar *>(opt))
    {
        QStyleOptionProgressBarV2 progressBarV2(*progressBarOption);
        if (progressBarV2.orientation == Qt::Vertical)
        {

            if (sz.height()>m_maxSize.height())
                sz.setHeight(m_maxSize.height());
        }
        else
        {

            if (sz.width()>m_maxSize.width())
                sz.setWidth(m_maxSize.width());
        }
    }
    return contentsSize;
}

QAndroidStyle::AndroidSeekBarControl::AndroidSeekBarControl(const QVariantMap &control, ItemType itemType)
    :AndroidProgressBarControl(control, itemType)
{
    QVariantMap::const_iterator it=control.find(QLatin1String("SeekBar_thumb"));
    if (it!=control.end())
        m_seekBarThumb = AndroidDrawable::fromMap(it.value().toMap(), itemType);
    else
        m_seekBarThumb = 0;
}

QAndroidStyle::AndroidSeekBarControl::~AndroidSeekBarControl()
{
    delete m_seekBarThumb;
}

void QAndroidStyle::AndroidSeekBarControl::drawControl(const QStyleOption *option, QPainter *p, const QWidget */*w*/)
{
    if (!m_seekBarThumb || !m_progressDrawable)
        return;

    if (const QStyleOptionSlider *styleOption =
           qstyleoption_cast<const QStyleOptionSlider *>(option))
    {
        double factor = (double)styleOption->sliderPosition/(styleOption->maximum-styleOption->minimum);
        if (m_progressDrawable->type()==QAndroidStyle::Layer)
        {
            QAndroidStyle::AndroidDrawable * clipDrawable = static_cast<QAndroidStyle::AndroidLayerDrawable *>(m_progressDrawable)->layer(m_progressId);
            if (clipDrawable->type() == QAndroidStyle::Clip)
                static_cast<QAndroidStyle::AndroidClipDrawable *>(clipDrawable)->setFactor(factor, styleOption->orientation);
        }
        const AndroidDrawable* drawable=m_seekBarThumb;
        if (drawable->type() == State)
            drawable = static_cast<const QAndroidStyle::AndroidStateDrawable*>(m_seekBarThumb)->bestAndroidStateMatch(option);
        QStyleOption copy(*option);
        copy.rect.setY((copy.rect.height()-m_minSize.height())/2);
        copy.rect.setHeight(m_minSize.height());
        copy.rect.setWidth(copy.rect.width()-drawable->size().width());
        copy.rect.translate(drawable->size().width()/2,0);
        m_progressDrawable->draw(p, &copy);
        if (styleOption->orientation == Qt::Vertical)
            qCritical()<<"Vertical slider are not supported";
        int pos=((double)copy.rect.width()*factor)-(drawable->size().width()/2);
        copy.rect.translate(pos, 0);
        copy.rect.setSize(drawable->size());
        m_seekBarThumb->draw(p, &copy);
    }
}

QSize QAndroidStyle::AndroidSeekBarControl::sizeFromContents(const QStyleOption *opt,
                               const QSize &contentsSize, const QWidget *w) const
{
    QSize sz = AndroidProgressBarControl::sizeFromContents(opt, contentsSize, w);
    if (!m_seekBarThumb)
        return sz;
    const AndroidDrawable* drawable=m_seekBarThumb;
    if (drawable->type() == State)
        drawable = static_cast<const QAndroidStyle::AndroidStateDrawable*>(m_seekBarThumb)->bestAndroidStateMatch(opt);
    return sz.expandedTo(drawable->size());
}

QRect QAndroidStyle::AndroidSeekBarControl::subControlRect(const QStyleOptionComplex *option, SubControl sc,
                             const QWidget */*widget*/) const
{
    const QStyleOptionSlider *styleOption =
                   qstyleoption_cast<const QStyleOptionSlider *>(option);

    if (m_seekBarThumb && sc == SC_SliderHandle && styleOption)
    {
            const AndroidDrawable* drawable=m_seekBarThumb;
            if (drawable->type() == State)
                drawable = static_cast<const QAndroidStyle::AndroidStateDrawable*>(m_seekBarThumb)->bestAndroidStateMatch(option);

            QRect r(option->rect);
            double factor = (double)styleOption->sliderPosition/(styleOption->maximum-styleOption->minimum);
            int pos=((double)option->rect.width()*factor)-(drawable->size().width()/2);
            r.setX(r.x()+pos);
            r.setSize(drawable->size());
            return r;
    }
    return option->rect;
}

QAndroidStyle::AndroidSpinnerControl::AndroidSpinnerControl(const QVariantMap &control, QAndroidStyle::ItemType itemType)
    :AndroidControl(control, itemType)
{}

QRect QAndroidStyle::AndroidSpinnerControl::subControlRect(const QStyleOptionComplex *option, SubControl sc,
                                                           const QWidget *widget) const
{
    if (sc == QStyle::SC_ComboBoxListBoxPopup)
        return option->rect;
    else
        return AndroidControl::subControlRect(option, sc, widget);
}

QT_END_NAMESPACE

#endif // !defined(QT_NO_STYLE_ANDROID) || defined(QT_PLUGIN)
