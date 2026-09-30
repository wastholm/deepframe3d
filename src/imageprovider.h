#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>

class FrameImageProvider : public QQuickImageProvider
{
public:
    FrameImageProvider();
    
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    
    void setLeftFrame(const QImage &image);
    void setRightFrame(const QImage &image);
    void clearFrames();

private:
    QImage m_leftFrame;
    QImage m_rightFrame;
};

#endif // IMAGEPROVIDER_H
