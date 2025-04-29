#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H
#include <QObject>
#include <QVideoWidget>
#include <QString>
#include <QMediaPlayer>
#include <QList>
#include <QGridLayout>
#include <QWidget>
#include <QScrollArea>

class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoPlayer(QObject *parent = nullptr,
                         QVideoWidget *videoWidget = nullptr,
                         const QStringList &videoList = QStringList());
    ~VideoPlayer();

    // 播放模式枚举
    enum PlayMode {
        SINGLE_MODE,  // 单视频模式
        MAJOR_MODE,   // 主视频模式（2x2播放前四个）
        GLOBAL_MODE   // 全局模式（播放所有视频）
    };

public slots:
    // 模式切换槽函数
    void playSingle();  // 单视频模式
    void playMain();    // 主视频模式（2x2布局）
    void playGlobal();  // 全局模式（网格布局）

    // 选择特定视频播放
    void selectVideo(int index);

private:
    PlayMode m_currentMode;
    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;
    QWidget *m_containerWidget;     // 容器控件
    QGridLayout *m_gridLayout;      // 网格布局

    QScrollArea *m_selectorScrollArea; // 视频选择器滚动区域
    QWidget *m_selectorWidget;      // 视频选择器

    QList<QMediaPlayer*> m_players;      // 播放器列表
    QList<QVideoWidget*> m_videoWidgets; // 视频控件列表
    QStringList m_videoList;             // 视频文件路径列表
    int m_currentVideoIndex;             // 当前选中的视频索引

    // 加载并显示单个视频
    bool loadVideo(int index);

    // 创建视频播放控件
    void createVideoWidgets();

    // 根据模式布局视频控件
    void arrangeVideoWidgets();

    // 创建视频选择器
    void createVideoSelector();

    // 显示/隐藏视频选择器
    void showVideoSelector(bool show);

    // 事件过滤器（用于鼠标悬停检测）
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // VIDEOPLAYER_H
