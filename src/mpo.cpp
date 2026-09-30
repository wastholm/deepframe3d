#include "mpo.h"
#include <QFile>
#include <QImageReader>
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
        
        // Try to decode with QImageReader
        QImageReader reader(frameData, "JPG");
        QImage image = reader.read();
        
        if (!image.isNull()) {
            Frame frame;
            frame.image = image;
            frame.width = image.width();
            frame.height = image.height();
            frames.append(frame);
            
            // Stop after we have enough frames (typically 2 for stereo)
            // but continue to parse all available frames
        } else {
            qWarning() << "Failed to decode frame at" << frameStart << ":" << reader.errorString();
        }
        
        pos = frameEnd;
    }
    
    return frames;
}
