#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QStringList>
#include "mpo.h"
#include "imageprovider.h"

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(int currentFileIndex READ currentFileIndex WRITE setCurrentFileIndex NOTIFY currentFileIndexChanged)
    Q_PROPERTY(int fileCount READ fileCount NOTIFY fileCountChanged)
    Q_PROPERTY(QString currentFileName READ currentFileName NOTIFY currentFileNameChanged)
    Q_PROPERTY(int viewingMode READ viewingMode WRITE setViewingMode NOTIFY viewingModeChanged)
    Q_PROPERTY(int frameCount READ frameCount NOTIFY frameCountChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(int slideInterval READ slideInterval WRITE setSlideInterval NOTIFY slideIntervalChanged)
    Q_PROPERTY(int wiggleInterval READ wiggleInterval WRITE setWiggleInterval NOTIFY wiggleIntervalChanged)

public:
    explicit Backend(FrameImageProvider *provider, QObject *parent = nullptr);
    
    Q_INVOKABLE void loadFiles(const QStringList &filePaths);
    Q_INVOKABLE void nextFile();
    Q_INVOKABLE void prevFile();
    Q_INVOKABLE void togglePlay();
    Q_INVOKABLE void faster();
    Q_INVOKABLE void slower();

private:
    void loadCurrentFile();
    
    int currentFileIndex() const;
    int fileCount() const;
    QString currentFileName() const;
    int viewingMode() const;
    int frameCount() const;
    bool isPlaying() const;
    int slideInterval() const;
    int wiggleInterval() const;

public slots:
    void setCurrentFileIndex(int index);
    void setViewingMode(int mode);
    void setIsPlaying(bool playing);
    void setSlideInterval(int interval);
    void setWiggleInterval(int interval);
    
    void handleFramesParsed(const QVector<MpoParser::Frame> &frames);
    void handleError(const QString &message);

signals:
    void currentFileIndexChanged();
    void fileCountChanged();
    void currentFileNameChanged();
    void viewingModeChanged();
    void frameCountChanged();
    void isPlayingChanged();
    void slideIntervalChanged();
    void wiggleIntervalChanged();
    void error(const QString &message);

private:
    MpoParser m_parser;
    FrameImageProvider *m_imageProvider;
    QStringList m_fileList;
    int m_currentFileIndex;
    int m_viewingMode;
    bool m_isPlaying;
    int m_slideInterval;
    int m_wiggleInterval;
    QVector<MpoParser::Frame> m_currentFrames;
};

#endif // BACKEND_H
