#include "mazewidget.h"
#include "ui_mazewidget.h"
#include<QRect>
#include<QPainter>
#include <QElapsedTimer>


const double stupid_num = 0.2;
bool if_stupid = false;
mazeWidget::mazeWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::mazeWidget), map(new maze(20)) // 将 maze 实例传递给 solve 的构造函数
    , painting_switch(false), timing_switch(false)
    , keybord_switch(false), stop_switch(false), grade(0), time(0), obstacleMode(false), showMenu(true), antColony(nullptr), antAnimationTimer(nullptr) {
    //TODO:状态栏
    ui->setupUi(this);
    ui->progressBar->setVisible(false);                 //初始隐藏进度条
    ui->end_btn->setEnabled(false);                     //设置终止按钮禁用
    ui->stop_ptn->setEnabled(false);                    //设置暂停按钮禁用
    ui->grade_value->setText("  ");                     //设置分数值显示为空
    ui->time_value->setText("  ");                      //设置时间值显示为空
    map->makemap();                                     //生成地图
    timer = new QTimer(this);                                           //初始化计时器
    connect(timer, &QTimer::timeout, this, &mazeWidget::time_update);   //链接时间更新信号与槽
    connect(ui->disease, &QPushButton::clicked, this, &mazeWidget::on_disease_clicked);
    ui->plaque_time->setText("  ");
    ui->plaque_grade->setText("  ");
    ui->label->setVisible(true);
    ui->solve_btn->setEnabled(false);
    // 主窗口背景（改为暖灰到浅沙色渐变）
    this->setStyleSheet(
        "background: qlineargradient(x1:0 y1:0, x2:1 y2:1, stop:0 #f5f7fa, stop:1 #c3cfe2);"
        "color: #333;"
    );
    server = nullptr;
    clientSocket = nullptr;
    serverSocket = nullptr;

    audioOutput = new QAudioOutput(this);
    bgmPlayer = new QMediaPlayer(this);

    // 设置音频输出
    bgmPlayer->setAudioOutput(audioOutput);

    // 设置音量（0.0-1.0）
    audioOutput->setVolume(0.5); // 50%音量

    // 加载BGM文件（确保文件路径正确）
    bgmPlayer->setSource(QUrl::fromLocalFile("bgm/duck.wav"));

    // 设置循环播放
    bgmPlayer->setLoops(QMediaPlayer::Infinite);

    // 连接错误信号
    connect(bgmPlayer, &QMediaPlayer::errorOccurred, [](QMediaPlayer::Error error, const QString& errorString) {
        qDebug() << "BGM Error:" << errorString;
        });

    // 连接按钮信号槽
    connect(ui->hostMode, &QPushButton::clicked, this, &mazeWidget::on_hostMode_clicked);
    connect(ui->audienceMode, &QPushButton::clicked, this, &mazeWidget::on_audienceMode_clicked);
    connect(ui->startWatch, &QPushButton::clicked, this, &mazeWidget::on_startWatch_clicked);
    // 游戏区域面板（毛玻璃效果）
    ui->frame->setStyleSheet(
        "background-color: rgba(255, 255, 255, 0.3);"
        "border-radius: 15px;"
        "border: 1px solid rgba(0, 0, 0, 0.1);"
        "backdrop-filter: blur(5px);"  // 毛玻璃效果（需Qt 5.12+）
    );
    menuImage.load("painting/start.png"); // 替换为你的菜单图片路径
    if (menuImage.isNull()) {
        qDebug() << "Failed to load menu image";
        // 创建一个默认的菜单背景
        menuImage = QPixmap(400, 300);
        menuImage.fill(Qt::white);
        QPainter painter(&menuImage);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 20));
        painter.drawText(menuImage.rect(), Qt::AlignCenter, "迷宫游戏\n点击开始");
    }

    // 主窗口背景（深蓝黑渐变）
    //this->setStyleSheet(
    //    "background: qlineargradient(x1:0 y1:0, x2:1 y2:1, stop:0 #212529, stop:1 #343a40);"
    //    "color: #e9ecef;"
    //);
    //// 游戏面板（暗色玻璃）
    //ui->frame->setStyleSheet(
    //    "background-color: rgba(33, 37, 41, 0.7);"
    //    "border-radius: 16px;"
    //    "border: 1px solid rgba(73, 80, 87, 0.5);"
    //    "backdrop-filter: blur(10px);"
    //    "box-shadow: 0 4px 20px rgba(0,0,0,0.3);"
    //);


    //// 主窗口背景（低饱和渐变）
    //this->setStyleSheet(
    //    "background: qlineargradient(x1:0 y1:0, x2:1 y2:1, "
    //    "stop:0 #f0ebe3, stop:1 #e4dccf);"
    //    "color: #5e5343;"
    //);
    //// 游戏面板（亚麻质感）
    //ui->frame->setStyleSheet(
    //    "background-color: rgba(244, 240, 234, 0.85);"
    //    "border-radius: 16px;"
    //    "border: 1px solid rgba(174, 168, 158, 0.3);"
    //    "backdrop-filter: blur(6px);"
    //    "box-shadow: 0 3px 12px rgba(94,83,67,0.1);"
    //);

    //// 主窗口背景（霓虹蓝紫）
    //this->setStyleSheet(
    //    "background: qlineargradient(x1:0 y1:0, x2:1 y2:0, "
    //    "stop:0 #0f0c29, stop:0.5 #302b63, stop:1 #24243e);"
    //    "color: #00f2fe;"
    //);
    //// 游戏面板（发光玻璃）
    //ui->frame->setStyleSheet(
    //    "background-color: rgba(16, 20, 61, 0.6);"
    //    "border-radius: 16px;"
    //    "border: 1px solid rgba(0, 242, 254, 0.3);"
    //    "backdrop-filter: blur(12px);"
    //    "box-shadow: 0 0 15px rgba(0, 242, 254, 0.3);"
    //);

    //// 主窗口背景（紫粉渐变）
    //this->setStyleSheet(
    //    "background: qlineargradient(x1:0 y1:0, x2:1 y2:1, "
    //    "stop:0 #d8b5ff, stop:1 #1eae98);"
    //    "color: #2d0c3d;"
    //);

    //// 游戏面板（半透明珍珠白）
    //ui->frame->setStyleSheet(
    //    "background-color: rgba(255, 245, 250, 0.7);"
    //    "border-radius: 20px;"
    //    "border: 1px solid rgba(216, 181, 255, 0.4);"
    //    "backdrop-filter: blur(8px);"
    //    "box-shadow: 0 8px 32px rgba(46, 9, 70, 0.2);"
    //);

    // 解决按钮（陶土橙色调）
    ui->solve_btn->setStyleSheet(
        "QPushButton {"
        "   background: #e67e22;"  // 温暖的橙色
        "   color: white;"
        "   border: none;"
        "   padding: 8px 16px;"
        "   border-radius: 8px;"
        "   font-weight: 500;"
        "   min-width: 80px;"
        "}"
        "QPushButton:hover { background: #d35400; }"
        "QPushButton:pressed { background: #ba4a00; }"
    );

    // 其他按钮（自然中性色）
    QString naturalButtonStyle =
        "QPushButton {"
        "   background: rgba(255, 255, 255, 0.7);"
        "   color: #5d4037;"  // 深咖啡色文字
        "   border: 1px solid rgba(0, 0, 0, 0.1);"
        "   padding: 6px 12px;"
        "   border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 0.9);"
        "   border-color: rgba(0, 0, 0, 0.2);"
        "}";

    ui->start_btn->setStyleSheet(naturalButtonStyle);
    ui->end_btn->setStyleSheet(naturalButtonStyle);
}

mazeWidget::~mazeWidget() {
    delete ui;
    delete map;
    delete timer;
    delete bgmPlayer;
    delete audioOutput;
    stopAntAnimation(); // 先停止动画
    delete antColony;   // 再删除 AntColony
    delete map;         // 最后删除 maze
}
void mazeWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);     //画笔对象
    painter.setRenderHint(QPainter::Antialiasing); // 这行让图形更平滑
    if (showMenu) {
        QRect frameRect = ui->frame->geometry();
        painter.drawPixmap(frameRect, menuImage);
        return;
    }

    // 否则绘制迷宫
    if (!painting_switch) return;
    //绘图逻辑：
    QPixmap playerImage("painting/duck.png");
    if (playerImage.isNull()) {
        qDebug() << "Failed to load player image";
        playerImage = QPixmap(20, 20); // 创建空图片作为后备
        playerImage.fill(Qt::red);
    }

    QPixmap wallImage("painting/wall.png"); // 假设墙的贴图放在资源文件的images目录下
    if (wallImage.isNull()) {
        qDebug() << "Failed to load wall image";
        wallImage = QPixmap(20, 20); // 创建空图片作为后备
        wallImage.fill(Qt::gray);
    }

    QPixmap footImage("painting/Foot.png"); // 假设墙的贴图放在资源文件的images目录下
    if (footImage.isNull()) {
        qDebug() << "Failed to load foot image";
        footImage = QPixmap(20, 20); // 创建空图片作为后备
        footImage.fill(QColor(255, 183, 77));
    }

    QPixmap foodImage("painting/Food.png"); // 假设墙的贴图放在资源文件的images目录下
    if (foodImage.isNull()) {
        qDebug() << "Failed to load foot image";
        foodImage = QPixmap(20, 20); // 创建空图片作为后备
        foodImage.fill(QColor(183, 28, 28));
    }

    QPixmap roadImage("painting/grass.png"); // 假设墙的贴图放在资源文件的images目录下
    if (roadImage.isNull()) {
        qDebug() << "Failed to load foot image";
        roadImage = QPixmap(20, 20); // 创建空图片作为后备
        roadImage.fill(QColor(250, 250, 245));
    }
    QPixmap homeImage("painting/home.png"); // 假设墙的贴图放在资源文件的images目录下
    if (homeImage.isNull()) {
        qDebug() << "Failed to load foot image";
        homeImage = QPixmap(20, 20); // 创建空图片作为后备
        homeImage.fill(QColor(46, 125, 50));
    }

    QPixmap riverImage("painting/river.png"); // 河流贴图
    if (riverImage.isNull()) {
        qDebug() << "Failed to load river image";
        riverImage = QPixmap(20, 20);
        riverImage.fill(QColor(64, 164, 223)); // 蓝色表示河流
    }

    QPixmap badImage("painting/bad.png"); // 假设墙的贴图放在资源文件的images目录下
    if (badImage.isNull()) {
        qDebug() << "Failed to load foot image";
        badImage = QPixmap(20, 20); // 创建空图片作为后备
        badImage.fill(QColor(0, 0, 0));
    }

    int perblock = (std::min(ui->frame->width(), ui->frame->height()) - 20) / (map->getside());
    int start_x = ui->frame->x() + (ui->frame->width() - (ui->frame->x() + (map->getside()) * perblock)) / 2;
    int strat_y = ui->frame->y() + (ui->frame->height() - (ui->frame->y() + (map->getside()) * perblock)) / 2;
    for (int i = 0; i < map->getlevel() * 2 + 1; i++) {
        for (int j = 0; j < map->getlevel() * 2 + 1; j++) {
            QRect rect(start_x + i * perblock,
                strat_y + j * perblock,
                perblock, perblock);

            // 玩家位置（保持醒目的珊瑚色）
            if (i == map->p_x && j == map->p_y) {
                // 缩放图片以适应格子大小
                QPixmap scaled = playerImage.scaled(perblock, perblock, Qt::KeepAspectRatio);
                painter.drawPixmap(rect, scaled);
            }
            // 解决方案路径（琥珀色）
            else if (map->getmap()[i][j] == 7) {
                QPixmap scaledFoot = footImage.scaled(perblock, perblock, Qt::KeepAspectRatioByExpanding);
                painter.drawPixmap(rect, scaledFoot);
            }
            // 普通路径（乳白色）
            else if (map->getmap()[i][j] == 3 || map->getmap()[i][j] == 4) {
                QPixmap scaledroad = roadImage.scaled(perblock, perblock, Qt::KeepAspectRatio);
                painter.drawPixmap(rect, scaledroad);
            }
            // 起点（深绿色）
            else if (map->getmap()[i][j] == 5) {
                QPixmap scaledhome = homeImage.scaled(perblock, perblock, Qt::KeepAspectRatio);
                painter.drawPixmap(rect, scaledhome);
            }
            // 终点（深红色）
            else if (map->getmap()[i][j] == 6) {
                QPixmap scaledfood = foodImage.scaled(perblock, perblock, Qt::KeepAspectRatio);
                painter.drawPixmap(rect, scaledfood);
            }
            // 墙壁（浅灰色）
            else if (map->getmap()[i][j] == 8) {
                QPixmap scaledRiver = riverImage.scaled(perblock, perblock, Qt::KeepAspectRatio);
                painter.drawPixmap(rect, scaledRiver);
            }
            else if (map->getmap()[i][j] == 9) {
                QPixmap scaledbad = badImage.scaled(perblock, perblock, Qt::KeepAspectRatio);
                painter.drawPixmap(rect, scaledbad);
            }
            else {
                // 缩放墙的贴图以适应格子大小
                QPixmap scaledWall = wallImage.scaled(perblock, perblock, Qt::KeepAspectRatioByExpanding);
                painter.drawPixmap(rect, scaledWall);
            }
        }
    }
    if (antColony) {
        // 1. 先绘制所有被访问过的路径（半透明）
        QPixmap visitedTile("painting/visited.png");
        for (int i = 0; i < antColony->getVisitedMatrix().size(); i++) {
            for (int j = 0; j < antColony->getVisitedMatrix()[i].size(); j++) {
                if (antColony->getVisitedMatrix()[i][j]) {
                    QRect rect(start_x + i * perblock, strat_y + j * perblock, perblock, perblock);
                    painter.setOpacity(0.3); // 半透明效果
                    painter.drawPixmap(rect, visitedTile);
                    painter.setOpacity(1.0);
                }
            }
        }

        // 2. 只绘制活跃的蚂蚁（限制渲染数量）
        QPixmap antImage("painting/ant.png");
        int antsToDraw = std::min(50, (int)antColony->getAnts().size()); // 最多绘制50只
        for (int i = 0; i < antsToDraw; i++) {
            const auto& ant = antColony->getAnts()[i];
            if (ant.active) {
                QRect rect(start_x + ant.position.i * perblock,
                    strat_y + ant.position.j * perblock,
                    perblock, perblock);
                painter.drawPixmap(rect, antImage);
            }
        }
    }
}
void mazeWidget::keyPressEvent(QKeyEvent* event) {
    if (!keybord_switch) return;
    int x = map->p_x;
    int y = map->p_y;

    int currentCellValue = (*map)[x][y];
    //键盘移动逻辑：
    if (event->key() == Qt::Key_I || event->key() == Qt::Key_W) {
        if ((*map)[x][y - 1] == 3 || (*map)[x][y - 1] == 4 || (*map)[x][y - 1] == 5 || (*map)[x][y - 1] == 6 || (*map)[x][y - 1] == 7 || (*map)[x][y - 1] == 8|| (*map)[x][y - 1] == 9) {
            map->p_y--;
        }
    }
    else if (event->key() == Qt::Key_K || event->key() == Qt::Key_S) {
        if ((*map)[x][y + 1] == 3 || (*map)[x][y + 1] == 4 || (*map)[x][y + 1] == 5 || (*map)[x][y + 1] == 6 || (*map)[x][y + 1] == 7|| (*map)[x][y + 1] == 8|| (*map)[x][y + 1] == 9) {
            map->p_y++;
        }
    }
    else if (event->key() == Qt::Key_J || event->key() == Qt::Key_A) {
        if ((*map)[x - 1][y] == 3 || (*map)[x - 1][y] == 4 || (*map)[x - 1][y] == 5 || (*map)[x - 1][y] == 6 || (*map)[x - 1][y] == 7|| (*map)[x-1][y] == 8|| (*map)[x - 1][y] == 9) {
            map->p_x--;
        }
    }
    else if (event->key() == Qt::Key_L || event->key() == Qt::Key_D) {
        if ((*map)[x + 1][y] == 3 || (*map)[x + 1][y] == 4 || (*map)[x + 1][y] == 5 || (*map)[x + 1][y] == 6 || (*map)[x + 1][y] == 7|| (*map)[x + 1][y] == 8|| (*map)[x + 1][y] == 9) {
            map->p_x++;
        }
    }
    if ((*map)[map->p_x][map->p_y] == 8)
    {
        QMessageBox::warning(this, "警告", "小鸭子湿漉漉的，要扣分喽");
        grade -= 20;
    }
    if ((*map)[map->p_x][map->p_y] == 9)
    {
        QMessageBox::warning(this, "警告", "小鸭子受伤了，觅食时间减少喽");
        time = std::max(0, time - 10); // 确保时间不会变成负数
        ui->time_value->setText(QString::number(time));
    }
    //经过路径
    if ((*map)[map->p_x][map->p_y] != 5 && (*map)[map->p_x][map->p_y] != 6)(*map)[map->p_x][map->p_y] = 7;
    repaint();
    //到达终点
    if ((*map)[map->p_x][map->p_y] == 6) {
        map->makemap();
        repaint();
        if (if_stupid == false)
        {
            grade += pow(map->getlevel(), 2);
        }
        else if (if_stupid == true)
        {
            grade += stupid_num * pow(map->getlevel(), 2);
            if_stupid = false;
        }
        ui->grade_value->setText(QString::number(grade));
    }
}
void mazeWidget::time_update() {
    if (time != 0) {
        //计时中
        time--;
        ui->time_value->setText(QString::number(time));
        ui->progressBar->setValue(time / 2);
    }
    else {
        timer->stop();                          //停止计时器
        ui->progressBar->setVisible(false);     //隐藏进度条
        keybord_switch = false;                 //设置键盘响应、
        painting_switch = false;                //绘图响应、
        timing_switch = false;                  //计时响应为关闭状态
        repaint();                              //清除画布
        ui->start_btn->setEnabled(true);        //|
        ui->time_value->setText(" ");           //|
        ui->grade_value->setText(" ");          //|
        ui->stop_ptn->setEnabled(false);        //|设置各按钮与标签状态
        ui->end_btn->setEnabled(false);         //|
        ui->setting_btn->setEnabled(true);      //|
        QMessageBox scoreBox;
        scoreBox.setWindowTitle("游戏结束");

        // 2. 设置文字内容
        scoreBox.setText(QString("您的得分: %1").arg(grade));

        // 3. 加载图片并添加到弹窗
        QPixmap resultImage("painting/winning.png"); // 确保图片路径正确
        if (!resultImage.isNull()) {
            scoreBox.setIconPixmap(resultImage.scaled(200, 200, Qt::KeepAspectRatio));
        }
        else {
            qDebug() << "Failed to load result image";
        }

        // 4. 添加确定按钮
        scoreBox.addButton(QMessageBox::Ok);

        // 5. 显示弹窗
        scoreBox.exec();
        //分数重置
        grade = 0;
    }
}
void mazeWidget::on_start_btn_clicked() {
    showMenu = false; // 隐藏菜单
    ui->label->setVisible(false);
    ui->solve_btn->setEnabled(true);
    painting_switch = true;
    timing_switch = true;
    keybord_switch = true;
    time = 200;
    timer->start(1000);
    if (bgmPlayer->playbackState() != QMediaPlayer::PlayingState) {
        bgmPlayer->play();
    }
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(100);
    repaint();
    ui->time_value->setText(QString::number(time));
    ui->grade_value->setText(QString::number(grade));
    ui->start_btn->setEnabled(false);
    ui->stop_ptn->setEnabled(true);
    ui->end_btn->setEnabled(true);
    ui->setting_btn->setEnabled(false);
    ui->plaque_time->setText("时间");
    ui->plaque_grade->setText("分数");
}
void mazeWidget::on_stop_ptn_clicked() {
    if (stop_switch) {
        timing_switch = false;
        keybord_switch = false;
        timer->stop();
        ui->stop_ptn->setText("继续");
        stop_switch = false;
        bgmPlayer->pause();
    }
    else {
        timing_switch = true;
        keybord_switch = true;
        timer->start();
        ui->stop_ptn->setText("暂停");
        stop_switch = true;
        bgmPlayer->play();
    }
}
void mazeWidget::on_end_btn_clicked() {
    timing_switch = false;
    painting_switch = false;
    keybord_switch = false;
    stop_switch = false;
    timer->stop();
    time = 0;
    grade = 0;
    ui->plaque_time->setText("  ");
    ui->plaque_grade->setText("  ");
    ui->progressBar->setVisible(false);
    ui->grade_value->setText(" ");
    ui->time_value->setText(" ");
    ui->stop_ptn->setText("暂停");
    ui->stop_ptn->setEnabled(false);
    ui->end_btn->setEnabled(false);
    ui->start_btn->setEnabled(true);
    ui->setting_btn->setEnabled(true);
    map->rebuildmap();
    ui->label->setVisible(true);
    ui->solve_btn->setEnabled(false);
    bgmPlayer->stop();
    showMenu = true; // 游戏结束时重新显示菜单
    repaint();
}
void mazeWidget::on_rule_btn_clicked() {
    QMessageBox rule(QMessageBox::NoIcon, "规则", "计时200秒，根据迷宫等级与经过关卡记分。\n操作方式：WASD或者IJKL控制方向。\n放弃后不得分，使用提示后得分降为对应分段的20%", QMessageBox::Ok);
    rule.exec();
}
void mazeWidget::on_setting_btn_clicked() {
    QStringList difficultys;
    difficultys << tr("小朋友难度(5阶迷宫)") << tr("简单难度(10阶迷宫)") << tr("普通难度(20阶迷宫)") << tr("困难难度(40阶迷宫)");
    QString difficulty = QInputDialog::getItem(this, tr("选择难度"),
        tr("请选择一个条目"), difficultys, 0, false);
    if (difficulty == tr("小朋友难度(5阶迷宫)")) {
        delete map;
        map = new maze(5);
        map->makemap();
    }
    else if (difficulty == tr("简单难度(10阶迷宫)")) {
        delete map;
        map = new maze(10);
        map->makemap();
    }
    else if (difficulty == tr("普通难度(20阶迷宫)")) {
        delete map;
        map = new maze(20);
        map->makemap();
    }
    else if (difficulty == tr("困难难度(40阶迷宫)")) {
        delete map;
        map = new maze(40);
        map->makemap();
    }
}




void mazeWidget::on_solve_btn_clicked() {
    ui->solve_btn->setEnabled(false);
    map->aStarSolve(true);
    // 先清除之前的路径
    for (int i = 0; i < map->getside(); i++) {
        for (int j = 0; j < map->getside(); j++) {
            if (map->getmap()[i][j] == 7) {
                map->getmap()[i][j] = 3;  // 恢复为可通行区域
            }
        }
    }

    // 开始动画
    map->startSolveAnimation();

    // 使用QTimer定期重绘
    QTimer* animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, [this]() {
        this->repaint();
        });
    animationTimer->start(100);  // 每100毫秒重绘一次

    // 动画结束后恢复状态
    QTimer::singleShot(100 * (map->getside() * 2), [this, animationTimer]() {
        animationTimer->stop();
        animationTimer->deleteLater();
        map->stopSolveAnimation();
        map->makemap();
        repaint();
        ui->grade_value->setText(QString::number(grade));
        ui->solve_btn->setEnabled(true);
        });
}

void mazeWidget::on_stupid_clicked()
{   // 清除旧路径
    for (int i = 0; i < map->getside(); i++) {
        for (int j = 0; j < map->getside(); j++) {
            if (map->getmap()[i][j] == 7) {
                map->getmap()[i][j] = 3;
            }
        }
    }
    map->aStarSolve(true);
    repaint();
    if_stupid = true;
}

void mazeWidget::on_disease_clicked()
{
    obstacleMode = true;
}

// 鼠标点击事件处理
void mazeWidget::mousePressEvent(QMouseEvent* event)
{
    if (!obstacleMode || !painting_switch) {
        QWidget::mousePressEvent(event);
        return;
    }

    // 计算点击位置对应的迷宫坐标
    int perblock = (std::min(ui->frame->width(), ui->frame->height()) - 20) / (map->getside());
    int start_x = ui->frame->x() + (ui->frame->width() - (ui->frame->x() + (map->getside()) * perblock)) / 2;
    int start_y = ui->frame->y() + (ui->frame->height() - (ui->frame->y() + (map->getside()) * perblock)) / 2;

    int mazeX = (event->pos().x() - start_x) / perblock;
    int mazeY = (event->pos().y() - start_y) / perblock;

    // 检查坐标是否有效
    if (mazeX >= 0 && mazeX < map->getside() &&
        mazeY >= 0 && mazeY < map->getside())
    {
        // 检查是否是可行走区域（不是起点、终点或墙壁）
        if ((*map)[mazeX][mazeY] == 3 || (*map)[mazeX][mazeY] == 4 || (*map)[mazeX][mazeY] == 7)
        {
            (*map)[mazeX][mazeY] = 0;  // 设置为墙壁
            QMessageBox::information(this, "提示", "设置障碍成功！！！");
            repaint();
        }
    }
    obstacleMode = false;  // 一次设障后退出设障模式
    QWidget::mousePressEvent(event);
}
void mazeWidget::on_hostMode_clicked() {
    if (server == nullptr) {
        server = new QTcpServer(this);
        connect(server, &QTcpServer::newConnection, this, &mazeWidget::newConnection);

        if (server->listen(QHostAddress::Any, 12345)) {
            // 获取本地IP地址
            QString ipAddress;
            QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
            for (const QHostAddress& address : ipAddressesList) {
                if (address != QHostAddress::LocalHost && address.toIPv4Address()) {
                    ipAddress = address.toString();
                    break;
                }
            }
            if (ipAddress.isEmpty()) {
                ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
            }

            QMessageBox::information(this, "提示",
                "已开启主播模式，等待观众连接...\n你的IP地址是: " + ipAddress);
            ui->hostIP->setText("IP: " + ipAddress);
            startBroadcasting();
        }
        else {
            QMessageBox::critical(this, "错误", "无法启动服务器: " + server->errorString());
            delete server;
            server = nullptr;
        }
    }
    else {
        QMessageBox::information(this, "提示", "已经是主播模式");
    }
}

// 观众模式按钮点击
void mazeWidget::on_audienceMode_clicked() {
    if (clientSocket == nullptr) {
        clientSocket = new QTcpSocket(this);
        connect(clientSocket, &QTcpSocket::readyRead, this, &mazeWidget::readyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, [this]() {
            QMessageBox::information(this, "提示", "与主播断开连接");
            stopWatching();
            });

        QMessageBox::information(this, "提示", "已切换到观众模式，请输入主播IP开始观看");
    }
    else {
        QMessageBox::information(this, "提示", "已经是观众模式");
    }
}

// 开始观战按钮点击
void mazeWidget::on_startWatch_clicked() {
    if (clientSocket == nullptr) {
        QMessageBox::critical(this, "错误", "请先切换到观众模式");
        return;
    }

    bool ok;
    QString hostIP = QInputDialog::getText(this, "输入主播IP", "请输入主播IP地址:",
        QLineEdit::Normal, "127.0.0.1", &ok);
    if (ok && !hostIP.isEmpty()) {
        clientSocket->connectToHost(hostIP, 12345);
        if (clientSocket->waitForConnected(10000)) {
            QMessageBox::information(this, "提示", "已连接到主播");
            showMenu = false; // 游戏结束时重新显示菜单
            startWatching();
        }
        else {
            QMessageBox::critical(this, "错误", "连接失败: " + clientSocket->errorString());
            stopWatching();
        }
    }
}

// 开始直播
void mazeWidget::startBroadcasting() {
    if (serverSocket == nullptr && server != nullptr && server->hasPendingConnections()) {
        serverSocket = server->nextPendingConnection();
        connect(serverSocket, &QTcpSocket::disconnected, this, [this]() {
            QMessageBox::information(this, "提示", "观众已断开连接");
            stopBroadcasting();
            });

        // 定时发送游戏状态
        QTimer* broadcastTimer = new QTimer(this);
        connect(broadcastTimer, &QTimer::timeout, this, &mazeWidget::sendGameState);
        broadcastTimer->start(100); // 每100ms发送一次
    }
}

// 停止直播
void mazeWidget::stopBroadcasting() {
    if (serverSocket != nullptr) {
        serverSocket->disconnectFromHost();
        serverSocket->deleteLater();
        serverSocket = nullptr;
    }

    if (server != nullptr) {
        server->close();
        server->deleteLater();
        server = nullptr;
    }

    ui->hostIP->setText("IP: 未开启");
}

// 开始观看
void mazeWidget::startWatching() {
    // 禁用游戏控制按钮
    ui->start_btn->setEnabled(false);
    ui->stop_ptn->setEnabled(false);
    ui->end_btn->setEnabled(false);
    ui->solve_btn->setEnabled(false);

    // 启用绘图
    painting_switch = true;
    showMenu = false; // 游戏结束时重新显示菜单
    repaint();
}

// 停止观看
void mazeWidget::stopWatching() {
    if (clientSocket != nullptr) {
        clientSocket->disconnectFromHost();
        clientSocket->deleteLater();
        clientSocket = nullptr;
    }

    // 恢复游戏控制按钮
    ui->start_btn->setEnabled(true);
    ui->solve_btn->setEnabled(true);

    // 清除绘图
    showMenu = true; // 游戏结束时重新显示菜单
    painting_switch = false;
    repaint();
}

// 发送游戏状态
void mazeWidget::sendGameState() {
    if (serverSocket != nullptr && serverSocket->state() == QAbstractSocket::ConnectedState) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_15);

        // 发送迷宫数据
        out << map->getside();
        for (int i = 0; i < map->getside(); i++) {
            for (int j = 0; j < map->getside(); j++) {
                out << map->getmap()[i][j];
            }
        }

        // 发送玩家位置
        out << map->p_x << map->p_y;

        // 发送分数和时间
        out << grade << time;

        serverSocket->write(block);
    }
}

// 读取游戏状态
void mazeWidget::readGameState() {
    if (clientSocket != nullptr && clientSocket->bytesAvailable() > 0) {
        QDataStream in(clientSocket);
        in.setVersion(QDataStream::Qt_5_15);

        int side;
        in >> side;

        // 如果迷宫大小不匹配，重新创建
        if (map->getside() != side) {
            delete map;
            map = new maze((side - 1) / 2);
        }

        // 读取迷宫数据
        for (int i = 0; i < side; i++) {
            for (int j = 0; j < side; j++) {
                int value;
                in >> value;
                map->getmap()[i][j] = value;
            }
        }

        // 读取玩家位置
        in >> map->p_x >> map->p_y;

        // 读取分数和时间
        in >> grade >> time;

        // 更新UI
        ui->grade_value->setText(QString::number(grade));
        ui->time_value->setText(QString::number(time));

        // 重绘
        repaint();
    }
}

// 主播端有新连接
void mazeWidget::newConnection() {
    if (serverSocket == nullptr) {
        startBroadcasting();
    }
    else {
        // 已经有观众连接，拒绝新的连接
        QTcpSocket* newSocket = server->nextPendingConnection();
        newSocket->disconnectFromHost();
        newSocket->deleteLater();
        QMessageBox::information(this, "提示", "已有观众连接，拒绝新的连接");
    }
}

// 有数据可读
void mazeWidget::readyRead() {
    readGameState();
}

void mazeWidget::startAntAnimation() {
    stopAntAnimation(); // 确保先停止之前的动画

    antColony = new AntColony(map);
    antColony->start({ map->p_x, map->p_y });

    antAnimationTimer = new QTimer(this);
    connect(antAnimationTimer, &QTimer::timeout, this, &mazeWidget::updateAntAnimation);
    antAnimationTimer->start(50); // 降低刷新频率到200ms
}
void mazeWidget::stopAntAnimation() {
    if (antAnimationTimer) {
        antAnimationTimer->stop();
        delete antAnimationTimer;
        antAnimationTimer = nullptr;
    }
    if (antColony) {
        delete antColony;
        antColony = nullptr;
    }
    repaint(); // 清除残留的蚂蚁显示
}

void mazeWidget::updateAntAnimation() {
    if (!antColony) return;
    antColony->update();
    updateAntsRegion();

    if (antColony->foundPath()) {
        stopAntAnimation();
        clearAntColony();
        antColony = new AntColony(map); // 重新创建
        repaint();
        // 比较两种算法的时间
        int antTime = antColony->getElapsedTime();
        QMessageBox::information(this, "算法比较",
            QString("蚁群算法耗时: %1 ms\nA*算法耗时: %2 ms")
            .arg(antColony->getElapsedTime())  // int
            .arg(m_aStarTime)); // int
    }
}

void mazeWidget::on_antColony_clicked() {
    if (!map) return; // 防御性检查
    stopAntAnimation(); // 停止之前的动画
    antColony = new AntColony(map); // 重新创建
    startAntAnimation();
    // 1. 清除旧路径
    for (int i = 0; i < map->getside(); i++) {
        for (int j = 0; j < map->getside(); j++) {
            if (map->getmap()[i][j] == 7) {
                map->getmap()[i][j] = 3;
            }
        }
    }

    // 2. 运行A*并计时
    QElapsedTimer aStarTimer;
    aStarTimer.start();
    for (int i = 0; i < 100; i++) { // 运行100次取平均值
        map->aStarSolve(true);
    }
    m_aStarTime = aStarTimer.elapsed() / 100; // 计算平均耗时
    const int aStarTime = m_aStarTime;
    // 3. 运行蚁群算法
    if (antColony) {
        delete antColony;
        antColony = nullptr;
    }
    antColony = new AntColony(map);
    antColony->start({ map->p_x, map->p_y });

    // 4. 设置动画定时器
    if (antAnimationTimer) {
        antAnimationTimer->stop();
        delete antAnimationTimer;
    }
    antAnimationTimer = new QTimer(this);

    // 使用lambda捕获aStarTime
    connect(antAnimationTimer, &QTimer::timeout, [this, aStarTime]() {
        if (!antColony) return;

        antColony->update();
        repaint();

        if (antColony->foundPath()) {
            antAnimationTimer->stop();
            QMessageBox::information(
                this,
                "算法比较",
                QString("蚁群算法耗时: %1 ms\nA*算法耗时: %2 ms")
                .arg(antColony->getElapsedTime())
                .arg(aStarTime) // 这里使用已捕获的aStarTime
            );
        }
        });

    antAnimationTimer->start(100);
}

void mazeWidget::updateAntsRegion() {
    // 计算需要重绘的区域
    int perblock = (std::min(ui->frame->width(), ui->frame->height()) - 20) / (map->getside());
    int start_x = ui->frame->x() + (ui->frame->width() - (ui->frame->x() + (map->getside()) * perblock)) / 2;
    int start_y = ui->frame->y() + (ui->frame->height() - (ui->frame->y() + (map->getside()) * perblock)) / 2;

    // 只更新蚂蚁所在区域
    if (antColony) {
        for (const Ant& ant : antColony->getAnts()) {
            if (ant.active) {
                QRect rect(start_x + ant.position.i * perblock,
                    start_y + ant.position.j * perblock,
                    perblock, perblock);
                update(rect);
            }
        }
    }
}

void mazeWidget::clearAntColony() {
    if (antColony) {
        antColony->clearAllAnts(); // 清除所有蚂蚁
        stopAntAnimation(); // 停止动画
        repaint(); // 重绘界面
    }
}