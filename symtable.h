#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <QMainWindow>
#include<QTableWidgetItem>
#include<QLabel>
#include<QMovie>

namespace Ui {
class symtable;
}

class symtable : public QMainWindow
{
    Q_OBJECT

public:
    explicit symtable(QWidget *parent = nullptr);
    ~symtable();

    void setSYN();
    void setTYP();
    void setPFI();
    void setVAL();

signals:
    void back();

private:
    Ui::symtable *ui;
    QMovie *piggyMovie;
    QLabel *piggyLabel;
};

#endif // SYMTABLE_H
