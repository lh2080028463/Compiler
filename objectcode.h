#ifndef OBJECTCODE_H
#define OBJECTCODE_H

#include<QString>
#include<QVector>

class RDL
{
public:
    static QString R;
    static QString active;
};

class ObjectCode
{
public:
    ObjectCode();
    static int count;
    template <typename T>bool existT(QVector<T> v, T t);
    void clear();
    void getVar();//把变量从四元式中提取出来，设置初始的活跃信息，标识符为活跃，临时变量是不活跃的
    void activeinf();//逆序对四元式操作，增加四元式的活跃信息
    void fillObj();
    void scan();
    int print();
    void printVarTable();
    QString choose(const QString &s, int comma, int lag);	//获取第comma个逗号后的标识符(lag=0)/活跃信息(lag=1)
};




#endif // OBJECTCODE_H
