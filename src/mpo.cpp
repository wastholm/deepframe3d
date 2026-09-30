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
    // First, try to parse as JPS (JPEG Stereo) format
    QVector<Frame> frames = parseJps(data);
    if (!frames.isEmpty()) {
        return frames;
    }
    
    // Then, try to extract multiple frames (MPO format)
    frames = extractFrames(data);
    
    // If we got multiple frames, return them
    if (frames.size() > 1) {
        return frames;
    }
    
    // If extractFrames failed or returned only 1 frame, try as single image
    QImage singleImage = QImage::fromData(data);
    if (!singleImage.isNull()) {
        Frame frame;
        frame.image = singleImage;
        frame.width = singleImage.width();
        frame.height = singleImage.height();
        return {frame};
    }
    
    // Return whatever extractFrames gave us (might be 1 frame or empty)
    return frames;
}

QVector<MpoParser::Frame> MpoParser::extractFrames(const QByteArray &data) {
    QVector<Frame> frames;
    
    const char *rawData = data.constData();
    int size = data.size();
    
    // Try to parse as MPF (Multi-Picture Format) first
    // MPF uses APP2 marker (0xFFE2) with specific structure
    QVector<int> mpoOffsets = findMpoFrameOffsets(data);
    // qDebug() << "MPF parsing found" << mpoOffsets.size() << "frame offsets:" << mpoOffsets;
    if (!mpoOffsets.isEmpty()) {
        for (int offset : mpoOffsets) {
            if (offset >= 0 && offset < size) {
                int nextOffset = size;
                // Find end of this frame (next MPF offset or next SOI)
                for (int i = mpoOffsets.indexOf(offset) + 1; i < mpoOffsets.size(); i++) {
                    if (mpoOffsets[i] > offset) {
                        nextOffset = mpoOffsets[i];
                        break;
                    }
                }
                
                QByteArray frameData = data.mid(offset, nextOffset - offset);
                QImage image = QImage::fromData(frameData);
                if (image.isNull()) {
                    image = QImage::fromData(frameData, "JPG");
                }
                
                if (!image.isNull()) {
                    Frame frame;
                    frame.image = image;
                    frame.width = image.width();
                    frame.height = image.height();
                    frames.append(frame);
                }
            }
        }
        
        if (!frames.isEmpty()) {
            return frames;
        }
    }
    
    // Fallback: Find all JPEG SOI (Start of Image: 0xFFD8) markers and extract frames
    
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
        
        // Found SOI at pos, extract from here to end of file
        int frameStart = pos;
        QByteArray frameData = data.mid(frameStart, size - frameStart);
        
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
            }
        }
        
        // Move to next position after this SOI to avoid infinite loops
        pos = frameStart + 2;
    }
    
    // Sort frames by size (largest first)
    std::sort(frames.begin(), frames.end(), [](const Frame &a, const Frame &b) {
        return (a.width * a.height) > (b.width * b.height);
    });
    
    // Keep only the 2 largest frames
    if (frames.size() > 2) {
        frames.resize(2);
    }
    
    return frames;
}

// Parse JPS (JPEG Stereo) format
// JPS: Left eye is standard JPEG, right eye is in APP0+ marker
QVector<MpoParser::Frame> MpoParser::parseJps(const QByteArray &data) {
    QVector<Frame> frames;
    
    const char *rawData = data.constData();
    int size = data.size();
    
    // First frame: the standard JPEG (left eye)
    QImage leftImage = QImage::fromData(data);
    if (leftImage.isNull()) {
        return frames;  // Not a valid JPEG
    }
    
    // Add left frame
    Frame leftFrame;
    leftFrame.image = leftImage;
    leftFrame.width = leftImage.width();
    leftFrame.height = leftImage.height();
    frames.append(leftFrame);
    
    // Now look for the right eye image in APP0 marker with "JPS " identifier
    for (int i = 0; i < size - 1; i++) {
        if (static_cast<unsigned char>(rawData[i]) == 0xFF &&
            static_cast<unsigned char>(rawData[i + 1]) == 0xE0) {
            // APP0 marker
            if (i + 4 < size) {
                int length = (static_cast<unsigned char>(rawData[i + 2]) << 8) | 
                             static_cast<unsigned char>(rawData[i + 3]);
                
                // Check for JPS signature: "JPS " (note the space)
                if (length > 8 && i + 8 <= size &&
                    rawData[i + 4] == 'J' &&
                    rawData[i + 5] == 'P' &&
                    rawData[i + 6] == 'S' &&
                    rawData[i + 7] == ' ') {
                    

                    
                    // The right eye image starts after the marker header
                    // APP0 marker structure: FF E0 [length] [identifier] [data]
                    // For JPS: identifier is "JPS " (4 bytes), then the right eye JPEG
                    int imageStart = i + 4 + 4;  // Skip FF E0 + length (2) + "JPS " (4)
                    int imageLength = length - 4;  // Subtract identifier length
                    
                    if (imageStart + imageLength <= size) {
                        QByteArray rightData = data.mid(imageStart, imageLength);
                        QImage rightImage = QImage::fromData(rightData);
                        
                        if (rightImage.isNull()) {
                            rightImage = QImage::fromData(rightData, "JPG");
                        }
                        
                        if (!rightImage.isNull()) {
                            Frame rightFrame;
                            rightFrame.image = rightImage;
                            rightFrame.width = rightImage.width();
                            rightFrame.height = rightImage.height();
                            frames.append(rightFrame);
                            return frames;  // Success!
                        }
                    }
                }
            }
        }
    }
    
    // If we only found the left frame, return empty (not a valid JPS)
    return QVector<Frame>();
}

// Parse MPF (Multi-Picture Format) to find frame offsets
QVector<int> MpoParser::findMpoFrameOffsets(const QByteArray &data) {
    QVector<int> offsets;
    
    const char *rawData = data.constData();
    int size = data.size();
    
    // MPF format: After the first JPEG frame, there's an APP2 marker
    // APP2 marker: 0xFFE2 followed by 2-byte length, then "MPF\0"
    
    // Look for APP2 markers (0xFFE2)
    for (int i = 0; i < size - 1; i++) {
        if (static_cast<unsigned char>(rawData[i]) == 0xFF &&
            static_cast<unsigned char>(rawData[i + 1]) == 0xE2) {
            
            // Check if this is an MPF marker
            // APP2 length is next 2 bytes (big-endian)
            if (i + 4 < size) {
                int length = (static_cast<unsigned char>(rawData[i + 2]) << 8) | 
                             static_cast<unsigned char>(rawData[i + 3]);
                
                // Check for MPF signature: "MPF\0" at offset i+4
                if (i + 8 <= size &&
                    rawData[i + 4] == 'M' &&
                    rawData[i + 5] == 'P' &&
                    rawData[i + 6] == 'F' &&
                    rawData[i + 7] == 0) {
                    
                    // MPF header structure:
                    // - 4 bytes: "MPF\0"
                    // - 4 bytes: MPF version (e.g., "0100")
                    // - 4 bytes: Number of images (big-endian)
                    // - For each image: 4 bytes offset (big-endian, from start of file)
                    
                    int headerStart = i + 4;  // After APP2 marker + length
                    int numImagesOffset = headerStart + 8;  // Skip "MPF\0" + version
                    
                    if (numImagesOffset + 4 <= size) {
                        int numImages = (static_cast<unsigned char>(rawData[numImagesOffset]) << 24) |
                                       (static_cast<unsigned char>(rawData[numImagesOffset + 1]) << 16) |
                                       (static_cast<unsigned char>(rawData[numImagesOffset + 2]) << 8) |
                                       static_cast<unsigned char>(rawData[numImagesOffset + 3]);
                        
                        // Sanity check: numImages should be reasonable (2-10)
                        if (numImages < 2 || numImages > 10) {
                            break;
                        }
                        
                        int offsetsStart = numImagesOffset + 4;
                        for (int j = 0; j < numImages; j++) {
                            if (offsetsStart + j * 4 + 4 <= size) {
                                int offset = (static_cast<unsigned char>(rawData[offsetsStart + j * 4]) << 24) |
                                            (static_cast<unsigned char>(rawData[offsetsStart + j * 4 + 1]) << 16) |
                                            (static_cast<unsigned char>(rawData[offsetsStart + j * 4 + 2]) << 8) |
                                            static_cast<unsigned char>(rawData[offsetsStart + j * 4 + 3]);
                                
                                offsets.append(offset);
                            }
                        }
                    }
                    break;  // Found MPF, stop looking
                }
            }
        }
    }
    
    return offsets;
}
