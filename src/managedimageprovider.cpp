#include "managedimageprovider.h"

QImage makeDuboisAnaglyph(const QImage &left, const QImage &right) {
    QImage l = left.convertToFormat(QImage::Format_RGB32);
    QImage r = right.convertToFormat(QImage::Format_RGB32);
    if (l.size() != r.size()) r = r.scaled(l.size());
    QImage out(l.size(), QImage::Format_RGB32);

    // Dubois red/cyan matrices (output R,G,B rows; input l.r,l.g,l.b / r.r,r.g,r.b)
    static const float ML[3][3] = {
        { 0.4561f,  0.500484f,  0.176381f},
        {-0.0434706f,-0.0879388f,-0.00155529f},
        {-0.0214501f,-0.113241f,  0.0720689f}};
    static const float MR[3][3] = {
        {-0.0160861f,-0.0174583f,-0.0177491f},
        { 0.362133f,  0.472156f,  0.290862f},
        {-0.0929334f,-0.125626f, -0.102673f}};

    for (int y = 0; y < l.height(); ++y) {
        const QRgb *pl = reinterpret_cast<const QRgb*>(l.scanLine(y));
        const QRgb *pr = reinterpret_cast<const QRgb*>(r.scanLine(y));
        QRgb *po = reinterpret_cast<QRgb*>(out.scanLine(y));
        for (int x = 0; x < l.width(); ++x) {
            float lr = qRed(pl[x]),   lg = qGreen(pl[x]),   lb = qBlue(pl[x]);
            float rr = qRed(pr[x]),   rg = qGreen(pr[x]),   rb = qBlue(pr[x]);
            float R = ML[0][0]*lr + ML[0][1]*lg + ML[0][2]*lb + MR[0][0]*rr + MR[0][1]*rg + MR[0][2]*rb;
            float G = ML[1][0]*lr + ML[1][1]*lg + ML[1][2]*lb + MR[1][0]*rr + MR[1][1]*rg + MR[1][2]*rb;
            float B = ML[2][0]*lr + ML[2][1]*lg + ML[2][2]*lb + MR[2][0]*rr + MR[2][1]*rg + MR[2][2]*rb;
            po[x] = qRgb(qBound(0, int(R), 255), qBound(0, int(G), 255), qBound(0, int(B), 255));
        }
    }
    return out;
}

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
    } else if (baseId == "anaglyph") {
        QImage result = makeDuboisAnaglyph(m_leftFrame, m_rightFrame);
        if (size) {
            *size = result.size();
        }
        return result;
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
