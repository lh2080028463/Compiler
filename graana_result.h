#ifndef GRAANA_RESULT_H
#define GRAANA_RESULT_H

#include <QMainWindow>
#include<QLabel>
#include<QMovie>

namespace Ui {
class GraAna_result;
}

class GraAna_result : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraAna_result(QWidget *parent = nullptr);
    ~GraAna_result();
    void show();

signals:
    void back();

private:
    Ui::GraAna_result *ui;
    QMovie *piggyMovie;
    QLabel *piggyLabel;
};

#endif // GRAANA_RESULT_H
