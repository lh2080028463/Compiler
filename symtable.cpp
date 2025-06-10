#include "symtable.h"
#include "ui_symtable.h"

symtable::symtable(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::symtable)
{
    ui->setupUi(this);

    //返回switch窗口
    connect(ui->quitButton,&QPushButton::clicked,[=](){
        emit this->back();
    });
}

void symtable::setSYN(){
    extern QVector<QString>synbl_out;
    ui->SYNBL->setRowCount(synbl_out.size()/4);
    for(int i=0;i<synbl_out.size();i+=4){
        int row=i/4;
        QTableWidgetItem*temp1=new QTableWidgetItem(QString::number(row));
        QTableWidgetItem*temp2=new QTableWidgetItem(synbl_out[i]);
        QTableWidgetItem*temp3=new QTableWidgetItem(synbl_out[i+1]);
        QTableWidgetItem*temp4=new QTableWidgetItem(synbl_out[i+2]);
        QTableWidgetItem*temp5=new QTableWidgetItem(synbl_out[i+3]);
        ui->SYNBL->setItem(row,0,temp1);
        ui->SYNBL->setItem(row,1,temp2);
        ui->SYNBL->setItem(row,2,temp3);
        ui->SYNBL->setItem(row,3,temp4);
        ui->SYNBL->setItem(row,4,temp5);
    }
};

void symtable::setTYP(){
    extern QVector<QString>typel_out;
    ui->TAPEL->setRowCount(typel_out.size()/2);
    for(int i=0;i<typel_out.size();i+=2){
        int row=i/2;
        QTableWidgetItem*temp1=new QTableWidgetItem(QString::number(row));
        QTableWidgetItem*temp2=new QTableWidgetItem(typel_out[i]);
        QTableWidgetItem*temp3=new QTableWidgetItem(typel_out[i+1]);
        ui->TAPEL->setItem(row,0,temp1);
        ui->TAPEL->setItem(row,1,temp2);
        ui->TAPEL->setItem(row,2,temp3);
    }
}

symtable::~symtable()
{
    delete ui;
}
