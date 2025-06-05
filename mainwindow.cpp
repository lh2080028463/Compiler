#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFontDatabase>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMovie>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建图形视图和场景
    view = new QGraphicsView(this);  // 使用成员变量
    QGraphicsScene *scene = new QGraphicsScene(this);
    view->setScene(scene);

    // 设置视图填充整个窗口
    view->setGeometry(0, 0, this->width(), this->height());
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 创建GIF动画项
    QMovie *movie = new QMovie(":/images/piggy.gif");
    QGraphicsPixmapItem *animItem = new QGraphicsPixmapItem();

    connect(movie, &QMovie::frameChanged, [=]() {
        animItem->setPixmap(movie->currentPixmap());
    });

    if(movie->isValid()) {
        movie->start();
        scene->addItem(animItem);
    } else {
        qDebug() << "无法加载GIF动画：" << ":/images/piggy.gif";
    }

    // 设置窗口透明属性
    this->setAttribute(Qt::WA_TranslucentBackground);
    view->setStyleSheet("background: transparent;");

    // 确保视图在底层
    view->lower();

    // 创建其他控件（示例按钮）
    QPushButton *settingsBtn = new QPushButton("设置", this);
    settingsBtn->setIcon(QIcon(":/images/settings.png"));
    settingsBtn->setIconSize(QSize(32, 32));
    settingsBtn->move(10, 10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    // 调整视图大小
    view->setGeometry(0, 0, this->width(), this->height());
}
