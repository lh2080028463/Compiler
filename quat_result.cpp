#include "quat_result.h"
#include "ui_quat_result.h"

extern quat qt[100];//四元式区
extern QVector <QString> Object;


Quat_result::Quat_result(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Quat_result)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().size());

    //返回switch窗口
    connect(ui->quitButton,&QPushButton::clicked,[=](){
        emit this->back();
    });




}

void Quat_result::show(){
    QMainWindow::show();


    //优化前四元式输出
    int index=1;
    QString results;
    for(auto ele:qt){
        if(ele.getStr()!="(,,,)")
        results+="("+QString::number(index++)+") "+ele.getStr()+"\n";
    }
    ui->textBrowser->setPlainText(results);

    results.clear();
    //优化后四元式输出
    for(auto ele:Block){
        if(!ele.empty())
        for(auto q:ele){
            if(q.getStr()!="(,,,)")
                results+="("+QString::number(index++)+") "+q.getStr()+"\n";
        }
    }
    ui->textBrowser_2->setPlainText(results);


    results.clear();
    //目标代码输出
    for (auto line : Object) {
        results+=line;
    }
    ui->textBrowser_3->setPlainText(results);
}


Quat_result::~Quat_result()
{
    delete ui;
}
