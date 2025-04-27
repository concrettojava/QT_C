#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QVideoWidget>
#include <QString>
#include <QMediaPlayer>

class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    void setVideoPlayer(QVideoWidget *videoWidget);

    bool loadAndPlay(const QString &filePath);

    // 基本操作
    void play();
    void pause();
    void stop();

signals:

private:
    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;

};

#endif // VIDEOPLAYER_H
