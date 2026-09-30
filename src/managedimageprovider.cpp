#include "managedimageprovider.h"

ManagedImageProvider::ManagedImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage ManagedImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
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

void ManagedImageProvider::setLeftFrame(const QImage &image) {
    m_leftFrame = image;
}

void ManagedImageProvider::setRightFrame(const QImage &image) {
    m_rightFrame = image;
}

void ManagedImageProvider::clearFrames() {
    m_leftFrame = QImage();
    m_rightFrame = QImage();
}
