#include "symtable.h"
#include "ui_symtable.h"

symtable::symtable(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::symtable)
{
    ui->setupUi(this);
}

symtable::~symtable()
{
    delete ui;
}
