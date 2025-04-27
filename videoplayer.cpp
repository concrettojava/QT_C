#include "videoplayer.h"
#include <QDebug>
#include <QUrl>
#include <QFileInfo>

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject(parent),m_videoWidget(nullptr)
{
    m_player = new QMediaPlayer(this);
}

VideoPlayer::~VideoPlayer()
{
    // 停止播放并释放资源
    stop();
    delete m_player;
}

void VideoPlayer::setVideoPlayer(QVideoWidget *videoWidget)
{
    m_videoWidget = videoWidget;
    if (m_player && m_videoWidget) {
        m_player->setVideoOutput(m_videoWidget);
    }
}

bool VideoPlayer::loadAndPlay(const QString &filePath){


    QFileInfo fileInfo(filePath);
    if(!fileInfo.exists()||!fileInfo.isFile()){
        qDebug() <<"文件不存在:"<<filePath;
        return false;
    }

    m_player->setMedia(QUrl::fromLocalFile(filePath));
    play();
    return true;
}

void VideoPlayer::play(){
    if(m_player){
        m_player->play();
    }
}

void VideoPlayer::pause()
{
    if (m_player) {
        m_player->pause();
    }
}

void VideoPlayer::stop()
{
    if (m_player) {
        m_player->stop();
    }
}
