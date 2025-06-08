#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFontDatabase>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMovie>
#include<QFile>
#include<QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 加载GIF文件
    piggyMovie = new QMovie(":/images/piggy.gif"); // 使用资源文件路径
    ui->label_3->setMovie(piggyMovie);
    piggyMovie->setScaledSize(this->size());
    //开始播放GIF
    piggyMovie->start();



}

MainWindow::~MainWindow()
{
    delete ui;
}

// void MainWindow::resizeEvent(QResizeEvent *event)
// {
//     QMainWindow::resizeEvent(event);
//     // 调整视图大小
//     view->setGeometry(0, 0, this->width(), this->height());
// }
