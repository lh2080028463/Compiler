#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "wordana.h"

#include<QTextStream>

class SYNBLEntry   // 符号表总表表项
{
public:
    SYNBLEntry() = default;
    SYNBLEntry(const QString& n, const QString& t, const QString& c, const QString& a)
        : name(n), type(t), category(c), address(a) {}

    friend QTextStream& operator<<(QTextStream& out, const SYNBLEntry& entry);


    QString name;    // 标识符名字
    QString type;    // 标识符类型
    QString category; // 标识符种类
    QString address; // 地址指针

};

class TAPELEntry   // 类型表表项
{
public:
    TAPELEntry() = default;
    TAPELEntry(const QString& tv, const QString& tp)
        : typeValue(tv), typePoint(tp) {}

    friend QTextStream& operator<<(QTextStream& out, const TAPELEntry& entry);


    QString typeValue;  // 类型
    QString typePoint;  // 类型信息指针
};

class PFINFLEntry   // 函数表表项
{
public:
    PFINFLEntry() = default;
    PFINFLEntry(int lv, int os, int pn, const QString& pt)
        : level(lv), offset(os), paraNum(pn), paraTable(pt) {}

    void addParaNum() { paraNum++; }
    void modParaTable(const QString& s) { paraTable = s; }
    int getParaNum() const { return paraNum; }

    friend QTextStream& operator<<(QTextStream& out, const PFINFLEntry& entry);


    int level;      // 层次号
    int offset;     // 区距
    int paraNum;    // 参数个数
    QString paraTable; // 参数表指针

};

class SymbolTable
{
public:
    typedef int (SymbolTable::* Act)(Token tk);
    SymbolTable();
    int callAction(const QString& action, Token tk);
    int printTable();


    static QMap<QString, Act> ACTIONL;
    QVector<SYNBLEntry> SYNBL;
    QVector<TAPELEntry> TAPEL;
    QVector<PFINFLEntry> PFINFL;
    QVector<QString> VALL;

    int func(Token tk);
    int formalParaVar(Token tk);
    int locVariable(Token tk);
    int inputDisp(Token tk);
};

#endif
