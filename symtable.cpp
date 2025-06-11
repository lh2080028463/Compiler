#include "symtable.h"
#include "ui_symtable.h"

symtable::symtable(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::symtable)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().size());

    // 加载GIF文件
    piggyMovie = new QMovie(":/images/piggy.gif"); // 使用资源文件路径
    ui->label_3->setMovie(piggyMovie);
    piggyMovie->setScaledSize(this->size());
    //开始播放GIF
    piggyMovie->start();

    //返回switch窗口
    connect(ui->quitButton,&QPushButton::clicked,[=](){
        emit this->back();
    });
}

void symtable::setSYN(){
    extern QVector<QString>synbl_out;
    ui->SYNBL->setRowCount(synbl_out.size()/4);
    ui->SYNBL->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->SYNBL->verticalHeader()->hide();
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
}

void symtable::setTYP(){
    extern QVector<QString>typel_out;
    ui->TAPEL->setRowCount(typel_out.size()/2);
    ui->TAPEL->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TAPEL->verticalHeader()->hide();
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

void symtable::setPFI(){
    extern QVector<QString>pfinfl_out;
    ui->PFINFL->setRowCount(pfinfl_out.size()/4);
    ui->PFINFL->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->PFINFL->verticalHeader()->hide();
    for(int i=0;i<pfinfl_out.size();i+=4){
        int row=i/4;
        QTableWidgetItem*temp1=new QTableWidgetItem(QString::number(row));
        QTableWidgetItem*temp2=new QTableWidgetItem(pfinfl_out[i]);
        QTableWidgetItem*temp3=new QTableWidgetItem(pfinfl_out[i+1]);
        QTableWidgetItem*temp4=new QTableWidgetItem(pfinfl_out[i+2]);
        QTableWidgetItem*temp5=new QTableWidgetItem(pfinfl_out[i+3]);
        ui->PFINFL->setItem(row,0,temp1);
        ui->PFINFL->setItem(row,1,temp2);
        ui->PFINFL->setItem(row,2,temp3);
        ui->PFINFL->setItem(row,3,temp4);
        ui->PFINFL->setItem(row,4,temp5);
    }
}

void symtable::setVAL(){
    extern QVector<QString>vall_out;
    int SIZE=vall_out.size();
    ui->VALL->setRowCount(SIZE);
    ui->VALL->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->VALL->verticalHeader()->hide();
    for(int i=0;i<vall_out.size();i++){
        QTableWidgetItem*temp1=new QTableWidgetItem(QString::number(SIZE-1-i));
        QTableWidgetItem*temp2=new QTableWidgetItem(vall_out[i]);
        ui->VALL->setItem(i,0,temp1);
        ui->VALL->setItem(i,1,temp2);
    }
}

symtable::~symtable()
{
    delete ui;
}
