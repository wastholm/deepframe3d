#ifndef MANAGEDIMAGEPROVIDER_H
#define MANAGEDIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>

class ManagedImageProvider : public QQuickImageProvider
{
public:
    explicit ManagedImageProvider();
    
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    
    void setLeftFrame(const QImage &image);
    void setRightFrame(const QImage &image);
    void clearFrames();
    void setAnaglyphStyle(int style);

private:
    QImage m_leftFrame;
    QImage m_rightFrame;
    int m_anaglyphStyle;
};

#endif // MANAGEDIMAGEPROVIDER_H
