#ifndef GRAANA_RESULT_H
#define GRAANA_RESULT_H

#include <QMainWindow>


namespace Ui {
class GraAna_result;
}

class GraAna_result : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraAna_result(QWidget *parent = nullptr);
    ~GraAna_result();

signals:
    void back();

private:
    Ui::GraAna_result *ui;
};

#endif // GRAANA_RESULT_H
