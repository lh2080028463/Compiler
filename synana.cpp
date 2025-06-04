#include "synana.h"
#include <iostream>
#include <fstream>
#include <QList>
#include <QMap>
#include <QString>
#include <QVector>
#include <algorithm>
#include "symboltable.h"
#include "createquat.h"
#include<QList>
#include<QMap>
#include<wordana.h>
#include<global.h>
#include<QDebug>
#include<QFile>

using namespace std;
QVector<QString> VNQList;   //非终结符列表
QVector<QString> VTQList;   //终结符列表
//LL(1)分析表，格式为{"E", {{"i", 1}, {"(", 1}}}
//"E"为行表头，"i"和"("为列表头，1为该表项内容即对应的产生式序号
QMap<QString, QMap<QString, int>> LL1Table;

QVector<QVector<QString>> grammarRight;  //grammar按序号存储所有产生式右部

bool SynAna::AN(QString program)
{
    VNQList.clear(); VTQList.clear();
    LL1Table.clear();
    grammarRight.clear();
    QList<QString> stack;   //分析栈
    stack.push_back("#");
    stack.push_back("<函数>");  //开始符号入栈
    QString topSymbol;   //栈顶符号
    Token nowWord;   //现在分析的单词
    Token lastWord;
    GetLL1Table();

    SymbolTable symTable;
    createquat quat;
    quat.clear();
    WordAna wordAna;
    wordAna.clear();
    wordAna.scan(program, nowWord);
    //qDebug() << nowWord.value << Qt::endl;
    //qDebug() << "1" << Qt::endl;
    while (1)
    {
        topSymbol = stack.back();
        stack.pop_back();
        //如果栈顶符号是终结符
        if (isVT(topSymbol))
        {
            if (topSymbol == nowWord.value
                || (topSymbol == "<标识符>" && nowWord.type == TokenType::I)
                || (topSymbol == "<无符号整数>" && nowWord.type == TokenType::C))
            {
                lastWord = nowWord;
                //qDebug() << lastWord.value << Qt::endl;
                wordAna.scan(program, nowWord);
                //qDebug()<< nowWord.value << Qt::endl;

            }
            else
            {
                qDebug()<<"语法错误！请检查！"<<Qt::endl;
                qDebug() << "错误信息：" + nowWord.value << Qt::endl;
                return false;
            }
        }
        //如果栈顶符号是非终结符
        else if (isVN(topSymbol))
        {
            int graNum;
            if (nowWord.type == TokenType::I)
                graNum = LL1Table[topSymbol]["<标识符>"];
            else if (nowWord.type == TokenType::C)
                graNum = LL1Table[topSymbol]["<无符号整数>"];
            else
                graNum = LL1Table[topSymbol][nowWord.value];
            if (graNum == -1)
            {
                qDebug() << "语法错误！请检查！" << Qt::endl;
                qDebug() << "错误信息：" + nowWord.value<< Qt::endl;
                return false;
            }
            else
            {
                //产生式右部逆序压栈
                for (auto i = grammarRight[graNum].rbegin(); i != grammarRight[graNum].rend(); i++)
                    if (*i != "<空>") stack.push_back(*i);
            }
        }
        //如果栈顶符号是符号表填写动作符号
        else if (isSyTb(topSymbol))
        {
            symTable.callAction(topSymbol, nowWord);
        }
        else if (isQuat(topSymbol))
        {
            quat.action(topSymbol, lastWord.value);
        }
        //如果栈顶符号是"#"
        else if (topSymbol == "#")
        {
            if (nowWord.value == "#")
            {
                qDebug() << "语法分析已完成！" << Qt::endl;
                //wordAna.print(Token);
                break;
            }
            else
            {
                qDebug() << "语法错误！请检查！" << Qt::endl;
                qDebug() << "错误信息：已读取所有源码，语法分析器未成功结束！" << Qt::endl;
                return false;
                //exit(1);
            }
        }
        else
        {
            qDebug() << "语法错误！请检查！" << Qt::endl;
            qDebug() << "错误信息：未知错误！" << Qt::endl;
            qDebug() << topSymbol << Qt::endl;
            return false;
            //exit(1);
        }
    }

    //输出符号表
    symTable.printTable();
    quat.PRINT();

    return true;
}
bool SynAna::isVN(QString str)
{
    if (find(VNQList.begin(), VNQList.end(), str) == VNQList.end())
        return false;
    else
        return true;
}
bool SynAna::isVT(QString str)
{
    if (find(VTQList.begin(), VTQList.end(), str) == VTQList.end())
        return false;
    else
        return true;
}
bool SynAna::isQuat(QString str)
{
    if (str[0] == '$' && str != "$")return true;
    else return false;
}
bool SynAna::isSyTb(QString str)
{
    if (str[0] == '\"' && str != "\"")return true;
    else return false;
}
//用于分析文法，生成非终结符，终结符，右部集合
//求取First集，Follow集，Select集，判定文法是否为LL(1)文法
//若是，生成LL(1)分析表
//对所分析文法的格式要求：
//1.所有非终结符用<>括起来   2. 任意符号之间用空格分开
//3.每行第一个符号为产生式的非终结符，后面是右部，不需写->，右部之间用'|'分隔
//4.每行以'#'结尾
bool SynAna::GetLL1Table()
{
    QVector<QVector<QString>> grammar;
    QVector<QVector<int>> rightQList;
    QVector<int> leftQList;
    QString temp;
    QVector<QString> tempRight, tempRightAll;

    QFile file("TextFile/grammar.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开grammar.txt";
        return false;
    }
    QTextStream in(&file);

    while (!in.atEnd()) {
        in >> temp;
        VNQList.push_back(temp);
        rightQList.push_back({});
        tempRight.clear();
        tempRightAll.clear();
        while (true) {
            in >> temp;
            if (temp == "|" || temp == "#") {
                grammar.push_back(tempRight);
                grammarRight.push_back(tempRightAll);
                rightQList.last().push_back(grammar.size() - 1);
                leftQList.push_back(VNQList.size() - 1);
                if (temp == "|") {
                    tempRight.clear();
                    tempRightAll.clear();
                    continue;
                } else break;
            } else {
                if ((temp[0] != '<' && temp[0] != '\"' && temp[0] != '$') || temp == "<" || temp == "\"") {
                    if (!VTQList.contains(temp))
                        VTQList.push_back(temp);
                }
                if ((temp[0] != '\"' && temp[0] != '$') || temp == "\"")
                    tempRight.push_back(temp);
                tempRightAll.push_back(temp);
            }
        }
    }
    VTQList.push_back("<标识符>");
    VTQList.push_back("<无符号整数>");
    file.close();

    QFile file1("TextFile/Symbol.txt");
    if (file1.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file1);
        out << "非终结符：\n";
        for (const auto& vn : VNQList) out << vn << ' ';
        out << '\n';
        out << "\n终结符：\n";
        for (const auto& vt : VTQList) out << vt << ' ';
        out << '\n';
        out << "\n右部：\n";
        int n = 1;
        for (const auto& g : grammar) {
            out << n++ << ": ";
            for (const auto& s : g) out << s << ' ';
            out << "\n";
        }
        file1.close();
    }

    qDebug() << "非终结符：";
    for (const auto& vn : VNQList)
        qDebug() << vn;
    qDebug();

    qDebug() << "终结符：";
    for (const auto& vt : VTQList)
        qDebug() << vt;
    qDebug();

    qDebug() << "产生式右部：";
    int n = 1;
    for (const auto& g : grammar) {
        qDebug().noquote() << QString("%1: %2").arg(n++).arg(g.join(' '));
    }
    qDebug();

    // 求First集
    QMap<QString, QSet<QString>> FIRST_VNVT;
    QVector<QSet<QString>> FIRST(grammar.size());

    // 终结符First集为本身
    for (const auto& vt : VTQList)
        FIRST_VNVT[vt] = {vt};

    // 非终结符First集为空
    for (const auto& vn : VNQList)
        FIRST_VNVT[vn] = {};

    bool change = true;
    while (change) {
        change = false;
        for (int m = 0; m < grammar.size(); ++m) {
            const auto& production = grammar[m];
            QSet<QString> tempFIRST_VNVT = FIRST_VNVT[production.first()];
            tempFIRST_VNVT.remove("<空>");

            int i = 1;
            while (i < production.size() && FIRST_VNVT[production[i-1]].contains("<空>")) {
                tempFIRST_VNVT.unite(FIRST_VNVT[production[i]]);
                tempFIRST_VNVT.remove("<空>");
                ++i;
            }

            if (i == production.size() && FIRST_VNVT[production.last()].contains("<空>"))
                tempFIRST_VNVT.insert("<空>");

            if (production.size() == 1 && production[0] == "<空>")
                tempFIRST_VNVT.insert("<空>");

            QString leftVN = VNQList[leftQList[m]];
            if (FIRST_VNVT[leftVN].unite(tempFIRST_VNVT).size() > FIRST_VNVT[leftVN].size()) {
                change = true;
            }

            if (FIRST[m].unite(tempFIRST_VNVT).size() > FIRST[m].size()) {
                change = true;
            }
        }
    }

    qDebug() << "First集：";
    for (int m = 0; m < FIRST.size(); ++m) {
        qDebug().noquote() << QString("%1: %2").arg(m+1).arg(QStringList(FIRST[m].toList()).join(' '));
    }
    qDebug();

    // 求Follow集
    QMap<QString, QSet<QString>> FOLLOW;
    for (const auto& vn : VNQList)
        FOLLOW[vn] = {};
    FOLLOW[VNQList.first()].insert("#");

    change = true;
    while (change) {
        change = false;
        for (int m = 0; m < grammar.size(); ++m) {
            const auto& production = grammar[m];
            QSet<QString> Trailer = FOLLOW[VNQList[leftQList[m]]];

            for (auto it = production.rbegin(); it != production.rend(); ++it) {
                if (VNQList.contains(*it)) {
                    int oldSize = FOLLOW[*it].size();
                    FOLLOW[*it].unite(Trailer);
                    if (FOLLOW[*it].size() > oldSize) change = true;

                    if (FIRST_VNVT[*it].contains("<空>")) {
                        Trailer.unite(FIRST_VNVT[*it]);
                        Trailer.remove("<空>");
                    } else {
                        Trailer = FIRST_VNVT[*it];
                    }
                } else {
                    Trailer = FIRST_VNVT[*it];
                }
            }
        }
    }

    qDebug() << "Follow集：";
    for (const auto& vn : VNQList) {
        qDebug().noquote() << QString("%1: %2").arg(vn).arg(QStringList(FOLLOW[vn].toList()).join(' '));
    }
    qDebug();

    // 求Select集
    QVector<QSet<QString>> SELECT(grammar.size());
    for (int i = 0; i < grammar.size(); ++i) {
        if (!FIRST[i].contains("<空>")) {
            SELECT[i] = FIRST[i];
        } else {
            SELECT[i] = FOLLOW[VNQList[leftQList[i]]];
            SELECT[i].unite(FIRST[i]);
            SELECT[i].remove("<空>");
        }
    }

    qDebug() << "Select集：";
    for (int i = 0; i < SELECT.size(); ++i) {
        qDebug().noquote() << QString("%1: %2").arg(i+1).arg(QStringList(SELECT[i].toList()).join(' '));
    }
    qDebug();

    // 检查是否为LL(1)文法
    for (int i = 0; i < VNQList.size(); ++i) {
        QSet<QString> tempSelect;
        for (int j : rightQList[i]) {
            if (!tempSelect.intersect(SELECT[j]).isEmpty()) {
                qDebug() << "文法非LL(1)文法！请检查！";
                qDebug() << "提示信息：左部" << VNQList[i];
                return false;
            }
            tempSelect.unite(SELECT[j]);
        }
    }
    qDebug() << "文法满足LL(1)文法要求。";

    // 构建LL1分析表
    for (const auto& vn : VNQList) {
        for (const auto& vt : VTQList)
            LL1Table[vn][vt] = -1;
        LL1Table[vn]["#"] = -1;
    }

    for (int i = 0; i < VNQList.size(); ++i) {
        for (int x : rightQList[i]) {
            for (const auto& k : SELECT[x]) {
                if (VTQList.contains(k) || k == "#")
                    LL1Table[VNQList[i]][k] = x;
            }
        }
    }

    // 输出LL(1)分析表到文件
    QFile file5("TextFile/LL1Table.txt");
    if (file5.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file5);
        out << "LL(1)分析表\t";
        for (const auto& vt : VTQList) out << vt << "\t\t";
        out << "#\t\n";
        for (const auto& vn : VNQList) {
            out << vn;
            for (int k = vn.size(); k < 15; k++) out << ' ';
            out << '\t';
            for (const auto& vt : VTQList)
                out << LL1Table[vn][vt] << "\t\t";
            out << LL1Table[vn]["#"] << '\t';
            out << '\n';
        }
        file5.close();
    }

    return true;
}
