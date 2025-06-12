// 包含自定义头文件 symtable.h，其中声明了 symtable 类等信息
#include "symtable.h"
// 包含自动生成的 UI 头文件，定义了界面元素的布局和属性
#include "ui_symtable.h"

/**
 * @brief symtable 类的构造函数
 *
 * 初始化主窗口界面，设置固定窗口大小，加载并播放 GIF 动画，
 * 同时连接返回按钮的点击信号到自定义的 back 信号。
 *
 * @param parent 父窗口指针，默认为 nullptr
 */
symtable::symtable(QWidget *parent)
    : QMainWindow(parent)  // 调用父类 QMainWindow 的构造函数
    , ui(new Ui::symtable)  // 初始化 UI 对象
{
    ui->setupUi(this);  // 设置 UI 界面
    // 设置窗口为固定大小，大小为当前窗口几何尺寸
    this->setFixedSize(this->geometry().size());

    // 加载 GIF 文件，使用资源文件路径
    piggyMovie = new QMovie(":/images/piggy.gif");
    // 将 GIF 动画设置到名为 label_3 的标签控件上
    ui->label_3->setMovie(piggyMovie);
    // 设置 GIF 动画的缩放尺寸为窗口大小
    piggyMovie->setScaledSize(this->size());
    // 开始播放 GIF 动画
    piggyMovie->start();

    // 连接退出按钮的点击信号到自定义的 back 信号
    connect(ui->quitButton, &QPushButton::clicked, [=](){
        // 发射 back 信号，用于触发返回操作
        emit this->back();
    });
}

/**
 * @brief 填充 SYNBL 表格
 *
 * 从外部变量 synbl_out 中获取数据，设置 SYNBL 表格的行数，
 * 禁止表格编辑，隐藏垂直表头，将数据逐行填充到表格中。
 */
void symtable::setSYN(){
    // 声明外部变量 synbl_out，该变量在其他文件中定义
    extern QVector<QString>synbl_out;
    // 设置 SYNBL 表格的行数，每 4 个元素占一行
    ui->SYNBL->setRowCount(synbl_out.size()/4);
    // 设置表格不可编辑
    ui->SYNBL->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 隐藏表格的垂直表头
    ui->SYNBL->verticalHeader()->hide();
    // 遍历 synbl_out 数组，每 4 个元素为一组
    for(int i=0;i<synbl_out.size();i+=4){
        // 计算当前行号
        int row=i/4;
        // 创建表格项，显示行号
        QTableWidgetItem*temp1=new QTableWidgetItem(QString::number(row));
        // 创建表格项，显示 synbl_out 中的第 i 个元素
        QTableWidgetItem*temp2=new QTableWidgetItem(synbl_out[i]);
        // 创建表格项，显示 synbl_out 中的第 i+1 个元素
        QTableWidgetItem*temp3=new QTableWidgetItem(synbl_out[i+1]);
        // 创建表格项，显示 synbl_out 中的第 i+2 个元素
        QTableWidgetItem*temp4=new QTableWidgetItem(synbl_out[i+2]);
        // 创建表格项，显示 synbl_out 中的第 i+3 个元素
        QTableWidgetItem*temp5=new QTableWidgetItem(synbl_out[i+3]);
        // 将表格项设置到第 row 行第 0 列
        ui->SYNBL->setItem(row,0,temp1);
        // 将表格项设置到第 row 行第 1 列
        ui->SYNBL->setItem(row,1,temp2);
        // 将表格项设置到第 row 行第 2 列
        ui->SYNBL->setItem(row,2,temp3);
        // 将表格项设置到第 row 行第 3 列
        ui->SYNBL->setItem(row,3,temp4);
        // 将表格项设置到第 row 行第 4 列
        ui->SYNBL->setItem(row,4,temp5);
    }
}

/**
 * @brief 填充 TAPEL 表格
 *
 * 从外部变量 typel_out 中获取数据，设置 TAPEL 表格的行数，
 * 禁止表格编辑，隐藏垂直表头，将数据逐行填充到表格中。
 */
void symtable::setTYP(){
    // 声明外部变量 typel_out，该变量在其他文件中定义
    extern QVector<QString>typel_out;
    // 设置 TAPEL 表格的行数，每 2 个元素占一行
    ui->TAPEL->setRowCount(typel_out.size()/2);
    // 设置表格不可编辑
    ui->TAPEL->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 隐藏表格的垂直表头
    ui->TAPEL->verticalHeader()->hide();
    // 遍历 typel_out 数组，每 2 个元素为一组
    for(int i=0;i<typel_out.size();i+=2){
        // 计算当前行号
        int row=i/2;
        // 创建表格项，显示行号
        QTableWidgetItem*temp1=new QTableWidgetItem(QString::number(row));
        // 创建表格项，显示 typel_out 中的第 i 个元素
        QTableWidgetItem*temp2=new QTableWidgetItem(typel_out[i]);
        // 创建表格项，显示 typel_out 中的第 i+1 个元素
        QTableWidgetItem*temp3=new QTableWidgetItem(typel_out[i+1]);
        // 将表格项设置到第 row 行第 0 列
        ui->TAPEL->setItem(row,0,temp1);
        // 将表格项设置到第 row 行第 1 列
        ui->TAPEL->setItem(row,1,temp2);
        // 将表格项设置到第 row 行第 2 列
        ui->TAPEL->setItem(row,2,temp3);
    }
}

/**
 * @brief 填充 PFINFL 表格
 *
 * 从外部变量 pfinfl_out 中获取数据，设置 PFINFL 表格的行数，
 * 禁止表格编辑，隐藏垂直表头，将数据逐行填充到表格中。
 */
void symtable::setPFI(){
    // 声明外部变量 pfinfl_out，该变量在其他文件中定义
    extern QVector<QString>pfinfl_out;
    // 设置 PFINFL 表格的行数，每 4 个元素占一行
    ui->PFINFL->setRowCount(pfinfl_out.size()/4);
    // 设置表格不可编辑
    ui->PFINFL->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 隐藏表格的垂直表头
    ui->PFINFL->verticalHeader()->hide();
    // 遍历 pfinfl_out 数组，每 4 个元素为一组
    for(int i=0;i<pfinfl_out.size();i+=4){
        // 计算当前行号
        int row=i/4;
        // 创建表格项，显示行号
        QTableWidgetItem*temp1=new QTableWidgetItem(QString::number(row));
        // 创建表格项，显示 pfinfl_out 中的第 i 个元素
        QTableWidgetItem*temp2=new QTableWidgetItem(pfinfl_out[i]);
        // 创建表格项，显示 pfinfl_out 中的第 i+1 个元素
        QTableWidgetItem*temp3=new QTableWidgetItem(pfinfl_out[i+1]);
        // 创建表格项，显示 pfinfl_out 中的第 i+2 个元素
        QTableWidgetItem*temp4=new QTableWidgetItem(pfinfl_out[i+2]);
        // 创建表格项，显示 pfinfl_out 中的第 i+3 个元素
        QTableWidgetItem*temp5=new QTableWidgetItem(pfinfl_out[i+3]);
        // 将表格项设置到第 row 行第 0 列
        ui->PFINFL->setItem(row,0,temp1);
        // 将表格项设置到第 row 行第 1 列
        ui->PFINFL->setItem(row,1,temp2);
        // 将表格项设置到第 row 行第 2 列
        ui->PFINFL->setItem(row,2,temp3);
        // 将表格项设置到第 row 行第 3 列
        ui->PFINFL->setItem(row,3,temp4);
        // 将表格项设置到第 row 行第 4 列
        ui->PFINFL->setItem(row,4,temp5);
    }
}

/**
 * @brief 填充 VALL 表格
 *
 * 从外部变量 vall_out 中获取数据，设置 VALL 表格的行数，
 * 禁止表格编辑，隐藏垂直表头，将数据逐行填充到表格中，
 * 行号按逆序显示。
 */
void symtable::setVAL(){
    // 声明外部变量 vall_out，该变量在其他文件中定义
    extern QVector<QString>vall_out;
    // 获取 vall_out 数组的大小
    int SIZE=vall_out.size();
    // 设置 VALL 表格的行数为 vall_out 数组的大小
    ui->VALL->setRowCount(SIZE);
    // 设置表格不可编辑
    ui->VALL->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 隐藏表格的垂直表头
    ui->VALL->verticalHeader()->hide();
    // 遍历 vall_out 数组
    for(int i=0;i<vall_out.size();i++){
        // 创建表格项，显示逆序行号
        QTableWidgetItem*temp1=new QTableWidgetItem(QString::number(SIZE-1-i));
        // 创建表格项，显示 vall_out 中的第 i 个元素
        QTableWidgetItem*temp2=new QTableWidgetItem(vall_out[i]);
        // 将表格项设置到第 i 行第 0 列
        ui->VALL->setItem(i,0,temp1);
        // 将表格项设置到第 i 行第 1 列
        ui->VALL->setItem(i,1,temp2);
    }
}

/**
 * @brief symtable 类的析构函数
 *
 * 释放 UI 对象占用的内存，防止内存泄漏。
 */
symtable::~symtable()
{
    // 释放 ui 对象的内存
    delete ui;
}
