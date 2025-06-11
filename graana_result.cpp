#include "graana_result.h"
#include "ui_graana_result.h"

#include<QFile>

GraAna_result::GraAna_result(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraAna_result)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().size());

    // 加载GIF文件
    piggyMovie = new QMovie(":/images/piggy.gif"); // 使用资源文件路径
    ui->label_movie->setMovie(piggyMovie);
    piggyMovie->setScaledSize(this->size());
    //开始播放GIF
    piggyMovie->start();

    //返回switch窗口
    connect(ui->quitButton,&QPushButton::clicked,[=](){
        emit this->back();
    });

}


void GraAna_result::show(){
    QMainWindow::show();

    QString results;
    QFile first("TextFile/First.txt");
    if (!first.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开文件: TextFile/First.txt";
        return;
    }

    QTextStream ftool(&first);
    while (!ftool.atEnd()) {
        QString tem = ftool.readLine().trimmed();
        if (!tem.isEmpty()) {
            results += tem+"\n";
        }
    }
    ui->textBrowser->setPlainText(results);
    first.close();



    results.clear();
    //输出follow集
    QFile follow("TextFile/Follow.txt");
    if (!follow.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开文件: TextFile/Follow.txt";
        return;
    }

    QTextStream fotool(&follow);
    while (!fotool.atEnd()) {
        QString tem = fotool.readLine().trimmed();
        if (!tem.isEmpty()) {
            results += tem+"\n";
        }
    }
    ui->textBrowser_2->setPlainText(results);
    follow.close();


    results.clear();
    //输出Select集合
    QFile select("TextFile/SElect.txt");
    if (!select.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开文件: TextFile/Select.txt";
        return;
    }

    QTextStream setool(&select);
    while (!setool.atEnd()) {
        QString tem = setool.readLine().trimmed();
        if (!tem.isEmpty()) {
            results += tem+"\n";
        }
    }
    ui->textBrowser_3->setPlainText(results);
    select.close();


}

GraAna_result::~GraAna_result()
{
    delete ui;
}
