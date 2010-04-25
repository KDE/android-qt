#include "qandroidplatformscreen.h"
#include "qandroidplatformintegration.h"

QT_BEGIN_NAMESPACE

QAndroidPlatformScreen::QAndroidPlatformScreen()
    :m_geometry(QRect(0, 0, QAndroidPlatformIntegration::mDefaultGeometryWidth, QAndroidPlatformIntegration::mDefaultGeometryHeight))
    ,m_size(QSize(QAndroidPlatformIntegration::mDefaultPhysicalSizeWidth, QAndroidPlatformIntegration::mDefaultPhysicalSizeHeight))
{
    m_format = QImage::Format_RGB16;
    m_depth = 16;
}

void QAndroidPlatformScreen::setGeometry(QRect rect)
{
    m_geometry = rect;
    emit screenResized(rect);
}
QRect QAndroidPlatformScreen::geometry() const
{
    return m_geometry;
}

void QAndroidPlatformScreen::setFormat(QImage::Format format)
{
    m_format = format;
}

QImage::Format QAndroidPlatformScreen::format() const
{
    return m_format;
}

void QAndroidPlatformScreen::setDepth(int depth)
{
    m_depth = depth;
}
int QAndroidPlatformScreen::depth() const
{
    return m_depth;
}

void QAndroidPlatformScreen::setPhysicalSize(const QSize & size)
{
    m_size=size;
}

QSize QAndroidPlatformScreen::physicalSize() const
{
    return m_size;
}

void QAndroidPlatformScreen::redrawScreen()
{
    emit(update());
}

QT_END_NAMESPACE
