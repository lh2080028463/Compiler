#include "graana_result.h"
#include "ui_graana_result.h"

#include<QFile>

GraAna_result::GraAna_result(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraAna_result)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().size());

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

    QTextStream tool(&first);
    while (!tool.atEnd()) {
        QString tem = tool.readLine().trimmed();
        if (!tem.isEmpty()) {
            results += tem+"\n";
        }
    }
    ui->textBrowser->setPlainText(results);
    first.close();


    //输出follow集
    QFile follow("TextFile/Follow.txt");
    if (!first.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开文件: TextFile/Follow.txt";
        return;
    }

    QTextStream tool1(&first);
    while (!tool1.atEnd()) {
        QString tem = tool1.readLine().trimmed();
        if (!tem.isEmpty()) {
            results += tem+"\n";
        }
    }
    ui->textBrowser_2->setPlainText(results);

}

GraAna_result::~GraAna_result()
{
    delete ui;
}
