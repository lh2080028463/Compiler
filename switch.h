#ifndef SWITCH_H
#define SWITCH_H

#include <QMainWindow>

namespace Ui {
class Switch;
}

class Switch : public QMainWindow
{
    Q_OBJECT

public:
    explicit Switch(QWidget *parent = nullptr);
    ~Switch();

private:
    Ui::Switch *ui;
};

#endif // SWITCH_H
