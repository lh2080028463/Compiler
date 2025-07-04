#ifndef SWITCH_H
#define SWITCH_H

#include <QMainWindow>
#include<QTableWidgetItem>
#include"quat_result.h"
#include"graana_result.h"
#include"symtable.h"
#include<QMovie>
#include<QLabel>
#include<QMessageBox>

namespace Ui {
class Switch;
}

class Switch : public QMainWindow
{
    Q_OBJECT

public:
    explicit Switch(QWidget *parent = nullptr);
    ~Switch();

    Quat_result* window3=NULL;//选择界面实例化对象
    GraAna_result* window4=NULL;//选择界面实例化对象
    symtable* window5=NULL;//选择界面实例化对象

signals:
    void back();

private:
    Ui::Switch *ui;
    QMovie *piggyMovie;
    QLabel *piggyLabel;
};

#endif // SWITCH_H
