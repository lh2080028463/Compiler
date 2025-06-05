#ifndef QUAT_RESULT_H
#define QUAT_RESULT_H

#include <QMainWindow>

namespace Ui {
class Quat_result;
}

class Quat_result : public QMainWindow
{
    Q_OBJECT

public:
    explicit Quat_result(QWidget *parent = nullptr);
    ~Quat_result();

private:
    Ui::Quat_result *ui;
};

#endif // QUAT_RESULT_H
