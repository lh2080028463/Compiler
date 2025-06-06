#include "graana_result.h"
#include "ui_graana_result.h"

GraAna_result::GraAna_result(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraAna_result)
{
    ui->setupUi(this);
}

GraAna_result::~GraAna_result()
{
    delete ui;
}
