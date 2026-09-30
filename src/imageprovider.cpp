#include "imageprovider.h"

FrameImageProvider::FrameImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}

QImage FrameImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    // Strip query parameters from id (e.g., "left?version=1" -> "left")
    QString baseId = id.split('?').first();
    
    if (baseId == "left") {
        if (size) {
            *size = m_leftFrame.size();
        }
        return m_leftFrame;
    } else if (baseId == "right") {
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
