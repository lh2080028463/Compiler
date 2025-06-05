#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGraphicsView>
#include <QMainWindow>
#include <QLabel>       // 添加这行
#include <QMovie>       // 如果需要使用GIF动画
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QGraphicsView *view;  // 如果使用QGraphicsView方法需要声明这个成员变量
    void resizeEvent(QResizeEvent *event) override;  // 添加这行声明

private:
    Ui::MainWindow *ui;
    QMovie *piggyMovie;
    QLabel *piggyLabel;
};
#endif // MAINWINDOW_H
