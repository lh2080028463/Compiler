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

    this->window2=new Switch;//实例化页面二

    extern QString input_s;
    connect(ui->inputButton,&QPushButton::clicked,[=](){//文本框输入
        int flag=ui->comboBox->currentIndex();//0为直接输入，1为文件输入
        if(flag){
            QString runPath = QCoreApplication::applicationDirPath();//获取项目的根路径
            QString file_name = QFileDialog::getOpenFileName(this,QStringLiteral("选择文件"),runPath,"Text Files(*.txt)",nullptr,QFileDialog::DontResolveSymlinks);
            ui->textEdit->clear();
            if(!file_name.isEmpty()){
                QFile file(file_name);
                bool isok = file.open(QIODevice::ReadOnly);
                if(isok){
                    ui->textEdit->setPlainText(file.readAll());
                }
                file.close();
            }
        }
        input_s = ui->textEdit->toPlainText();
        input_s+="#";
    });
    connect(ui->nextButton,&QPushButton::clicked,[=](){//下一步
        if(input_s.length()){
            this->hide();
            this->window2->show();
        }
    });

    connect(this->window2,&Switch::back,[=](){//从窗口2返回
        this->window2->hide();
        this->show();
    });

    connect(ui->quitButton,&QPushButton::clicked,[=](){//退出
        exit(0);
    });

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
