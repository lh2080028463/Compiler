#ifndef QUAT_RESULT_H
#define QUAT_RESULT_H

#include <QMainWindow>
#include"createquat.h"
#include"optimize.h"

namespace Ui {
class Quat_result;
}

class Quat_result : public QMainWindow,public createquat,public optimize
{
    Q_OBJECT

public:
    explicit Quat_result(QWidget *parent = nullptr);
    ~Quat_result();
    void show();
signals:
    void back();

private:
    Ui::Quat_result *ui;
    //optimize opti;
};

#endif // QUAT_RESULT_H
