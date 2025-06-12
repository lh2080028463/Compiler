#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFontDatabase>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMovie>
#include<QFile>
#include<QFileDialog>

extern QVector <QString> Tokens;
extern QString input_s;
const QVector<QChar> other = {
    '-','/','(',')','=','<','+','*','>','=',',',';',
    '{','}','\'','\"','!','\n',' ','#'
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().size());

    // 加载GIF文件
    piggyMovie = new QMovie(":/images/piggy.gif"); // 使用资源文件路径
    ui->label_3->setMovie(piggyMovie);
    piggyMovie->setScaledSize(this->size());
    //开始播放GIF
    piggyMovie->start();

    this->window2=new Switch;//实例化页面二


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

        int LENGTH=input_s.length();
        nextflag=1;
        for(int i=0;i<LENGTH;i++){
            if((input_s[i]>='a'&&input_s<='z')||(input_s[i]>='A'&&input_s[i]<='Z'))continue;
            if(input_s[i]>='0'&&input_s[i]<='9')continue;
            int failflag=1;
            for(int j=0;j<other.size();j++){
                if(input_s[i]==other[j]){
                    failflag=0;
                    break;
                }
            }
            if(failflag){
                nextflag=0;
                break;
            }
        }
        if(nextflag==0){//含非法字符
            QMessageBox::critical(
                this,
                "错误信息",
                "输入非法字符"
                );
        }
        else{
            SynAna s;

            //初始化符号表及错误信息
            extern QVector<QString>synbl_out;
            extern QVector<QString>typel_out;
            extern QVector<QString>pfinfl_out;
            extern QVector<QString>vall_out;
            extern QString err_inf;
            synbl_out.clear();
            typel_out.clear();
            pfinfl_out.clear();
            vall_out.clear();
            err_inf.clear();

            nextflag=s.AN(input_s);
            if (nextflag)
            {
                Quat_result op;
                op.opti.run();
                ObjectCode objCode;
                objCode.clear();
                objCode.scan();
            }
            else
            {   // 弹出错误信息对话框
                QMessageBox::critical(
                    this,
                    "错误信息",
                    err_inf
                    );
            }
            print(Tokens,ui->textBrowser);
        }
    });
    connect(ui->nextButton,&QPushButton::clicked,[=](){//下一步
        if(!input_s.length()){
            QMessageBox::critical(
                this,
                "错误信息",
                "请输入字符串"
                );
        }
        else if(!nextflag){
            extern QString err_inf;
            QMessageBox::critical(
                this,
                "错误信息",
                err_inf
                );
        }
        else{//跳转至选择窗口
            this->hide();
            this->window2->show();
        }
    });

    connect(this->window2,&Switch::back,[=](){//从选择窗口返回
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
