#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGraphicsView>
#include <QMainWindow>
#include <QLabel>       // 添加这行
#include <QMovie>       // 如果需要使用GIF动画
#include"switch.h"

#include"synana.h"

#include"optimize.h"
#include"objectcode.h"
#include <QtWidgets/QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow,public WordAna
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Switch* window2=NULL;//选择界面实例化对象
    bool nextflag=false;

private:
    Ui::MainWindow *ui;
    QMovie *piggyMovie;
    QLabel *piggyLabel;
};
#endif // MAINWINDOW_H
