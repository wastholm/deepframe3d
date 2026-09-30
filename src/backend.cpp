#include "backend.h"
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>

Backend::Backend(FrameImageProvider *provider, QObject *parent)
    : QObject(parent),
      m_imageProvider(provider),
      m_currentFileIndex(0),
      m_viewingMode(0),
      m_isPlaying(false),
      m_slideInterval(2000),
      m_wiggleInterval(200),
      m_imageVersion(0)
{
    connect(&m_parser, &MpoParser::error, this, &Backend::handleError);
}

Q_INVOKABLE void Backend::loadFiles(const QStringList &filePaths) {
    m_fileList.clear();
    
    for (const QString &path : filePaths) {
        QFileInfo info(path);
        if (info.exists() && info.isFile()) {
            m_fileList.append(path);
        }
    }
    
    if (!m_fileList.isEmpty()) {
        m_currentFileIndex = 0;
        loadCurrentFile();
    }
    
    emit fileCountChanged();
    emit currentFileIndexChanged();
    emit currentFileNameChanged();
}

void Backend::loadCurrentFile() {
    if (m_currentFileIndex >= 0 && m_currentFileIndex < m_fileList.size()) {
        QString filePath = m_fileList.at(m_currentFileIndex);
        QVector<MpoParser::Frame> frames = m_parser.parse(filePath);
        handleFramesParsed(frames);
    }
}

Q_INVOKABLE void Backend::nextFile() {
    if (m_fileList.isEmpty()) return;
    m_currentFileIndex = (m_currentFileIndex + 1) % m_fileList.size();
    loadCurrentFile();
    emit currentFileIndexChanged();
    emit currentFileNameChanged();
}

Q_INVOKABLE void Backend::prevFile() {
    if (m_fileList.isEmpty()) return;
    m_currentFileIndex = (m_currentFileIndex - 1 + m_fileList.size()) % m_fileList.size();
    loadCurrentFile();
    emit currentFileIndexChanged();
    emit currentFileNameChanged();
}

Q_INVOKABLE void Backend::togglePlay() {
    setIsPlaying(!m_isPlaying);
}

Q_INVOKABLE void Backend::faster() {
    if (m_slideInterval > 500) {
        setSlideInterval(std::max(500, m_slideInterval - 500));
    }
}

Q_INVOKABLE void Backend::slower() {
    setSlideInterval(std::min(10000, m_slideInterval + 500));
}

Q_INVOKABLE void Backend::quit() {
    QCoreApplication::quit();
}

void Backend::handleFramesParsed(const QVector<MpoParser::Frame> &frames) {
    m_currentFrames = frames;
    emit frameCountChanged();
    
    // Update image provider
    if (frames.size() >= 2) {
        m_imageProvider->setLeftFrame(frames[0].image);
        m_imageProvider->setRightFrame(frames[1].image);
    } else if (frames.size() == 1) {
        m_imageProvider->setLeftFrame(frames[0].image);
        m_imageProvider->setRightFrame(QImage());
    } else {
        m_imageProvider->clearFrames();
    }
    m_imageVersion++;
    emit framesLoaded();
}

void Backend::handleError(const QString &message) {
    emit error(message);
}

// Getters
int Backend::currentFileIndex() const { return m_currentFileIndex; }
int Backend::fileCount() const { return m_fileList.size(); }
QString Backend::currentFileName() const {
    if (m_currentFileIndex >= 0 && m_currentFileIndex < m_fileList.size()) {
        return QFileInfo(m_fileList[m_currentFileIndex]).fileName();
    }
    return QString();
}
int Backend::viewingMode() const { return m_viewingMode; }
int Backend::frameCount() const { return m_currentFrames.size(); }
bool Backend::isPlaying() const { return m_isPlaying; }
int Backend::slideInterval() const { return m_slideInterval; }
int Backend::wiggleInterval() const { return m_wiggleInterval; }
int Backend::imageVersion() const { return m_imageVersion; }

// Setters
void Backend::setCurrentFileIndex(int index) {
    if (m_currentFileIndex != index) {
        m_currentFileIndex = index;
        emit currentFileIndexChanged();
        emit currentFileNameChanged();
    }
}

void Backend::setViewingMode(int mode) {
    if (m_viewingMode != mode) {
        m_viewingMode = mode;
        emit viewingModeChanged();
    }
}

void Backend::setIsPlaying(bool playing) {
    if (m_isPlaying != playing) {
        m_isPlaying = playing;
        emit isPlayingChanged();
    }
}

void Backend::setSlideInterval(int interval) {
    if (m_slideInterval != interval) {
        m_slideInterval = interval;
        emit slideIntervalChanged();
    }
}

void Backend::setWiggleInterval(int interval) {
    if (m_wiggleInterval != interval) {
        m_wiggleInterval = interval;
        emit wiggleIntervalChanged();
    }
}
