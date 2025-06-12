#include "objectcode.h"



#include "ObjectCode.h"
#include <QString>
#include <QVector>
#include <QMap>
#include <QStack>
#include<QDebug>
#include<QFile>

// 使用标准库命名空间，可能会导致命名冲突，建议尽量避免
using namespace std;

// 定义语义栈，用于存储中间结果或跳转地址等信息
QStack <int> SEM;	//栈
// 声明外部变量 I，该变量在其他文件中定义
extern QVector<QString> I;    //声明，不是定义
// 待处理的四元式序列，存储中间代码
QVector <QString> Quadruple;
// 存储翻译后的目标代码语句
QVector <QString> Object;
// 存储变量及其活跃信息，'y' 表示活跃，'n' 表示不活跃
QMap <QString, QChar> Var;
// 操作符映射表，存储操作符对应的相关信息，如操作类型、指令名称、是否可交换等
QMap <QString, QVector<QString>> Operate = { {"+",{"w","add","y"}},{"-",{"w","sub","n"}},{"*",{"w","mul","y"}},
                                       {"/",{"w","div","n"}},{">",{"w","gt","n"}},{"<",{"w","lt","n"}},
                                       {"==",{"w","eq","y"}},{"!=",{"w","ne","y"}},
                                       {">=",{"w","ge","n"}} ,{"<=",{"w","le","n"}} };
// 初始化 ObjectCode 类的静态成员变量 count，用于记录目标代码的行号
int ObjectCode::count = 1;
// 初始化 RDL 类的静态成员变量 R，用于表示寄存器当前存储的变量
QString RDL::R = "0";
// 初始化 RDL 类的静态成员变量 active，用于表示寄存器中变量的活跃信息
QString RDL::active = "0";


ObjectCode::ObjectCode() {}

/**
 * @brief 初始化相关数据结构和变量
 *
 * 清空语义栈、四元式序列、目标代码序列和变量活跃信息表，
 * 并重置计数器和寄存器相关信息。
 */
void ObjectCode::clear()    //初始化
{
    while (!SEM.empty()) SEM.pop();  //清空语义栈
    Quadruple.clear();  //清空四元式
    Object.clear();
    Var.clear();
    ObjectCode::count = 1;
    RDL::R = "0";
    RDL::active = "0";
}

/**
 * @brief 检查指定元素是否存在于向量中
 *
 * 遍历向量，检查是否存在与指定元素相等的元素。
 *
 * @tparam T 元素的类型
 * @param v 要检查的向量
 * @param t 要查找的元素
 * @return 如果存在返回 true，否则返回 false
 */
template<typename T>bool ObjectCode::existT(QVector<T> v, T t)
{
    for (int i = 0; i < (int)v.size(); i++)
    {
        if (t == v[i]) return true;
    }
    return false;
}

/**
 * @brief 从四元式中提取变量并设置初始活跃信息
 *
 * 遍历四元式序列，提取操作数和结果变量，
 * 标识符变量初始化为活跃状态，临时变量初始化为不活跃状态。
 */
void ObjectCode::getVar() //把变量从四元式中提取出来，设置初始的活跃信息，标识符为活跃，临时变量是不活跃的
{
    QString temp;
    for (auto it = Quadruple.begin(); it != Quadruple.end(); ++it)
    {
        temp = choose(*it, 1, 0);
        if (existT<QString>(I, temp) && !Var.contains(temp))
            Var.insert(temp, 'y');

        temp = choose(*it, 2, 0);
        if (existT<QString>(I, temp) && !Var.contains(temp))
            Var.insert(temp, 'y');

        temp = choose(*it, 3, 0);
        if (existT<QString>(I, temp) && !Var.contains(temp))
            Var.insert(temp, 'y');

        if (!existT<QString>(I, temp) && temp.startsWith('t') && !Var.contains(temp))
            Var.insert(temp, 'n');
    }
}

/**
 * @brief 逆序处理四元式，填写所有变量的活跃信息
 *
 * 从后往前遍历四元式序列，根据变量的使用情况更新其活跃信息，
 * 并将活跃信息插入到四元式字符串中。
 */
//逆序对四元式操作，填写所有变量的活跃信息
void ObjectCode::activeinf()
{
    for (auto it = Quadruple.rbegin(); it != Quadruple.rend(); ++it)
    {
        QString temp = *it;
        int comma = 0;

        // 查找最后一个'('的位置
        int j = temp.lastIndexOf('(');
        if (j == -1) continue; // 如果没有找到'('则跳过

        // 从右向左遍历字符串
        for (; j >= 0; --j)
        {
            if (temp[j] == ',')
            {
                comma++;
                int k = j + 1;
                while (k < temp.size() && temp[k] != ',' && temp[k] != ')')
                {
                    ++k;
                }

                QString s = temp.mid(j + 1, k - j - 1);
                if (Var.contains(s))
                {
                    QChar symbol = Var.value(s);
                    QString sert = QString("(%1)").arg(symbol);

                    temp.replace(j + 1, k - j - 1, sert);

                    if (comma == 1) {
                        Var.insert(s, 'n'); // 结果变量赋为不活跃
                    }
                    else if (comma == 2 || comma == 3) {
                        Var.insert(s, 'y');
                    }
                }
            }
        }
        *it = temp;
    }
}

/**
 * @brief 从字符串中提取指定位置的内容
 *
 * 根据逗号的位置和 lag 参数提取字符串中的内容，
 * 若参数不合法或未找到指定位置的逗号，输出错误信息并退出程序。
 *
 * @param s 输入的字符串
 * @param comma 要查找的逗号序号
 * @param lag 提取模式，0 表示提取逗号后的内容，1 表示提取分隔符后的单个字符
 * @return 提取的内容
 */
QString ObjectCode::choose(const QString &s, int comma, int lag)
{
    if (comma > 3 || comma < 0) {
        qCritical() << "错误！参数comma超出范围";
        exit(-1);
    }

    int cm = 0;
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == ',') cm++;
        if (comma == cm) {
            int j;
            if (lag == 0) {
                // 查找下一个分隔符
                for (j = i + 1; j < s.size() && s[j] != '(' && s[j] != ',' && s[j] != ')'; j++) {}
                    return s.mid(i + 1, j - i - 1); // 获取逗号后的内容
            }
            else if (lag == 1) {
                // 查找下一个分隔符
                for (j = i + 1; j < s.size() && s[j] != '(' && s[j] != ',' && s[j] != ')'; j++) {}
                    return s.mid(j + 1, 1); // 获取分隔符后的单个字符
            }
            else {
                qCritical() << "错误！参数lag无效";
                exit(-1);
            }
        }
    }

    qCritical() << "错误！未找到指定位置的逗号";
    exit(-1);
}

/**
 * @brief 生成目标代码
 *
 * 遍历四元式序列，根据四元式的操作类型生成相应的目标代码，
 * 并考虑寄存器的使用和变量的活跃信息。
 */
void ObjectCode::fillObj()//写目标代码
{
    for (auto it = Quadruple.begin(); it != Quadruple.end(); ++it)
    {
        if (choose(*it, 0, 0) == "=")//赋值
        {
            if (RDL::R == "0")//寄存器空
            {
                QString s = QString::number(count - 1) + " LD R," + choose(*it, 1, 0);
                Object.push_back(s);
                count++;
            }
            else if (RDL::R == choose(*it, 1, 0))//寄存器是第一个操作数
            {
                if (choose(*it, 1, 1) == "y")//活跃的必须ST
                {
                    QString s = QString::number(count - 1) + " ST R," + choose(*it, 1, 0);
                    Object.push_back(s);
                    count++;
                }
            }
            else if (RDL::R != choose(*it, 1, 0) && RDL::R != choose(*it, 3, 0))
            {//寄存器不是操作数，是结果数
                if (RDL::active == "y")
                {
                    QString s_1 = QString::number(count - 1) + " ST R," + RDL::R;
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = QString::number(count - 1) + " LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    count++;
                }
                else
                {
                    QString s = QString::number(count - 1) + " LD R," + choose(*it, 1, 0);
                    Object.push_back(s);
                    count++;
                }
            }
            RDL::R = choose(*it, 3, 0);//寄存器保存结果数
            RDL::active = choose(*it, 3, 1);
        }//是赋值语句
        else if (Operate.contains(choose(*it, 0, 0)) && Operate[choose(*it, 0, 0)][0] == "w")
        {//是加减等语句
            if (RDL::R == "0")//寄存器空
            {
                QString s_1 = QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                Object.push_back(s_1);
                count++;
                QString s_2 = QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                Object.push_back(s_2);
                count++;
            }
            else if (RDL::R == choose(*it, 1, 0))//寄存器是首个操作数
            {
                if (choose(*it, 1, 1) == "y")//活跃必须保存
                {
                    QString s_1 = QString::number(count - 1) + " " + "ST R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s_2);
                    count++;
                }
                else
                {
                    QString s = QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s);
                    count++;
                }
            }
            else if (RDL::R == choose(*it, 2, 0) && Operate[choose(*it, 0, 0)][2] == "y")
            {//是第二个操作数，而且可交换
                if (choose(*it, 2, 1) == "y")//活跃
                {
                    QString s_1 = QString::number(count - 1) + " " + "ST R," + choose(*it, 2, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    count++;
                }
                else
                {
                    QString s = QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 1, 0);
                    Object.push_back(s);
                    count++;
                }
            }
            else//不可交换
            {
                if (RDL::active == "y")
                {
                    QString s_1 = QString::number(count - 1) + " " + "ST R," + RDL::R;
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    count++;
                    QString s_3 = QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s_3);
                    count++;
                }
                else
                {
                    QString s_1 = QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s_2);
                    count++;
                }
            }

            RDL::R = choose(*it, 3, 0);
            RDL::active = choose(*it, 3, 1);
        }
        else if (choose(*it, 0, 0) == "if")
        {
            if (RDL::R == "0")
            {
                QString s_1 = QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                Object.push_back(s_1);
                count++;
                QString s_2 = QString::number(count - 1) + " " + "FJ R,";
                Object.push_back(s_2);
                count++;
                SEM.push(count - 1);
            }
            else if (RDL::R == choose(*it, 1, 0))
            {
                if (choose(*it, 1, 1) == "y")
                {
                    QString s_1 = QString::number(count - 1) + " " + "ST R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s_2);
                    count++;
                }
                else
                {
                    QString s =  QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s);
                    count++;
                }
                SEM.push(count - 1);
                RDL::R = "0";
                RDL::active = "0";
            }
            else
            {
                if (RDL::active == "y")
                {
                    QString s_1 =   QString::number(count - 1) + " " + "ST R," + RDL::R;
                    Object.push_back(s_1);
                    count++;
                    QString s_2 =   QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    count++;
                    QString s_3 =   QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s_3);
                    count++;
                }
                else
                {
                    QString s_1 =   QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 =   QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s_2);
                    count++;
                }
                SEM.push(count - 1);
                RDL::R = "0";
                RDL::active = "0";
            }
        }
        else if (choose(*it, 0, 0) == "el")
        {
            if (RDL::active == "y")
            {
                QString s =   QString::number(count - 1) + " " + "ST R," + RDL::R;
                Object.push_back(s);
                count++;
            }
            QString s_1 =   QString::number(count + 1);
            Object[SEM.top() - 1] = Object[SEM.top() - 1] + s_1;
            SEM.pop();
            QString s_2 =   QString::number(count - 1) + " " + "JMP ";
            Object.push_back(s_2);
            count++;
            SEM.push(count - 1);
        }
        else if (choose(*it, 0, 0) == "ie")
        {
            if (RDL::active == "y")
            {
                QString s =   QString::number(count - 1) + " " + "ST R," + RDL::R;
                Object.push_back(s);
                count++;
            }
            QString s =   QString::number(count - 1);
            Object[SEM.top() - 1] = Object[SEM.top() - 1] + s;
            SEM.pop();
        }
        else if (choose(*it, 0, 0) == "wh")
        {
            SEM.push(count);
        }
        else if (choose(*it, 0, 0) == "do")
        {
            if (RDL::R == "0")
            {
                QString s_1 =   QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                Object.push_back(s_1);
                count++;
                QString s_2 =   QString::number(count - 1) + " " + "FJ R,";
                Object.push_back(s_2);
                count++;
                SEM.push(count - 1);
            }
            else if (RDL::R == choose(*it, 1, 0))
            {
                if (choose(*it, 1, 1) == "y")
                {
                    QString s_1 =   QString::number(count - 1) + " " + "ST R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 =   QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s_2);
                    count++;
                }
                else
                {
                    QString s =   QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s);
                    count++;
                }
                SEM.push(count - 1);
                RDL::R = "0";
                RDL::active = "0";
            }
            else
            {
                if (RDL::active == "y")
                {
                    QString s_1 =   QString::number(count - 1) + " " + "ST R," + RDL::R;
                    Object.push_back(s_1);
                    count++;
                    QString s_2 =   QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    count++;
                    QString s_3 =   QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s_3);
                    count++;
                }
                else
                {
                    QString s_1 =   QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 =   QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s_2);
                    count++;
                }
                SEM.push(count - 1);
                RDL::R = "0";
                RDL::active = "0";
            }
        }
        else if (choose(*it, 0, 0) == "we")
        {
            if (RDL::active == "y")
            {
                QString s =   QString::number(count - 1) + " " + "ST R," + RDL::R;
                Object.push_back(s);
                count++;
            }
            RDL::R = "0";
            RDL::active = "0";
            QString s_1 =   QString::number(count);
            Object[SEM.top() - 1] = Object[SEM.top() - 1] + s_1;
            SEM.pop();
            QString s_2 =   QString::number(count - 1) + " " + "JMP " + QString::number(SEM.top());
            Object.push_back(s_2);
            count++;
            SEM.pop();
        }
        else if (choose(*it, 0, 0) == "fun")
        {
            count++;
            Object.push_back(choose(*it, 1, 0) + " " + "SEGMENT");
        }
        else if (choose(*it, 0, 0) == "endfun")
        {
            Object.push_back("SEGMENT END");
        }
        else if (choose(*it, 0, 0) == "ret")
        {
            QString s =  QString::number(count - 1) + " LD R," + choose(*it, 1, 0);
            Object.push_back(s);
            count++;
            RDL::R = choose(*it, 1, 0);
            RDL::active = choose(*it, 1, 0);
        }
        else
        {
            qDebug() << "目标代码生成错误！";
            exit(-1);
        }
    }
}

/**
 * @brief 扫描并处理四元式文件
 *
 * 读取四元式文件内容，按语句块分割，对每个语句块提取变量、
 * 更新活跃信息并生成目标代码，最后调用 print 方法输出目标代码。
 */
void ObjectCode::scan()
{
    QString str = "$";
    QFile inFile("TextFile/optQuat.txt"); // 使用Qt文件操作

    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "无法打开文件: TextFile/optQuat.txt";
        return;
    }

    QTextStream in(&inFile);
    while (!in.atEnd()) {
        QString tem = in.readLine().trimmed();
        if (!tem.isEmpty()) {
            str += tem.mid(0,1) + "$"+tem.mid(2)+"$";
        }
    }
    inFile.close();

    if (!str.isEmpty()) {
        str[str.size() - 1] = '#';
    }

    int flag = 0;
    QChar k = str.size() > 1 ? str[1] : QChar();

    for (int i = 0; i < str.size() && str[i] != '#'; i++) {
        if (str[i] == '$') {
            if (i + 1 < str.size()) {
                if (str[i + 1] == k) {
                    flag = 1; // 相同语句块
                } else {
                    // 进入不同的语句块
                    getVar();
                    activeinf();
                    fillObj();
                    Quadruple.clear();
                    flag = 1;
                    k = str[i + 1];
                }
            }
        }

        if (flag == 1 && str[i] == '(') {
            int j = i;
            while (j < str.size() && str[j] != ')') {
                j++;
            }

            if (j < str.size() && str[j] == ')') {
                Quadruple.push_back(str.mid(i, j - i + 1)); // 使用mid()替代substr()
                i = j; // 跳到当前位置
                flag = 0; // 重置flag
            }
        }
    }

    print(); // 打印目标代码到文件，并输出在控制台
}

/**
 * @brief 将目标代码输出到文件并在控制台打印
 *
 * 打开目标文件，将生成的目标代码逐行写入文件，
 * 同时在控制台输出目标代码，最后输出完成信息。
 *
 * @return 操作成功返回 0，失败返回 -1
 */
int ObjectCode::print()
{
    QFile outFile("TextFile/ObjectCode.txt");
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "无法打开文件进行写入: TextFile/ObjectCode.txt";
        return -1;
    }

    QTextStream out(&outFile);
    for (const QString &line : Object) {
        out << line << Qt::endl;
        qDebug().noquote() << line;
    }

    outFile.close();
    qInfo() << "目标代码已输出...";
    return 0;
}

/**
 * @brief 打印变量表信息
 *
 * 输出变量表的标题和分隔线，遍历变量表，
 * 打印每个变量的名称和活跃信息，最后输出变量总数。
 */
// 在ObjectCode.cpp中添加如下函数
void ObjectCode::printVarTable()
{
    qDebug().noquote() << "符号表内容如下：";
    qDebug().noquote() << "-----------------------------";

    for (auto it = Var.constBegin(); it != Var.constEnd(); ++it) {
        qDebug().noquote() << QString("变量名: %1\t活跃信息: %2")
                                  .arg(it.key(), -15)  // -15表示左对齐，宽度15
                                  .arg(it.value());
    }

    qDebug().noquote() << "-----------------------------";
    qDebug().noquote() << QString("总计: %1 个变量").arg(Var.size());
}
