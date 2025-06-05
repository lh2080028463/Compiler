#ifndef QUATSHOW_H
#define QUATSHOW_H

#include <QMainWindow>

namespace Ui {
class quatshow;
}

class quatshow : public QMainWindow
{
    Q_OBJECT

public:
    explicit quatshow(QWidget *parent = nullptr);
    ~quatshow();

private:
    Ui::quatshow *ui;
};

#endif // QUATSHOW_H
