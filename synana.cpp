#include "synana.h"

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
    VNQList.clear();
    VTQList.clear();

    QFile file("TextFile\\grammar.txt");
    //QFile file(QDir::currentPath()+"/TextFile/grammar.txt");
    qDebug()<<QDir::currentPath();

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
    QMap<QString, QVector<QString>> FIRST_VNVT;
    QVector<QVector<QString>> FIRST(grammar.size());

    // 终结符First集为本身
    for (const auto& vt : VTQList)
        FIRST_VNVT[vt] = {vt};

    // 非终结符First集为空
    for (const auto& vn : VNQList)
        FIRST_VNVT[vn] = {};

    bool change = true;
    while (change) {
        change = false;
        int m = 0;
        //对每一个产生式A->b
        for (auto j = grammar.begin(); j != grammar.end(); j++, m++)
        {
            //b=b1b2b3..., bi是VN | VT, tempFIRST_VNVT <- First(b1) - {空}
            QVector<QString> tempFIRST_VNVT ;
            tempFIRST_VNVT = FIRST_VNVT[*(j->begin())];
            auto nullItera = find(tempFIRST_VNVT.begin(), tempFIRST_VNVT.end(), "<空>");
            if (nullItera != tempFIRST_VNVT.end())tempFIRST_VNVT.erase(nullItera);
            int i = 1;
            //当First(b0...bi-1)包含空，即b0到bi-1都可为空，tempFIRST_VNVT += First(bi)
            while (find(FIRST_VNVT[(*j)[i - 1]].begin(), FIRST_VNVT[(*j)[i - 1]].end(), "<空>")
                       != FIRST_VNVT[(*j)[i - 1]].end()
                   && i <= (int)j->size() - 1)
            {
                for (auto k = FIRST_VNVT[(*j)[i]].begin(); k != FIRST_VNVT[(*j)[i]].end(); k++)
                    if (*k != "<空>") tempFIRST_VNVT.push_back(*k);
                i++;
            }
            //如果右部可推出空，tempFIRST_VNVT += <空>
            if (i == (int)j->size() - 1
                && find(FIRST_VNVT[(*j)[i - 1]].begin(), FIRST_VNVT[(*j)[i - 1]].end(), "<空>")
                       != FIRST_VNVT[(*j)[i - 1]].end())
                tempFIRST_VNVT.push_back("<空>");
            //如果右部=<空>，tempFIRST_VNVT <- <空>
            if (j->size() == 1 && *(j->begin()) == "<空>")
                tempFIRST_VNVT.push_back("<空>");
            //更新左部的First集
            for (auto k = tempFIRST_VNVT.begin(); k != tempFIRST_VNVT.end(); k++)
            {
                QString tempVN = VNQList[leftQList[m]];
                if (find(FIRST_VNVT[tempVN].begin(), FIRST_VNVT[tempVN].end(), *k) == FIRST_VNVT[tempVN].end())
                {
                    FIRST_VNVT[tempVN].push_back(*k);
                    change = true;
                }
            }
            //更新右部的First集
            for (auto k = tempFIRST_VNVT.begin(); k != tempFIRST_VNVT.end(); k++)
            {
                if (find(FIRST[m].begin(), FIRST[m].end(), *k) == FIRST[m].end())
                {
                    FIRST[m].push_back(*k);
                    change = true;
                }
            }
        }
    }

    qDebug() << "First集：";
    for(int m=1;m<=FIRST.size();m++){
        qDebug()<<m<<": ";
        for(int m1=0;m1<FIRST[m-1].size();m1++){
            qDebug()<<FIRST[m-1][m1];
        }
    }
    // for (int m = 0; m < FIRST.size(); ++m) {
    //     qDebug().noquote() << QString("%1: %2").arg(m+1).arg(QStringList(FIRST[m].values()).join(' '));
    // }

    // 求Follow集
    QMap<QString, QVector<QString>> FOLLOW;
    for (const auto& vn : VNQList)
        FOLLOW[vn] = {};
    FOLLOW[VNQList.first()].push_back("#");

    change = true;
    while (change) {
        change = false;
        int m=0;
        for (auto j = grammar.begin(); j != grammar.end(); j++, m++)
        {
            //Trailer <- Follow(A)
            QVector<QString> Trailer = FOLLOW[VNQList[leftQList[m]]];
            //从后往前遍历产生式右部
            for (auto i = j->rbegin(); i != j->rend(); i++)
            {
                //如果bi是非终结符，将其后跟着的单词first加到follow中
                if (find(VNQList.begin(), VNQList.end(), *i) != VNQList.end())
                {
                    //Follow(bi) += Trailer,存入无重复元素
                    for (auto k = Trailer.begin(); k != Trailer.end(); k++)
                    {
                        if (find(FOLLOW[*i].begin(), FOLLOW[*i].end(), *k) == FOLLOW[*i].end())
                        {
                            FOLLOW[*i].push_back(*k);//将当前左部的加到这个元素中
                            change = true;
                        }
                    }
                    //如果First(bi)包含空，Trailer += First(bi)-"<空>"
                    if (find(FIRST_VNVT[*i].begin(), FIRST_VNVT[*i].end(), "<空>") != FIRST_VNVT[*i].end())
                    {
                        for (auto k = FIRST_VNVT[*i].begin(); k != FIRST_VNVT[*i].end(); k++)
                        {
                            if (*k != "<空>")
                            {
                                if (find(Trailer.begin(), Trailer.end(), *k) == Trailer.end())
                                {
                                    Trailer.push_back(*k);
                                }
                            }
                        }
                    }
                    //否则Trailer = First(bi)
                    else
                    {
                        Trailer = FIRST_VNVT[*i];
                    }
                }
                //如果bi是终结符，Trailer = First(bi)
                else
                {
                    Trailer = FIRST_VNVT[*i];
                }
            }
        }
    }

    qDebug() << "Follow集：";
    for (const auto& vn : VNQList) {
        qDebug().noquote() << QString("%1: %2").arg(vn).arg(QStringList(FOLLOW[vn]).join(' '));
    }
    qDebug();

    // 求Select集
    QVector<QVector<QString>> SELECT(grammar.size());
    for (int i = 0; i < grammar.size(); ++i) {
        //如果First(b)不包含空，即右部不能推出空，select集等于右部first集
        if (find(FIRST[i].begin(), FIRST[i].end(), "<空>") == FIRST[i].end())
            SELECT[i] = FIRST[i];
        //否则，select集等于右部first集并上左部follow集
        else
        {
            SELECT[i] = FOLLOW[VNQList[leftQList[i]]];
            for (auto j = FIRST[i].begin(); j != FIRST[i].end(); j++)
            {
                if (*j != "<空>")
                    if (find(SELECT[i].begin(), SELECT[i].end(), *j) == SELECT[i].end())
                        SELECT[i].push_back(*j);
            }
        }
    }

    qDebug() << "Select集：";
    for(int i=1;i<=SELECT.size();i++){
        qDebug()<<i<<": ";
        for(int j=0;j<SELECT[i-1].size();j++){
            qDebug()<<SELECT[i-1][j];
        }
    }

    // for (int i = 0; i < SELECT.size(); ++i) {
    //     qDebug().noquote() << QString("%1: %2").arg(i+1).arg(QStringList(SELECT[i].values()).join(' '));
    // }

    //检查是否为LL1文法
    for (int i = 0; i < (int)VNQList.size(); i++)
    {
        QVector<QString> tempSelect;
        tempSelect.clear();
        for (auto j = rightQList[i].begin(); j != rightQList[i].end(); j++)
        {
            for (auto k = SELECT[*j].begin(); k != SELECT[*j].end(); k++)
            {
                if (find(tempSelect.begin(), tempSelect.end(), *k) == tempSelect.end())
                    tempSelect.push_back(*k);
                else
                {
                    qDebug() << "文法非LL(1)文法！请检查！\n" << "提示信息：左部" << VNQList[i];
                    return false;
                }
            }
        }
    }
    qDebug() << "文法满足LL(1)文法要求。";

    // 构建LL1分析表

    for (int i = 0; i < VNQList.size(); ++i) {
        for (auto j :VTQList)
            LL1Table[VNQList[i]][j] = -1;
        LL1Table[VNQList[i]]["#"] = -1;
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
