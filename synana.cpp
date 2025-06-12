#include "synana.h"

#include<QDebug>
#include<QFile>

using namespace std;
// 非终结符列表，存储文法中的非终结符
QVector<QString> VNQList;
// 终结符列表，存储文法中的终结符
QVector<QString> VTQList;
// LL(1) 分析表，格式为 {"E", {{"i", 1}, {"(", 1}}}
// "E" 为行表头（非终结符），"i" 和 "(" 为列表头（终结符），1 为该表项内容（对应的产生式序号）
QMap<QString, QMap<QString, int>> LL1Table;
// grammar 按序号存储所有产生式右部
QVector<QVector<QString>> grammarRight;

/**
 * @brief 对输入的程序进行语法分析
 *
 * 该函数使用 LL(1) 分析方法对输入的程序进行语法分析，
 * 过程中会读取文法文件生成 LL(1) 分析表，同时处理符号表和四元式的生成。
 *
 * @param program 待分析的程序代码
 * @return bool 若语法分析成功返回 true，否则返回 false
 */
bool SynAna::AN(QString program)
{
    extern QString err_inf;
    // 清空非终结符列表、终结符列表、LL(1) 分析表和产生式右部列表
    VNQList.clear(); VTQList.clear();
    LL1Table.clear();
    grammarRight.clear();
    // 分析栈，用于模拟 LL(1) 分析过程
    QList<QString> stack;
    // 栈底符号入栈
    stack.push_back("#");
    // 开始符号入栈
    stack.push_back("<函数>");
    // 栈顶符号
    QString topSymbol;
    // 现在分析的单词
    Token nowWord;
    Token lastWord;
    // 生成 LL(1) 分析表
    GetLL1Table();

    SymbolTable symTable;
    createquat quat;
    // 清空四元式生成器
    quat.clear();
    WordAna wordAna;
    // 清空词法分析器
    wordAna.clear();
    // 进行词法分析，获取第一个单词
    wordAna.scan(program, nowWord);
    //qDebug() << nowWord.value ;
    //qDebug() << "1" ;
    while (1)
    {
        // 获取栈顶符号
        topSymbol = stack.back();
        // 弹出栈顶符号
        stack.pop_back();
        // 如果栈顶符号是终结符
        if (isVT(topSymbol))
        {
            if (topSymbol == nowWord.value
                || (topSymbol == "<标识符>" && nowWord.type == TokenType::I)
                || (topSymbol == "<无符号整数>" && nowWord.type == TokenType::C))
            {
                lastWord = nowWord;
                //qDebug() << lastWord.value ;
                // 继续进行词法分析，获取下一个单词
                wordAna.scan(program, nowWord);
                //qDebug()<< nowWord.value ;

            }
            else
            {
                qDebug()<<"语法错误！请检查！";
                qDebug() << "错误信息：" + nowWord.value;
                err_inf+="语法错误！请检查！\n错误信息：";
                err_inf+=nowWord.value;
                return false;
            }
        }
        // 如果栈顶符号是非终结符
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
                qDebug() << "语法错误！请检查！" ;
                qDebug() << "错误信息：" + nowWord.value;
                err_inf+="语法错误！请检查！\n错误信息：";
                err_inf+=nowWord.value;
                return false;
            }
            else
            {
                // 产生式右部逆序压栈
                for (auto i = grammarRight[graNum].rbegin(); i != grammarRight[graNum].rend(); i++)
                    if (*i != "<空>") stack.push_back(*i);
            }
        }
        // 如果栈顶符号是符号表填写动作符号
        else if (isSyTb(topSymbol))
        {
            // 调用符号表的相应动作
            symTable.callAction(topSymbol, nowWord);
        }
        else if (isQuat(topSymbol))
        {
            // 调用四元式生成器的相应动作
            quat.action(topSymbol, lastWord.value);
        }
        // 如果栈顶符号是 "#"
        else if (topSymbol == "#")
        {
            if (nowWord.value == "#")
            {
                qDebug() << "语法分析已完成！" ;
                //wordAna.print(Token);
                break;
            }
            else
            {
                qDebug() << "语法错误！请检查！" ;
                qDebug() << "错误信息：已读取所有源码，语法分析器未成功结束！" ;
                err_inf+="语法错误！请检查！\n错误信息：已读取所有源码，语法分析器未成功结束！";
                return false;
                //exit(1);
            }
        }
        else
        {
            qDebug() << "语法错误！请检查！" ;
            qDebug() << "错误信息：未知错误！" ;
            err_inf+="语法错误！请检查！\n错误信息：未知错误！";
            qDebug() << topSymbol ;
            return false;
            //exit(1);
        }
    }

    // 输出符号表
    symTable.printTable();
    // 输出四元式
    quat.PRINT();

    return true;
}

/**
 * @brief 判断一个字符串是否为非终结符
 *
 * @param str 待判断的字符串
 * @return bool 若是非终结符返回 true，否则返回 false
 */
bool SynAna::isVN(QString str)
{
    if (find(VNQList.begin(), VNQList.end(), str) == VNQList.end())
        return false;
    else
        return true;
}

/**
 * @brief 判断一个字符串是否为终结符
 *
 * @param str 待判断的字符串
 * @return bool 若是终结符返回 true，否则返回 false
 */
bool SynAna::isVT(QString str)
{
    if (find(VTQList.begin(), VTQList.end(), str) == VTQList.end())
        return false;
    else
        return true;
}

/**
 * @brief 判断一个字符串是否为四元式相关符号
 *
 * 以 $ 开头且不为 $ 的字符串被认为是四元式相关符号
 *
 * @param str 待判断的字符串
 * @return bool 若是四元式相关符号返回 true，否则返回 false
 */
bool SynAna::isQuat(QString str)
{
    if (str[0] == '$' && str != "$")return true;
    else return false;
}

/**
 * @brief 判断一个字符串是否为符号表填写动作符号
 *
 * 以 " 开头且不为 " 的字符串被认为是符号表填写动作符号
 *
 * @param str 待判断的字符串
 * @return bool 若是符号表填写动作符号返回 true，否则返回 false
 */
bool SynAna::isSyTb(QString str)
{
    if (str[0] == '\"' && str != "\"")return true;
    else return false;
}

/**
 * @brief 分析文法，生成 LL(1) 分析表
 *
 * 该函数读取文法文件，生成非终结符、终结符、产生式右部集合，
 * 求取 First 集、Follow 集、Select 集，判定文法是否为 LL(1) 文法，
 * 若是则生成 LL(1) 分析表并输出到文件。
 *
 * 对所分析文法的格式要求：
 * 1. 所有非终结符用 <> 括起来
 * 2. 任意符号之间用空格分开
 * 3. 每行第一个符号为产生式的非终结符，后面是右部，不需写 ->，右部之间用 '|' 分隔
 * 4. 每行以 '#' 结尾
 *
 * @return bool 若成功生成 LL(1) 分析表返回 true，否则返回 false
 */
bool SynAna::GetLL1Table()
{
    QVector<QVector<QString>> grammar;
    QVector<QVector<int>> rightQList;
    QVector<int> leftQList;
    QString temp;
    QVector<QString> tempRight, tempRightAll;
    // 清空非终结符列表和终结符列表
    VNQList.clear();
    VTQList.clear();

    // 打开文法文件
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
        // 将非终结符添加到非终结符列表
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
                        // 将终结符添加到终结符列表
                        VTQList.push_back(temp);
                }
                if ((temp[0] != '\"' && temp[0] != '$') || temp == "\"")
                    tempRight.push_back(temp);
                tempRightAll.push_back(temp);
            }
        }
    }
    // 添加特殊终结符
    VTQList.push_back("<标识符>");
    VTQList.push_back("<无符号整数>");
    file.close();

    // 打开文件，将非终结符、终结符和产生式右部信息写入文件
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

    // 求 First 集
    QMap<QString, QVector<QString>> FIRST_VNVT;
    QVector<QVector<QString>> FIRST(grammar.size());

    // 终结符 First 集为本身
    for (const auto& vt : VTQList)
        FIRST_VNVT[vt] = {vt};

    // 非终结符 First 集为空
    for (const auto& vn : VNQList)
        FIRST_VNVT[vn] = {};

    bool change = true;
    while (change) {
        change = false;
        int m = 0;
        // 对每一个产生式 A->b
        for (auto j = grammar.begin(); j != grammar.end(); j++, m++)
        {
            // b=b1b2b3..., bi 是 VN | VT, tempFIRST_VNVT <- First(b1) - {空}
            QVector<QString> tempFIRST_VNVT;
            tempFIRST_VNVT= FIRST_VNVT[*(j->begin())];
            auto nullItera = find(tempFIRST_VNVT.begin(), tempFIRST_VNVT.end(), "<空>");
            if (nullItera != tempFIRST_VNVT.end())tempFIRST_VNVT.erase(nullItera);
            int i = 1;
            // 当 First(b0...bi-1) 包含空，即 b0 到 bi-1 都可为空，tempFIRST_VNVT += First(bi)
            while (find(FIRST_VNVT[(*j)[i - 1]].begin(), FIRST_VNVT[(*j)[i - 1]].end(), "<空>")
                       != FIRST_VNVT[(*j)[i - 1]].end()
                   && i <= (int)j->size() - 1)
            {
                for (auto k = FIRST_VNVT[(*j)[i]].begin(); k != FIRST_VNVT[(*j)[i]].end(); k++)
                    if (*k != "<空>") tempFIRST_VNVT.push_back(*k);
                i++;
            }
            // 如果右部可推出空，tempFIRST_VNVT += <空>
            if (i == (int)j->size() - 1
                && find(FIRST_VNVT[(*j)[i - 1]].begin(), FIRST_VNVT[(*j)[i - 1]].end(), "<空>")
                       != FIRST_VNVT[(*j)[i - 1]].end())
                tempFIRST_VNVT.push_back("<空>");
            // 如果右部=<空>，tempFIRST_VNVT <- <空>
            if (j->size() == 1 && *(j->begin()) == "<空>")
                tempFIRST_VNVT.push_back("<空>");
            // 更新左部的 First 集
            for (auto k = tempFIRST_VNVT.begin(); k != tempFIRST_VNVT.end(); k++)
            {
                QString tempVN = VNQList[leftQList[m]];
                if (find(FIRST_VNVT[tempVN].begin(), FIRST_VNVT[tempVN].end(), *k) == FIRST_VNVT[tempVN].end())
                {
                    FIRST_VNVT[tempVN].push_back(*k);
                    change = true;
                }
            }
            // 更新右部的 First 集
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

    qDebug() << "First 集：";
    for(int m=1;m<=FIRST.size();m++){
        qDebug()<<m<<": ";
        for(int m1=0;m1<FIRST[m-1].size();m1++){
            qDebug()<<FIRST[m-1][m1];
        }
    }

    // 求 Follow 集
    QMap<QString, QVector<QString>> FOLLOW;
    for (const auto& vn : VNQList)
        FOLLOW[vn] = {};
    // 开始符号的 Follow 集包含 #
    FOLLOW[VNQList.first()].push_back("#");

    change = true;
    while (change) {
        change = false;
        int m=0;
        for (auto j = grammar.begin(); j != grammar.end(); j++, m++)
        {
            // Trailer <- Follow(A)
            QVector<QString> Trailer = FOLLOW[VNQList[leftQList[m]]];
            // 从后往前遍历产生式右部
            for (auto i = j->rbegin(); i != j->rend(); i++)
            {
                // 如果 bi 是非终结符，将其后跟着的单词 first 加到 follow 中
                if (find(VNQList.begin(), VNQList.end(), *i) != VNQList.end())
                {
                    // Follow(bi) += Trailer, 存入无重复元素
                    for (auto k = Trailer.begin(); k != Trailer.end(); k++)
                    {
                        if (find(FOLLOW[*i].begin(), FOLLOW[*i].end(), *k) == FOLLOW[*i].end())
                        {
                            FOLLOW[*i].push_back(*k);// 将当前左部的加到这个元素中
                            change = true;
                        }
                    }
                    // 如果 First(bi) 包含空，Trailer += First(bi)-"<空>"
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
                    // 否则 Trailer = First(bi)
                    else
                    {
                        Trailer = FIRST_VNVT[*i];
                    }
                }
                // 如果 bi 是终结符，Trailer = First(bi)
                else
                {
                    Trailer = FIRST_VNVT[*i];
                }
            }
        }
    }

    qDebug() << "Follow 集：";
    for (const auto& vn : VNQList) {
        qDebug().noquote() << QString("%1: %2").arg(vn).arg(QStringList(FOLLOW[vn]).join(' '));
    }
    qDebug();

    // 输出 Follow 集到文件
    QFile file3("TextFile/Follow.txt");
    if (file3.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file3);
        out << "Follow 集：" << Qt::endl;

        for (auto i = VNQList.begin(); i != VNQList.end(); i++) {
            out << *i << ": ";
            for (auto j = FOLLOW[*i].begin(); j != FOLLOW[*i].end(); j++) {
                out << *j << ' ';
            }
            out << '\n';
        }

        file3.close();
    } else {
        // 处理文件打开失败的情况
        qDebug() << "无法打开文件:" << file3.errorString();
    }

    // 求 Select 集
    QVector<QVector<QString>> SELECT(grammar.size());
    for (int i = 0; i < grammar.size(); ++i) {
        // 如果 First(b) 不包含空，即右部不能推出空，select 集等于右部 first 集
        if (find(FIRST[i].begin(), FIRST[i].end(), "<空>") == FIRST[i].end())
            SELECT[i] = FIRST[i];
        // 否则，select 集等于右部 first 集并上左部 follow 集
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

    // 输出 Select 集到文件
    QFile file4("TextFile/Select.txt");
    if (file4.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file4);
        out << "Select 集：" << Qt::endl;

        int v = 1;
        for (auto i = SELECT.begin(); i != SELECT.end(); i++) {
            out << v++ << ": ";
            for (auto j = i->begin(); j != i->end(); j++) {
                out << *j << " ";
            }
            out << "\n";
        }

        file4.close();
    } else {
        qDebug() << "无法打开 Select.txt 文件:" << file4.errorString();
    }

    qDebug() << "Select 集：";
    for(int i=1;i<=SELECT.size();i++){
        qDebug()<<i<<": ";
        for(int j=0;j<SELECT[i-1].size();j++){
            qDebug()<<SELECT[i-1][j];
        }
    }

    // 检查是否为 LL(1) 文法
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
                    qDebug() << "文法非 LL(1) 文法！请检查！\n" << "提示信息：左部" << VNQList[i];
                    return false;
                }
            }
        }
    }
    qDebug() << "文法满足 LL(1) 文法要求。";

    // 构建 LL(1) 分析表
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

    // 输出 LL(1) 分析表到文件
    QFile file5("TextFile/LL1Table.txt");
    if (file5.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file5);
        out << "LL(1) 分析表\t";
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
