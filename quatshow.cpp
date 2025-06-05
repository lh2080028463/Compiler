#include "quatshow.h"
#include "ui_quatshow.h"

quatshow::quatshow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::quatshow)
{
    ui->setupUi(this);
}

quatshow::~quatshow()
{
    delete ui;
}
