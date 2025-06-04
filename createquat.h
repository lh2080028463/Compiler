#ifndef CREATEQUAT_H
#define CREATEQUAT_H

#endif // CREATEQUAT_H
#ifndef CREATQUAT_H
#define CREATQUAT_H

#include<QString>
#include<QVector>

class quat//定义四元式类
{
public:
    QString op;//操作符
    QString arg1;//操作数1
    QString arg2;//操作数2
    QString result;//结果
};

class createquat//定义生成四元式类
{
public:
    void clear();//清空
    void action(QString top, QString temppush);
    void pushi(QString str);//<因子> <标识符> $PUSHi$ | <无符号整数> $PUSHi$ | ( <表达式> ) #
    void add();//加减乘除
    void sub();
    void mul();
    void div();
    void assign();//<赋值语句> @ <标识符>=<表达式> $ASSI$;
    void logic();//<条件> <表达式> <比较语句> $LOGIC$ #
    void IF();//<条件语句> if ( <条件> ) $IF$ <复合语句> <else语句> $IE$#
    void EL();//<else语句> else $EL$ <复合语句> | <空> #
    void IE();
    void WH();//<循环语句> while $WH$ ( <条件> )  $DO$ <复合语句> $WE$ #
    void DO();
    void WE();
    void RET();//<返回内容> ; | <表达式> ; $RET$#
    void FUN();//<函数> <返回类型> "函数名" $PUSHi$ <标识符> <参数列表> $FUN$ <复合语句> $FUNEND$ #
    void FUNEND();
    void PRINT();//输出
    static int q;//记录四元式表的长度
    static int t;//作为活跃变量的t后面跟着的数字，用作记录
};


#endif // CREATQUAT_H

