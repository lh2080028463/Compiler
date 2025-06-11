#include "mainwindow.h"

#include <QApplication>

QString input_s;//输入的待编译程序
QVector<QString>synbl_out;
QVector<QString>typel_out;
QVector<QString>pfinfl_out;
QVector<QString>vall_out;
QString err_inf;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
