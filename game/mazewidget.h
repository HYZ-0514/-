#ifndef MAZEWIDGET_H
#define MAZEWIDGET_H


#include <QWidget>
#include <QMessageBox>      //Qt信息窗口头文件
#include <QPainter>         //Qt绘图头文件
#include <QDebug>           //QtDebug头文件
#include <QKeyEvent>        //Qt按键事件头文件
#include <QTimer>           //Qt计时器头文件
#include <QInputDialog>     //Qt输入对话框头文件
#include<QTcpSocket>    // 客户端Socket
#include <QTcpServer>   // 服务端
#include <QNetworkInterface>
#include <QHostAddress>
#include <QMediaPlayer>   // 多媒体播放器
#include <QAudioOutput>   // 音频输出
#include "maze.h"           //迷宫类头文件

QT_BEGIN_NAMESPACE
namespace Ui {
    class mazeWidget;
}
QT_END_NAMESPACE
class mazeWidget : public QWidget {
    Q_OBJECT
public:
    mazeWidget(QWidget* parent = nullptr);
    ~mazeWidget();

protected:
    void paintEvent(QPaintEvent*);        //绘图事件
    void keyPressEvent(QKeyEvent*);       //按键按下事件
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void on_start_btn_clicked();    //|
    void on_stop_ptn_clicked();     //|
    void on_end_btn_clicked();      //|
    void on_rule_btn_clicked();     //|各按钮点击槽函数
    void on_setting_btn_clicked();  //|
    void time_update();             //时间更新槽函数
    void on_solve_btn_clicked();
    void on_stupid_clicked();
    void on_disease_clicked();  // 设障按钮点击槽函数
    void on_hostMode_clicked();   // 主播模式按钮
    void on_audienceMode_clicked(); // 观众模式按钮
    void on_startWatch_clicked(); // 开始观战按钮
    void newConnection();         // 主播端有新连接
    void readyRead();             // 有数据可读
    void on_antColony_clicked();



private:
    Ui::mazeWidget* ui;         //ui对象
    maze* map;//迷宫对象
    bool obstacleMode;  // 标记是否处于设障模式
    bool painting_switch;       //绘图开关
    bool timing_switch;         //计时开关
    bool keybord_switch;        //键盘响应开关
    bool stop_switch;           //暂停按钮状态
    QPixmap menuImage; // 菜单图片
    bool showMenu;     // 是否显示菜单

    int grade;                  //分数
    int time;                   //时间
    QTimer* timer;

    QTcpServer* server;      // 主播端使用的服务器
    QTcpSocket* clientSocket; // 观众端使用的客户端socket
    QTcpSocket* serverSocket; // 主播端使用的客户端socket(用于发送数据)

    void startBroadcasting(); // 开始直播
    void stopBroadcasting();  // 停止直播
    void startWatching();     // 开始观看
    void stopWatching();      // 停止观看
    void sendGameState();     // 发送游戏状态
    void readGameState();     // 读取游戏状态

    QMediaPlayer* bgmPlayer;  // BGM播放器
    QAudioOutput* audioOutput; // 音频输出

    AntColony* antColony;
    QTimer* antAnimationTimer;
    void startAntAnimation();
    void stopAntAnimation();
    void updateAntAnimation();
    void updateAntsRegion();
    int m_aStarTime;
    void clearAntColony(); // 新增清除蚁群的函数
};
#endif // MAZEWIDGET_H
