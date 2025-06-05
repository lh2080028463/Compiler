#include "symboltable.h"


#include<QDebug>
#include<QFile>

// 移除标准命名空间 using namespace std;

QDebug operator<<(QDebug debug, const SYNBLEntry& entry)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << qSetFieldWidth(8) << entry.name
                    << qSetFieldWidth(12) << entry.type
                    << qSetFieldWidth(8) << entry.category
                    << "\t" << qSetFieldWidth(8) << entry.address;
    return debug;
}

QDebug operator<<(QDebug debug, const TAPELEntry& entry)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << entry.typeValue << "\t" << qSetFieldWidth(12) << entry.typePoint;
    return debug;
}

QDebug operator<<(QDebug debug, const PFINFLEntry& entry)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << qSetFieldWidth(5) << entry.level << "\t"
                    << qSetFieldWidth(4) << entry.offset << "\t"
                    << qSetFieldWidth(8) << entry.paraNum << "\t"
                    << qSetFieldWidth(9) << entry.paraTable;
    return debug;
}

QMap<QString, SymbolTable::Act> SymbolTable::ACTIONL =
    {
        {QStringLiteral("函数名"), &SymbolTable::func},
        {QStringLiteral("int形参名"), &SymbolTable::formalParaVar},
        {QStringLiteral("int变量名"), &SymbolTable::locVariable},
        {QStringLiteral("参数串结束"), &SymbolTable::inputDisp}
};

SymbolTable::SymbolTable()
{
    TAPEL.append({ QStringLiteral("int"), QStringLiteral("TAPEL[0]") });
    VALL.append(QStringLiteral("OLD SP"));
    VALL.append(QStringLiteral("返回地址"));
    VALL.append(QStringLiteral("全局display地址"));
}

int SymbolTable::callAction(const QString& action, Token tk)
{
    auto it = ACTIONL.find(action);
    if (it != ACTIONL.end()) {
        return (this->*(it.value()))(tk);
    }
    qWarning() << "Unknown action:" << action;
    return 0;
}

int SymbolTable::printTable()
{
    QFile file("TextFile/SymbolTable.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for writing:" << file.errorString();
        return 0;
    }

    QTextStream out(&file);
    out.setFieldAlignment(QTextStream::AlignLeft);

    int j = 0;
    out << "符号表总表\n序号\t标识符名\t类型\t种类\t地址指针\n";
    qDebug() << "符号表总表\n序号\t标识符名\t类型\t种类\t地址指针";

    for (const auto& entry : qAsConst(SYNBL)) {
        qDebug().nospace() << qSetFieldWidth(2) << j << "\t" << entry;
        out << qSetFieldWidth(2) << j++ << "\t"
            << qSetFieldWidth(8) << entry.name
            << qSetFieldWidth(12) << entry.type
            << qSetFieldWidth(8) << entry.category
            << "\t" << qSetFieldWidth(8) << entry.address << "\n";
    }

    j = 0;
    out << "\n\n类型表\n序号\t类型\t类型信息指针\n";
    qDebug() << "\n\n类型表\n序号\t类型\t类型信息指针";

    for (const auto& entry : qAsConst(TAPEL)) {
        qDebug().nospace() << qSetFieldWidth(2) << j << "\t" << entry;
        out << qSetFieldWidth(2) << j++ << "\t"
            << entry.typeValue << "\t" << qSetFieldWidth(12) << entry.typePoint << "\n";
    }

    j = 0;
    out << "\n\n函数表\n序号\t层次号\t区距\t参数个数\t参数表指针\n";
    qDebug() << "\n\n函数表\n序号\t层次号\t区距\t参数个数\t参数表指针";

    for (const auto& entry : qAsConst(PFINFL)) {
        qDebug().nospace() << qSetFieldWidth(2) << j << "\t" << entry;
        out << qSetFieldWidth(2) << j++ << "\t"
            << qSetFieldWidth(5) << entry.level << "\t"
            << qSetFieldWidth(4) << entry.offset << "\t"
            << qSetFieldWidth(8) << entry.paraNum << "\t"
            << qSetFieldWidth(9) << entry.paraTable << "\n";
    }

    j = VALL.size() - 1;
    out << "\n\n活动记录表VALL\n\n";
    qDebug() << "\n\n活动记录表VALL";

    for (auto i = VALL.crbegin(); i != VALL.crend(); ++i) {
        qDebug() << j << "   " << *i;
        out << j-- << "   " << *i << "\n";
    }

    file.close();
    qDebug() << "符号表已输出。";
    return 1;
}

int SymbolTable::func(Token tk)
{
    if (tk.type == TokenType::I)
    {
        VALL.append(QStringLiteral("0"));   // 填写VALL表参数个数
        PFINFL.append({ 0, 3, 0, QStringLiteral("TAPEL[0]") });   // 填写函数表
        QString addr = QString("PFINFL[%1]").arg(PFINFL.size() - 1);
        SYNBL.append({ tk.value, QStringLiteral("TAPEL[0]"), QStringLiteral("f"), addr });  // 填写符号表总表
    }
    return 1;
}

int SymbolTable::formalParaVar(Token tk)
{
    if (tk.type == TokenType::I)
    {
        VALL.append(tk.value);   // 填写VALL表
        QString addr = QString("VALL[%1]").arg(VALL.size() - 1);
        SYNBL.append({ tk.value, QStringLiteral("TAPEL[0]"), QStringLiteral("vf"), addr });  // 填写符号表总表

        if (!PFINFL.isEmpty()) {
            PFINFL.last().addParaNum();   // 递增函数表该函数形参个数
            VALL[3] = QString::number(PFINFL.last().getParaNum());  // 修改VALL表中参数个数
            addr = QString("SYNBL[%1]").arg(SYNBL.size() - 1);
            PFINFL.last().modParaTable(addr);  // 修改函数表该函数参数表指针
        }
    }
    return 1;
}

int SymbolTable::inputDisp(Token token)
{
    Q_UNUSED(token);
    VALL.append(QStringLiteral("Display表"));
    return 1;
}

int SymbolTable::locVariable(Token tk)
{
    if (tk.type == TokenType::I)
    {
        VALL.append(tk.value);   // 填写VALL表
        QString addr = QString("VALL[%1]").arg(VALL.size() - 1);
        SYNBL.append({ tk.value, QStringLiteral("TAPEL[0]"), QStringLiteral("v"), addr });  // 填写符号表总表
    }
    return 1;
}
