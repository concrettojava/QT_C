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
    bool loadAndShow(const QString &filePath); // 加载并显示但不播放

    void setVideoList(const QStringList &videoList); // 设置视频列表

    enum PlayMode{
        SINGLE_MODE,
        MAJOR_MODE,
        GLOBAL_MODE
    };

public slots:
    void playSingle();  // 单视频模式
    void playMain();    // 主要模式（前四个视频）
    void playGlobal();  // 全局模式（所有视频）
    void selectVideo(int index); // 选择特定视频

signals:
    void videoHovered(); // 鼠标悬停在视频上

private:
    PlayMode m_currentMode;
    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;

    // 基本操作
    void play();
    void pause();
    void stop();

    QList<QMediaPlayer*> m_players;
    QList<QVideoWidget*> m_videoWidgets;
    QStringList m_videoList;
    int m_currentVideoIndex = 0; // 当前选中的视频索引

    void createVideoWidgets(); // 创建视频窗口
    void arrangeVideoWidgets(); // 排列视频窗口
    bool eventFilter(QObject *watched, QEvent *event) override; // 事件过滤器用于检测鼠标悬停
};

#endif // VIDEOPLAYER_H
