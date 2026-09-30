#include "mpo.h"
#include <QFile>
#include <QImageReader>
#include <QBuffer>
#include <QDebug>

MpoParser::MpoParser(QObject *parent) : QObject(parent) {}

QVector<MpoParser::Frame> MpoParser::parse(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(tr("Cannot open file: %1").arg(filePath));
        return {};
    }
    QByteArray data = file.readAll();
    return parse(data);
}

QVector<MpoParser::Frame> MpoParser::parse(const QByteArray &data) {
    // First, try to parse as a standard QImage (single image)
    QImage singleImage = QImage::fromData(data);
    if (!singleImage.isNull()) {
        Frame frame;
        frame.image = singleImage;
        frame.width = singleImage.width();
        frame.height = singleImage.height();
        return {frame};
    }
    
    // If that fails, try to parse as MPO (multi-frame)
    return extractFrames(data);
}

QVector<MpoParser::Frame> MpoParser::extractFrames(const QByteArray &data) {
    QVector<Frame> frames;
    
    // MPO: typically starts with APP2 marker for MPF, but we use a simpler approach:
    // Find all JPEG SOI (Start of Image: 0xFFD8) markers and extract frames
    
    const char *rawData = data.constData();
    int size = data.size();
    
    int pos = 0;
    while (pos < size - 1) {
        // Find next SOI marker (0xFFD8)
        while (pos < size - 1) {
            if (static_cast<unsigned char>(rawData[pos]) == 0xFF && 
                static_cast<unsigned char>(rawData[pos + 1]) == 0xD8) {
                break;
            }
            pos++;
        }
        
        if (pos >= size - 1) {
            break;
        }
        
        // Found SOI at pos, now find the end of this JPEG frame
        // JPEG frames typically end at next SOI or EOF
        int frameStart = pos;
        int frameEnd = size;
        
        // Look for the next SOI to determine frame boundary
        int nextSoi = pos + 2;
        while (nextSoi < size - 1) {
            if (static_cast<unsigned char>(rawData[nextSoi]) == 0xFF && 
                static_cast<unsigned char>(rawData[nextSoi + 1]) == 0xD8) {
                frameEnd = nextSoi;
                break;
            }
            nextSoi++;
        }
        
        // Extract the frame data
        QByteArray frameData = data.mid(frameStart, frameEnd - frameStart);
        
        // Try to decode directly
        QImage image = QImage::fromData(frameData);
        
        if (!image.isNull()) {
            Frame frame;
            frame.image = image;
            frame.width = image.width();
            frame.height = image.height();
            frames.append(frame);
        } else {
            // Try with explicit format
            image = QImage::fromData(frameData, "JPG");
            if (!image.isNull()) {
                Frame frame;
                frame.image = image;
                frame.width = image.width();
                frame.height = image.height();
                frames.append(frame);
            } else {
                qWarning() << "Failed to decode frame at" << frameStart;
            }
        }
        
        pos = frameEnd;
    }
    
    return frames;
}
