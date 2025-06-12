// 包含自定义头文件 graana_result.h
#include "graana_result.h"
// 包含 UI 生成的头文件 ui_graana_result.h
#include "ui_graana_result.h"

// 包含 Qt 文件操作相关的头文件
#include<QFile>
// 包含 Qt 窗口部件基类头文件
#include <QWidget>
// 包含 Qt 主窗口类头文件
#include <QMainWindow>
// 包含 Qt 动画类头文件，用于播放 GIF
#include <QMovie>
// 包含 Qt 按钮类头文件
#include <QPushButton>
// 包含 Qt 字符串类头文件
#include <QString>
// 包含 Qt 输入输出设备枚举类头文件
#include <QIODevice>
// 包含 Qt 文本流类头文件
#include <QTextStream>
// 包含 Qt 错误输出函数头文件
#include <QDebug>

/**
 * @brief GraAna_result 类的构造函数
 *
 * 初始化窗口界面，设置固定大小，加载并播放 GIF 动画，
 * 同时连接返回按钮的点击信号到自定义的 back 信号。
 *
 * @param parent 父窗口指针，默认为 nullptr
 */
GraAna_result::GraAna_result(QWidget *parent)
    : QMainWindow(parent)  // 调用父类 QMainWindow 的构造函数
    , ui(new Ui::GraAna_result)  // 初始化 UI 对象
{
    ui->setupUi(this);  // 设置 UI 界面
    // 设置窗口为固定大小，大小为当前窗口几何尺寸
    this->setFixedSize(this->geometry().size());

    // 加载 GIF 文件
    piggyMovie = new QMovie(":/images/piggy.gif"); // 使用资源文件路径
    ui->label_movie->setMovie(piggyMovie);  // 将 GIF 动画设置到标签控件上
    piggyMovie->setScaledSize(this->size());  // 设置 GIF 动画的缩放尺寸为窗口大小
    // 开始播放 GIF
    piggyMovie->start();

    // 返回 switch 窗口
    // 连接退出按钮的点击信号到自定义的 back 信号
    connect(ui->quitButton, &QPushButton::clicked, [=](){
        emit this->back();  // 发射 back 信号
    });
}

/**
 * @brief 重写 QMainWindow 的 show 方法
 *
 * 在窗口显示时，依次读取 First.txt、Follow.txt 和 Select.txt 文件的内容，
 * 并将内容显示在对应的文本浏览器控件中。
 */
void GraAna_result::show(){
    QMainWindow::show();  // 调用父类的 show 方法显示窗口

    QString results;  // 用于存储从文件中读取的内容
    QFile first("TextFile/First.txt");  // 创建 QFile 对象，指向 First.txt 文件
    // 尝试以只读文本模式打开文件
    if (!first.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开文件: TextFile/First.txt";  // 输出错误信息
        return;  // 打开失败则返回
    }

    QTextStream ftool(&first);  // 创建文本流对象，用于读取文件内容
    // 逐行读取文件内容，直到文件结束
    while (!ftool.atEnd()) {
        QString tem = ftool.readLine().trimmed();  // 读取一行内容并去除首尾空格
        if (!tem.isEmpty()) {  // 如果内容不为空
            results += tem + "\n";  // 将内容添加到 results 中并换行
        }
    }
    ui->textBrowser->setPlainText(results);  // 将读取的内容显示在 textBrowser 控件中
    first.close();  // 关闭文件

    results.clear();  // 清空 results 变量

    // 输出 follow 集
    QFile follow("TextFile/Follow.txt");  // 创建 QFile 对象，指向 Follow.txt 文件
    // 尝试以只读文本模式打开文件
    if (!follow.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开文件: TextFile/Follow.txt";  // 输出错误信息
        return;  // 打开失败则返回
    }

    QTextStream fotool(&follow);  // 创建文本流对象，用于读取文件内容
    // 逐行读取文件内容，直到文件结束
    while (!fotool.atEnd()) {
        QString tem = fotool.readLine().trimmed();  // 读取一行内容并去除首尾空格
        if (!tem.isEmpty()) {  // 如果内容不为空
            results += tem + "\n";  // 将内容添加到 results 中并换行
        }
    }
    ui->textBrowser_2->setPlainText(results);  // 将读取的内容显示在 textBrowser_2 控件中
    follow.close();  // 关闭文件

    results.clear();  // 清空 results 变量

    // 输出 Select 集合
    QFile select("TextFile/SElect.txt");  // 创建 QFile 对象，指向 Select.txt 文件
    // 尝试以只读文本模式打开文件
    if (!select.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开文件: TextFile/Select.txt";  // 输出错误信息
        return;  // 打开失败则返回
    }

    QTextStream setool(&select);  // 创建文本流对象，用于读取文件内容
    // 逐行读取文件内容，直到文件结束
    while (!setool.atEnd()) {
        QString tem = setool.readLine().trimmed();  // 读取一行内容并去除首尾空格
        if (!tem.isEmpty()) {  // 如果内容不为空
            results += tem + "\n";  // 将内容添加到 results 中并换行
        }
    }
    ui->textBrowser_3->setPlainText(results);  // 将读取的内容显示在 textBrowser_3 控件中
    select.close();  // 关闭文件
}

/**
 * @brief GraAna_result 类的析构函数
 *
 * 释放 UI 对象占用的内存。
 */
GraAna_result::~GraAna_result()
{
    delete ui;  // 释放 UI 对象
}
