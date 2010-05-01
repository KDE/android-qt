#ifndef QANDROIDPLATFORMSCREEN_H
#define QANDROIDPLATFORMSCREEN_H

#include <QPlatformScreen>
QT_BEGIN_NAMESPACE

class QAndroidPlatformScreen :  public QPlatformScreen
{
    Q_OBJECT
public:
    QAndroidPlatformScreen();
    void setGeometry(QRect rect);
    virtual QRect geometry() const;

    void setDepth(int depth);
    virtual int depth() const;

    void setFormat(QImage::Format format);
    virtual QImage::Format format() const;

    virtual void setPhysicalSize(const QSize & size);
    virtual QSize physicalSize() const;

    virtual void redrawScreen();

private:
    QRect m_geometry;
    int   m_depth;
    QImage::Format m_format;
    QSize m_size;
};

QT_END_NAMESPACE

#endif // QANDROIDPLATFORMSCREEN_H
