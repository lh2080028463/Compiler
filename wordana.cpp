#include "wordana.h"
#include<QDebug>
#include<global.h>

int WordAna::count = 0;
int flag = 0;

int WordAna::jmpBlank(QString s, int i)
{
    int j = 0;
    while (s[i] == ' ' || s[i] == '\n' || s[i] == '\t')
    {
        i++;
        j++;
    }
    return j;
}

bool WordAna::clear()
{
    I.clear();
    C.clear();
    Tokens.clear();
    WordAna::count = 0;
    return true;
}

bool WordAna::scan(QString s, Token& tk)
{
    int i = count;
    if (flag == 1)
    {
        qDebug() << "错误信息：词法分析错误" << Qt::endl;
        exit(0);
    }
    else if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t')
    {
        i += jmpBlank(s, i);
        if (s[i] == '#')
        {
            qDebug() << "Token:";
            print(Tokens);
            qDebug() << Qt::endl;
            qDebug() << "K:";
            print(K);
            qDebug() << Qt::endl;
            qDebug() << "P:";
            print(P);
            qDebug() << Qt::endl;
            qDebug() << "I:";
            print(I);
            qDebug() << Qt::endl;
            qDebug() << "C:";
            print(C);
            qDebug() << Qt::endl;
            qDebug() << "S:";
            print(S);
            qDebug() << Qt::endl;
            qDebug() << Qt::endl;
            tk.type = TokenType::O;
            tk.index = -1;
            tk.value = "#";
        }
        else if ((s[i] <= 'Z' && s[i] >= 'A') || (s[i] <= 'z' && s[i] >= 'a') || s[i] == '_')
        {
            i += isI(s, i, tk);
        }
        else if (s[i] <= '9' && s[i] >= '0')
        {
            int existance = isC(s, i, tk);
            if (existance == -1)
            {
                flag = 1;
                return false;
            }
            else
                i += isC(s, i, tk);
        }
        else if (s[i] == '\'')
        {
            int existance = isS(s, i, tk);
            if (existance == -1)
            {
                flag = 1;
                return false;
            }
            else
                i += isS(s, i, tk);
        }
        else if (s[i] == '\"')
        {
            int existance = isS(s, i, tk);
            if (existance == -1)
            {
                flag = 1;
                return false;
            }
            else
                i += isS(s, i, tk);
        }
        else
            i += isP(s, i, tk);
    }
    else
    {
        if (s[i] == '#')
        {
            tk.type = TokenType::O;
            tk.index = -1;
            tk.value = "#";
        }
        else if ((s[i] <= 'Z' && s[i] >= 'A') || (s[i] <= 'z' && s[i] >= 'a') || s[i] == '_')
        {
            i += isI(s, i, tk);
        }
        else if (s[i] <= '9' && s[i] >= '0')
        {
            int existance = isC(s, i, tk);
            if (existance == -1)
            {
                flag = 1;
                return false;
            }
            else
                i += isC(s, i, tk);
        }
        else if (s[i] == '\'')
        {
            int existance = isS(s, i, tk);
            if (existance == -1)
            {
                flag = 1;
                return false;
            }
            else
                i += isS(s, i, tk);
        }
        else if (s[i] == '\"')
        {
            int existance = isS(s, i, tk);
            if (existance == -1)
            {
                flag = 1;
                return false;
            }
            else
                i += isS(s, i, tk);
        }
        else
            i += isP(s, i, tk);
    }
    count = i;
    return true;
}

template<typename T>
int WordAna::existT(T t, QVector<T> v)
{
    for (int i = 0; i < (int)v.size(); i++)
    {
        if (t == v[i])
            return i + 1;//存在 返回表中地址地址
    }
    return 0;//不存在 返回0
}

int WordAna::isI(QString s, int i, Token& tk)
{
    // 参数校验
    if (i < 0 || i >= s.length()) {
        return 0;
    }

    const int start = i;

    // 使用QChar方法代替直接字符比较
    while (i < s.length()) {
        const QChar c = s.at(i);
        if (!(c.isLetter() || c == '_' || (i > start && c.isDigit()))) {
            break;
        }
        ++i;
    }

    // 提取标识符子串
    const QString ident = s.mid(start, i - start);
    if (ident.isEmpty()) {
        return 0;
    }

    // 检查是否为关键字
    const int keywordIndex = existT<QString>(ident, K);
    if (keywordIndex != 0) {
        tk.type = TokenType::K;
        tk.index = keywordIndex;
        tk.value = ident;

        // 使用QString的arg()方法格式化字符串
        Token.push_back(QString("(K %1)").arg(keywordIndex));
    }
    // 处理标识符
    else {
        // 检查并添加到标识符表
        const int identIndex = existT<QString>(ident, I);
        if (identIndex == 0) {
            I.push_back(ident);
        }

        tk.type = 'I';
        tk.index = existT<QString>(ident, I);
        tk.value = ident;

        Token.push_back(QString("(I %1)").arg(tk.index));
    }

    return i - start;
}

int WordAna::isC(QString s, int i, Token& tk)
{
    int start = i;
    int flag = 0;
    char ope;
    int location;
    int n_ope = 1;
    int sum = 0;
    if (s[i] == '0' && (s[i + 1] == 'x' || s[i + 1] == 'X'))//十六进制常整数
    {
        i += 2;
        while ((s[i] <= '9' && s[i] >= '0') || (s[i] <= 'z' && s[i] >= 'a') || (s[i] <= 'Z' && s[i] >= 'A'))
        {
            sum = sum * 16 + val(s[i]);
            i++;
        }
        flag = 3;
    }
    else if (s[i] <= '9' && s[i] >= '0')
    {
        while ((s[i] <= '9' && s[i] >= '0') || (s[i] == '.') || s[i] == 'e')
        {
            if (s[i] == '.')
                flag = 1;
            if (s[i] == '.' && (s[i + 1] >= '9' || s[i + 1] <= '0'))
            {
                qDebug() << "错误";
                return -1;
            }
            if (s[i] == 'e')
            {
                location = i;
                if (s[i + 1] == '+' || s[i + 1] == '-')
                {
                    ope = s[i + 1];
                    i += 2;
                    flag = 2;
                    continue;
                }
                else if (s[i + 1] <= '9' && s[i + 1] >= '0')
                {
                    ope = '+';
                    flag = 2;
                    n_ope = 0;
                }
                else
                {
                    qDebug() << "错误";
                    return -1;
                }
            }
            i++;
        }
    }
    QString temp = s.substr(start, i - start);
    if (flag == 0)
    {
        float check = (float)atoi(temp.c_str());
        if (!existT<float>(check, C))//不存在则填表
            C.push_back(check);
        QString key = "(C " + to_QString(existT<float>(check, C)) + ')';
        Token.push_back(key);
        tk.type = 'C';
        tk.index = existT<float>(check, C);
        tk.value = temp;
    }
    else if (flag == 1 || flag == 2)
    {
        float check = 0;
        if (flag == 1)
            check = (float)atof(temp.c_str());
        if (flag == 2)
            check = (float)expOp(start, i, location, ope, s, n_ope);
        if (!existT<float>(check, C))//不存在则填表
            C.push_back(check);
        QString key = "(C " + to_QString(existT<float>(check, C)) + ')';
        Token.push_back(key);
        tk.type = 'C';
        tk.index = existT<float>(check, C);
        tk.value = temp;
    }
    else if (flag == 3)
    {
        if (!existT<float>(sum, C))//不存在则填表
            C.push_back(sum);
        QString key = "(C " + to_QString(existT<float>(sum, C)) + ')';
        Token.push_back(key);
        tk.type = 'C';
        tk.index = existT<float>(sum, C);
        tk.value = to_QString(sum);
    }
    return i - start;
}
float WordAna::expOp(int start, int end, int loc, char w, QString s, int n_ope)
{
    int front = 0; //记录整数部分长度
    int behind = 0;//记录小数部分长度
    int baseindex = 0;//基数
    for (int j = start; ; j++)
    {
        if (s[j] == '.')
        {
            front = j - start;
            behind = loc - j - 1;
            QString temp = s.substr(start, loc - start);
            baseindex = (int)(atof(temp.c_str()) * pow(10, behind));
            break;
        }
        if (s[j] == 'e')
        {
            QString temp = s.substr(start, loc - start);
            baseindex = (int)(atof(temp.c_str()) * pow(10, behind));
            break;
        }
    }
    QString temp = s.substr(loc + 1 + n_ope, end - loc - 1 - n_ope);
    int times = 0;
    if (w == '+')
        times = atoi(temp.c_str()) - behind;
    if (w == '-')
        times = atoi(temp.c_str()) * (-1) - behind;
    return (float)(baseindex * pow(10, times));
}

int WordAna::isP(QString s, int i, Token& tk)
{
    int re = 0;
    if (!existT<char>(s[i], Pdouble))
    {
        QString str = s.substr(i, 1);
        QString key = "(P " + to_QString(existT<QString>(str, P)) + ')';
        Token.push_back(key);
        tk.type = 'P';
        tk.index = existT<QString>(str, P);
        tk.value = str;
        re = 1;
    }
    else
    {
        QString st = s.substr(i, 2);
        if (!existT<QString>(st, P))
        {
            QString str = s.substr(i, 1);
            QString key = "(P " + to_QString(existT<QString>(str, P)) + ')';
            Token.push_back(key);
            tk.type = 'P';
            tk.index = existT<QString>(str, P);
            tk.value = str;
            re = 1;
        }
        else
        {
            QString key = "(P " + to_QString(existT<QString>(st, P)) + ')';
            Token.push_back(key);
            tk.type = 'P';
            tk.index = existT<QString>(st, P);
            tk.value = st;
            re = 2;
        }
    }
    return re;
}

template<typename T>
void WordAna::print(QVector<T> v)
{
    for (int i = 0; i < v.size(); i++)
    {
        qDebug() << v[i];
        if (i != v.size() - 1 && count != 1)
            qDebug() << ",";
        else
            qDebug() << Qt::endl;
    }
}

int WordAna::val(char ch)
{
    int value = 0;
    if (ch >= '0' && ch <= '9')
        value = ch - '0';
    else if (ch >= 'a' && ch <= 'f')
        value = ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
        value = ch - 'A' + 10;
    return value;
}

int WordAna::isS(QString s, int i, Token& tk)
{
    int start = i;
    if (s[i] == '\'')//字符常量
    {
        i++;
        i++;
        if (s[i] != '\'')
        {
            qDebug() << "错误";
            return -1;
        }
        i++;
    }
    else if (s[i] == '\"')//字符串常量
    {
        i++;
        while (s[i] != '\"')
            i++;
    }
    QString temp = s.substr(start + 1, i - start - 1);
    if (!existT<QString>(temp, S))//不存在则填表
        S.push_back(temp);
    QString key = "(S " + to_QString(existT<QString>(temp, S)) + ')';
    Token.push_back(key);
    tk.type = 'S';
    tk.index = existT<QString>(temp, S);
    tk.value = temp;
}
