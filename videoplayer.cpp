#include "videoplayer.h"
#include <QDebug>
#include <QUrl>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QEvent>
#include <QEnterEvent>
#include <QEvent>
#include <QTimer>
#include <Qtmath>
#include <QtGlobal>


VideoPlayer::VideoPlayer(QObject *parent, QVideoWidget *videoWidget, const QStringList &videoList)
    : QObject(parent)
    , m_videoWidget(videoWidget)
// 其他初始化...
    , m_containerWidget(nullptr)
    , m_gridLayout(nullptr)
    , m_selectorScrollArea(nullptr)
    , m_selectorWidget(nullptr)
    , m_currentMode(SINGLE_MODE)
    , m_currentVideoIndex(0)
{
    // 创建主媒体播放器
    m_player = new QMediaPlayer(this);

    // 如果提供了视频控件，初始化它
    if(m_videoWidget) {
        // 设置事件过滤器以捕获鼠标事件
        m_videoWidget->installEventFilter(this);

        // 设置主播放器的输出
        m_player->setVideoOutput(m_videoWidget);

        // 创建网格布局容器
        m_containerWidget = new QWidget(m_videoWidget);
        m_gridLayout = new QGridLayout(m_containerWidget);
        m_gridLayout->setContentsMargins(0, 0, 0, 0);
        m_gridLayout->setSpacing(2);
        m_containerWidget->setLayout(m_gridLayout);
        m_containerWidget->hide();
    }

    // 如果提供了视频列表，初始化它
    if(!videoList.isEmpty()) {
        m_videoList = videoList;
        createVideoSelector();
        loadVideo(0);
        playSingle();
    }
}

VideoPlayer::~VideoPlayer()
{
    // 清理资源
    delete m_player;

    // 清理其他播放器
    for(auto player : m_players) {
        if(player && player != m_player) {
            delete player;
        }
    }

    // 清理布局和容器
    if(m_containerWidget) {
        delete m_containerWidget;
    }

    if(m_selectorScrollArea) {
        delete m_selectorScrollArea;
    }
}

bool VideoPlayer::loadVideo(int index)
{
    if(index < 0 || index >= m_videoList.size()) {
        qDebug() << "Error: 无效的视频索引:" << index;
        return false;
    }

    QString filePath = m_videoList.at(index);
    QFileInfo fileInfo(filePath);
    if(!fileInfo.exists() || !fileInfo.isFile()) {
        qDebug() << "Error: 文件不存在或不是有效文件:" << filePath;
        return false;
    }

    // 更新当前视频索引
    m_currentVideoIndex = index;

    // 设置媒体
    m_player->setMedia(QUrl::fromLocalFile(filePath));
    m_player->play(); // Start playback to load the first frame
    m_player->pause(); // Immediately pause it


    return true;
}

void VideoPlayer::playSingle()
{
    if(m_videoList.isEmpty()) {
        qDebug() << "Error: 无视频可播放";
        return;
    }

    m_currentMode = SINGLE_MODE;

    // 停止所有其他播放器
    for(auto player : m_players) {
        if(player && player != m_player) {
            player->stop();
        }
    }

    // 隐藏网格容器，显示主视频控件
    if(m_containerWidget) {
        m_containerWidget->hide();
    }

    // 确保主视频控件可见
    if(m_videoWidget) {
        m_videoWidget->show();
    }

    // 如果当前没有加载视频，则加载当前索引的视频
    if(m_player->media().isNull()) {
        loadVideo(m_currentVideoIndex);
    }

    qDebug() << "切换到单视频模式";
}

void VideoPlayer::playMain()
{
    if(m_videoList.isEmpty()) {
        qDebug() << "Error: 无视频可播放";
        return;
    }

    m_currentMode = MAJOR_MODE;

    // 隐藏视频选择器
    showVideoSelector(false);

    // 创建并排列视频控件
    createVideoWidgets();
    arrangeVideoWidgets();

    qDebug() << "切换到主视频模式";
}

void VideoPlayer::playGlobal()
{
    if(m_videoList.isEmpty()) {
        qDebug() << "Error: 无视频可播放";
        return;
    }

    m_currentMode = GLOBAL_MODE;

    // 隐藏视频选择器
    showVideoSelector(false);

    // 创建并排列视频控件
    createVideoWidgets();
    arrangeVideoWidgets();

    qDebug() << "切换到全局视频模式";
}

void VideoPlayer::selectVideo(int index)
{
    if(index < 0 || index >= m_videoList.size()) {
        qDebug() << "Error: 无效的视频索引:" << index;
        return;
    }

    // 加载选中的视频
    loadVideo(index);

    // 如果当前不是单视频模式，切换到单视频模式
    if(m_currentMode != SINGLE_MODE) {
        playSingle();
    }
}

void VideoPlayer::createVideoWidgets()
{
    // 清理现有的播放器和视频控件
    for(auto player : m_players) {
        if(player && player != m_player) {
            player->stop();
            delete player;
        }
    }

    m_players.clear();
    m_videoWidgets.clear();

    // 添加主播放器和视频控件
    m_players.append(m_player);
    m_videoWidgets.append(m_videoWidget);

    // 确定要创建的视频数量
    int count = (m_currentMode == MAJOR_MODE) ?
                    qMin(4, m_videoList.size()) :
                    m_videoList.size();

    // 为每个视频创建播放器和视频控件
    for(int i = 1; i < count; i++) {
        // 创建新的播放器
        QMediaPlayer *player = new QMediaPlayer(this);
        m_players.append(player);

        // 创建新的视频控件
        QVideoWidget *videoWidget = new QVideoWidget();
        videoWidget->setStyleSheet("background-color: black;");
        m_videoWidgets.append(videoWidget);

        // 设置播放器输出
        player->setVideoOutput(videoWidget);

        // 设置媒体源
        player->setMedia(QUrl::fromLocalFile(m_videoList.at(i)));
    }
}

void VideoPlayer::arrangeVideoWidgets()
{
    if(!m_gridLayout || !m_containerWidget || m_videoList.isEmpty()) {
        return;
    }

    // 清空网格布局
    while(QLayoutItem *item = m_gridLayout->takeAt(0)) {
        if(item->widget()) {
            item->widget()->hide();
        }
        delete item;
    }

    // 根据当前模式设置布局
    if(m_currentMode == SINGLE_MODE) {
        // 单视频模式，隐藏网格容器
        m_containerWidget->hide();

        // 显示主视频控件
        if(m_videoWidget) {
            m_videoWidget->show();
        }

        return;
    }

    // 多视频模式，显示网格容器
    m_containerWidget->show();
    m_containerWidget->setGeometry(m_videoWidget->geometry());

    // 确定要显示的视频数量
    int count = (m_currentMode == MAJOR_MODE) ?
                    qMin(4, m_videoList.size()) :
                    m_videoList.size();

    // 确定网格大小
    int rows, cols;
    if(m_currentMode == MAJOR_MODE) {
        rows = cols = 2; // 2x2网格
    } else {
        // 计算适合的网格大小
        cols = qCeil(qSqrt(count));
        rows = qCeil(double(count) / cols);
    }

    // 添加视频控件到网格
    for(int i = 0; i < count; i++) {
        if(i < m_videoWidgets.size()) {
            QVideoWidget *videoWidget = m_videoWidgets.at(i);
            int row = i / cols;
            int col = i % cols;

            m_gridLayout->addWidget(videoWidget, row, col);
            videoWidget->show();

            // 播放视频
            if(i < m_players.size() && m_players.at(i)) {
                m_players.at(i)->play();
            }
        }
    }
}

void VideoPlayer::createVideoSelector()
{
    if(!m_videoWidget || m_videoList.isEmpty()) {
        return;
    }

    // 清理现有的视频选择器
    if(m_selectorScrollArea) {
        delete m_selectorScrollArea;
        m_selectorScrollArea = nullptr;
    }

    // 创建滚动区域
    m_selectorScrollArea = new QScrollArea(m_videoWidget);
    m_selectorScrollArea->setFrameShape(QFrame::NoFrame);
    m_selectorScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_selectorScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_selectorScrollArea->setWidgetResizable(true);
    m_selectorScrollArea->setStyleSheet("background-color: rgba(0, 0, 0, 180);");

    // 创建选择器内容区域
    m_selectorWidget = new QWidget();
    QVBoxLayout *selectorLayout = new QVBoxLayout(m_selectorWidget);

    // 添加视频文件名按钮
    for(int i = 0; i < m_videoList.size(); i++) {
        QString videoPath = m_videoList.at(i);
        QFileInfo fileInfo(videoPath);

        QPushButton *videoButton = new QPushButton();
        videoButton->setText(fileInfo.fileName());
        videoButton->setMinimumHeight(40);
        videoButton->setCursor(Qt::PointingHandCursor);
        videoButton->setFlat(true);
        videoButton->setStyleSheet("color: white; text-align: left; padding: 5px; margin: 2px;");

        // 连接点击事件
        connect(videoButton, &QPushButton::clicked, this, [this, i](){
            selectVideo(i);
        });

        selectorLayout->addWidget(videoButton);
    }

    // 添加间隔
    selectorLayout->addStretch();

    // 设置选择器内容
    m_selectorScrollArea->setWidget(m_selectorWidget);

    // 设置初始位置（右侧）
    int width = 200; // 选择器宽度
    m_selectorScrollArea->setGeometry(
        m_videoWidget->width() - width,
        0,
        width,
        m_videoWidget->height()
        );

    // 默认隐藏
    m_selectorScrollArea->hide();

    // 为选择器设置事件过滤器
    m_selectorScrollArea->installEventFilter(this);
}

void VideoPlayer::showVideoSelector(bool show)
{
    if(!m_selectorScrollArea) {
        return;
    }

    if(show && m_currentMode == SINGLE_MODE) {
        // 更新选择器位置
        int width = 200; // 选择器宽度
        m_selectorScrollArea->setGeometry(
            m_videoWidget->width() - width,
            0,
            width,
            m_videoWidget->height()
            );
        m_selectorScrollArea->show();
    } else {
        m_selectorScrollArea->hide();
    }
}

bool VideoPlayer::eventFilter(QObject *watched, QEvent *event)
{
    // 只有在单视频模式下处理事件
    if(m_currentMode == SINGLE_MODE) {
        // 处理视频控件上的事件
        if(watched == m_videoWidget) {
            if(event->type() == QEvent::Enter) {
                // 鼠标进入时显示视频选择器
                showVideoSelector(true);
                return false;
            }
            else if(event->type() == QEvent::Leave) {
                // 鼠标离开时检查是否移动到了选择器上
                QTimer::singleShot(300, this, [this](){
                    if(!m_selectorScrollArea->underMouse()) {
                        showVideoSelector(false);
                    }
                });
                return false;
            }
        }

        // 处理选择器上的鼠标离开事件
        if(watched == m_selectorScrollArea && event->type() == QEvent::Leave) {
            // 鼠标离开选择器时检查是否移动到了视频控件上
            if(!m_videoWidget->underMouse()) {
                showVideoSelector(false);
            }
            return false;
        }
    }

    return QObject::eventFilter(watched, event);
}
