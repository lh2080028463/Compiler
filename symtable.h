#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <QMainWindow>

namespace Ui {
class symtable;
}

class symtable : public QMainWindow
{
    Q_OBJECT

public:
    explicit symtable(QWidget *parent = nullptr);
    ~symtable();

private:
    Ui::symtable *ui;
};

#endif // SYMTABLE_H
