#ifndef SYNANA_H
#define SYNANA_H

#include "symboltable.h"
#include "createquat.h"
#include"wordana.h"
#include<QDir>


class SynAna                                   //递归子程序分析
{

public:
    SynAna(){};
    bool AN(QString program);//分析过程
private:
    bool GetLL1Table();//得到LL1分析表
    bool isVN(QString str);
    bool isVT(QString str);
    bool isSyTb(QString str);//判断是否为符号表
    bool isQuat(QString str);//判断是否为四元式

};

#endif // SYNANA_H
