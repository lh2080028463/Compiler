#include "switch.h"
#include "ui_switch.h"

extern quat qt[100];

Switch::Switch(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Switch)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().size());

    this->window3=new Quat_result;//显示四元式的窗口
    this->window4=new GraAna_result;//显示语法分析的窗口
    this->window5=new symtable;//显示符号表的窗口

    //返回主窗口
    connect(ui->returnButton,&QPushButton::clicked,[=](){
        emit this->back();
    });

    //符号表窗口
    connect(ui->symButton,&QPushButton::clicked,[=](){//进入

        this->hide();
        this->window5->show();
        this->window5->setSYN();
        this->window5->setTYP();
        this->window5->setPFI();
        this->window5->setVAL();
    });
    connect(this->window5,&symtable::back,[=](){//返回
        this->window5->hide();
        this->show();
    });

    //四元式窗口
    connect(ui->quatButton,&QPushButton::clicked,[=](){//进入
        this->hide();
        this->window3->show();

        QString quats;
        int index=1;
        for(auto ele:qt){
            quats += "("+QString::number(index++)+")"+ele.getStr()+"\n";
        }
        //window3->ui->textBrowser->setPlainText(quats);

    });
    connect(this->window3,&Quat_result::back,[=](){//返回
        this->window3->hide();
        this->show();
    });

    //语法分析窗口
    connect(ui->graButton,&QPushButton::clicked,[=](){//进入
        this->hide();
        this->window4->show();
    });
    connect(this->window4,&GraAna_result::back,[=](){//返回
        this->window4->hide();
        this->show();
    });
}

Switch::~Switch()
{
    delete ui;
}
