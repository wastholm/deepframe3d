#include "managedimageprovider.h"

// Anaglyph matrices as data (not baked into loops)
// Left eye matrices: row-major [R,G,B] output per input [R,G,B]
static const float DUBOIS_ML[3][3] = {
    { 0.4561f,  0.500484f,  0.176381f},
    {-0.0434706f,-0.0879388f,-0.00155529f},
    {-0.0214501f,-0.113241f,  0.0720689f}};
static const float DUBOIS_MR[3][3] = {
    {-0.0160861f,-0.0174583f,-0.0177491f},
    { 0.362133f,  0.472156f,  0.290862f},
    {-0.0929334f,-0.125626f, -0.102673f}};

// Naive matrices: simple channel isolation (left=red, right=cyan)
static const float NAIVE_ML[3][3] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}};
static const float NAIVE_MR[3][3] = {
    {0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f}};

QImage makeAnaglyph(const QImage &left, const QImage &right, const float ml[3][3], const float mr[3][3]) {
    QImage l = left.convertToFormat(QImage::Format_RGB32);
    QImage r = right.convertToFormat(QImage::Format_RGB32);
    if (l.size() != r.size()) r = r.scaled(l.size());
    QImage out(l.size(), QImage::Format_RGB32);

    for (int y = 0; y < l.height(); ++y) {
        const QRgb *pl = reinterpret_cast<const QRgb*>(l.scanLine(y));
        const QRgb *pr = reinterpret_cast<const QRgb*>(r.scanLine(y));
        QRgb *po = reinterpret_cast<QRgb*>(out.scanLine(y));
        for (int x = 0; x < l.width(); ++x) {
            float lr = qRed(pl[x]) / 255.0f;
            float lg = qGreen(pl[x]) / 255.0f;
            float lb = qBlue(pl[x]) / 255.0f;
            float rr = qRed(pr[x]) / 255.0f;
            float rg = qGreen(pr[x]) / 255.0f;
            float rb = qBlue(pr[x]) / 255.0f;
            
            float R = ml[0][0]*lr + ml[0][1]*lg + ml[0][2]*lb + mr[0][0]*rr + mr[0][1]*rg + mr[0][2]*rb;
            float G = ml[1][0]*lr + ml[1][1]*lg + ml[1][2]*lb + mr[1][0]*rr + mr[1][1]*rg + mr[1][2]*rb;
            float B = ml[2][0]*lr + ml[2][1]*lg + ml[2][2]*lb + mr[2][0]*rr + mr[2][1]*rg + mr[2][2]*rb;
            
            po[x] = qRgb(qBound(0, int(R * 255), 255), 
                         qBound(0, int(G * 255), 255), 
                         qBound(0, int(B * 255), 255));
        }
    }
    return out;
}

QImage makeDuboisAnaglyph(const QImage &left, const QImage &right) {
    return makeAnaglyph(left, right, DUBOIS_ML, DUBOIS_MR);
}

QImage makeNaiveAnaglyph(const QImage &left, const QImage &right) {
    return makeAnaglyph(left, right, NAIVE_ML, NAIVE_MR);
}

ManagedImageProvider::ManagedImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image),
      m_anaglyphStyle(0)
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
        QImage result;
        if (m_anaglyphStyle == 0) {
            result = makeDuboisAnaglyph(m_leftFrame, m_rightFrame);
        } else {
            result = makeNaiveAnaglyph(m_leftFrame, m_rightFrame);
        }
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

void ManagedImageProvider::setAnaglyphStyle(int style) {
    m_anaglyphStyle = style;
}
