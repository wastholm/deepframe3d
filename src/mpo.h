#ifndef MPO_H
#define MPO_H

#include <QObject>
#include <QVector>
#include <QImage>

class MpoParser : public QObject {
    Q_OBJECT
public:
    explicit MpoParser(QObject *parent = nullptr);
    
    struct Frame {
        QImage image;
        int width = 0;
        int height = 0;
    };

    QVector<Frame> parse(const QString &filePath);
    QVector<Frame> parse(const QByteArray &data);

signals:
    void error(const QString &message);

private:
    QVector<Frame> extractFrames(const QByteArray &data);
    QVector<Frame> parseJps(const QByteArray &data);
    QVector<int> findMpoFrameOffsets(const QByteArray &data);
};

#endif // MPO_H
