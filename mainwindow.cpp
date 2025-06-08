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
