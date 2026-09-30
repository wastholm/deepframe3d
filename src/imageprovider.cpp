#include "imageprovider.h"

FrameImageProvider::FrameImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}

QImage FrameImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    if (id == "left") {
        if (size) {
            *size = m_leftFrame.size();
        }
        return m_leftFrame;
    } else if (id == "right") {
        if (size) {
            *size = m_rightFrame.size();
        }
        return m_rightFrame;
    }
    
    // Return empty image for unknown ids
    return QImage();
}

void FrameImageProvider::setLeftFrame(const QImage &image) {
    m_leftFrame = image;
}

void FrameImageProvider::setRightFrame(const QImage &image) {
    m_rightFrame = image;
}

void FrameImageProvider::clearFrames() {
    m_leftFrame = QImage();
    m_rightFrame = QImage();
}
