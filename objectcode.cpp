#include "objectcode.h"

ObjectCode::ObjectCode() {}

#include "ObjectCode.h"
#include <QString>
#include <QVector>

#include <QMap>
#include <QStack>

#include<QDebug>
#include<QFile>

using namespace std;

QStack <int> SEM;	//栈

extern QVector<QString> I;    //声明，不是定义
QVector <QString> Quadruple;		//待处理的四元式序列
QVector <QString> Object;			//翻译语句
QMap <QString, QChar> Var;			//变量以及活跃信息
QMap <QString, QVector<QString>> Operate = { {"+",{"w","add","y"}},{"-",{"w","sub","n"}},{"*",{"w","mul","y"}},
                                       {"/",{"w","div","n"}},{">",{"w","gt","n"}},{"<",{"w","lt","n"}},
                                       {"==",{"w","eq","y"}},{"!=",{"w","ne","y"}},
                                       {">=",{"w","ge","n"}} ,{"<=",{"w","le","n"}} };

int ObjectCode::count = 1;
QString RDL::R = "0";
QString RDL::active = "0";

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

template<typename T>bool ObjectCode::existT(QVector<T> v, T t)
{
    for (int i = 0; i < (int)v.size(); i++)
    {
        if (t == v[i]) return true;
    }
    return false;
}

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
                if (j > i + 1) {
                    return s.mid(i + 1, j - i - 1); // 获取逗号后的内容
                }
            }
            else if (lag == 1) {
                // 查找下一个分隔符
                for (j = i + 1; j < s.size() && s[j] != '(' && s[j] != ',' && s[j] != ')'; j++) {}
                if (j + 1 < s.size()) {
                    return s.mid(j + 1, 1); // 获取分隔符后的单个字符
                }
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

void ObjectCode::fillObj()//写目标代码
{
    for (auto it = Quadruple.begin(); it != Quadruple.end(); ++it)
    {
        if (choose(*it, 0, 0) == "=")//赋值
        {
            if (RDL::R == "0")//寄存器空
            {
                QString s;
                s = "lab" + QString::number(count - 1) + " LD R," + choose(*it, 1, 0);
                Object.push_back(s);
                count++;
            }
            else if (RDL::R == choose(*it, 1, 0))//寄存器是第一个操作数
            {
                if (choose(*it, 1, 1) == "y")//活跃的必须ST
                {
                    QString s = "lab" + QString::number(count - 1) + " ST R," + choose(*it, 1, 0);
                    Object.push_back(s);
                    count++;
                }
            }
            else if (RDL::R != choose(*it, 1, 0) && RDL::R != choose(*it, 3, 0))
            {//寄存器不是操作数，是结果数
                if (RDL::active == "y")
                {
                    QString s_1 = "lab" + QString::number(count - 1) + " ST R," + RDL::R;
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    count++;
                }
                else
                {
                    QString s = "lab" + QString::number(count - 1) + " LD R," + choose(*it, 1, 0);
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
                QString s_1 = "lab" + QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                Object.push_back(s_1);
                count++;
                QString s_2 = "lab" + QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                Object.push_back(s_2);
                count++;
            }
            else if (RDL::R == choose(*it, 1, 0))//寄存器是首个操作数
            {
                if (choose(*it, 1, 1) == "y")//活跃必须保存
                {
                    QString s_1 = "lab" + QString::number(count - 1) + " " + "ST R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s_2);
                    count++;
                }
                else
                {
                    QString s = "lab" + QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s);
                    count++;
                }
            }
            else if (RDL::R == choose(*it, 2, 0) && Operate[choose(*it, 0, 0)][2] == "y")
            {//是第二个操作数，而且可交换
                if (choose(*it, 2, 1) == "y")//活跃
                {
                    QString s_1 = "lab" + QString::number(count - 1) + " " + "ST R," + choose(*it, 2, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    count++;
                }
                else
                {
                    QString s = "lab" + QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 1, 0);
                    Object.push_back(s);
                    count++;
                }
            }
            else//不可交换
            {
                if (RDL::active == "y")
                {
                    QString s_1 = "lab" + QString::number(count - 1) + " " + "ST R," + RDL::R;
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    count++;
                    QString s_3 = "lab" + QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
                    Object.push_back(s_3);
                    count++;
                }
                else
                {
                    QString s_1 = "lab" + QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " " + Operate[choose(*it, 0, 0)][1] + " R," + choose(*it, 2, 0);
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
                QString s_1 = "lab" + QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                Object.push_back(s_1);
                count++;
                QString s_2 = "lab" + QString::number(count - 1) + " " + "FJ R,";
                Object.push_back(s_2);
                count++;
                SEM.push(count - 1);
            }
            else if (RDL::R == choose(*it, 1, 0))
            {
                if (choose(*it, 1, 1) == "y")
                {
                    QString s_1 = "lab" + QString::number(count - 1) + " " + "ST R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s_2);
                    count++;
                }
                else
                {
                    QString s = "lab" + QString::number(count - 1) + " " + "FJ R,";
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
                    QString s_1 = "lab" + QString::number(count - 1) + " " + "ST R," + RDL::R;
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    count++;
                    QString s_3 = "lab" + QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s_3);
                    count++;
                }
                else
                {
                    QString s_1 = "lab" + QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " " + "FJ R,";
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
                QString s = "lab" + QString::number(count - 1) + " " + "ST R," + RDL::R;
                Object.push_back(s);
                count++;
            }
            QString s_1 = "lab" + QString::number(count + 1);
            Object[SEM.top() - 1] = Object[SEM.top() - 1] + s_1;
            SEM.pop();
            QString s_2 = "lab" + QString::number(count - 1) + " " + "JMP ";
            Object.push_back(s_2);
            count++;
            SEM.push(count - 1);
        }
        else if (choose(*it, 0, 0) == "ie")
        {
            if (RDL::active == "y")
            {
                QString s = "lab" + QString::number(count - 1) + " " + "ST R," + RDL::R;
                Object.push_back(s);
                count++;
            }
            QString s = "lab" + QString::number(count - 1);
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
                QString s_1 = "lab" + QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                Object.push_back(s_1);
                count++;
                QString s_2 = "lab" + QString::number(count - 1) + " " + "FJ R,";
                Object.push_back(s_2);
                count++;
                SEM.push(count - 1);
            }
            else if (RDL::R == choose(*it, 1, 0))
            {
                if (choose(*it, 1, 1) == "y")
                {
                    QString s_1 = "lab" + QString::number(count - 1) + " " + "ST R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s_2);
                    count++;
                }
                else
                {
                    QString s = "lab" + QString::number(count - 1) + " " + "FJ R,";
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
                    QString s_1 = "lab" + QString::number(count - 1) + " " + "ST R," + RDL::R;
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_2);
                    count++;
                    QString s_3 = "lab" + QString::number(count - 1) + " " + "FJ R,";
                    Object.push_back(s_3);
                    count++;
                }
                else
                {
                    QString s_1 = "lab" + QString::number(count - 1) + " " + "LD R," + choose(*it, 1, 0);
                    Object.push_back(s_1);
                    count++;
                    QString s_2 = "lab" + QString::number(count - 1) + " " + "FJ R,";
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
                QString s = "lab" + QString::number(count - 1) + " " + "ST R," + RDL::R;
                Object.push_back(s);
                count++;
            }
            RDL::R = "0";
            RDL::active = "0";
            QString s_1 = "lab" + QString::number(count);
            Object[SEM.top() - 1] = Object[SEM.top() - 1] + s_1;
            SEM.pop();
            QString s_2 = "lab" + QString::number(count - 1) + " " + "JMP lab" + QString::number(SEM.top());
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
            QString s = "lab" + QString::number(count - 1) + " LD R," + choose(*it, 1, 0);
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
            str += tem + "$";
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
