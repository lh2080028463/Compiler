#include "WordAnalysis.h"
#include <iostream>
#include <cmath>
using namespace std;

vector <string> K = { "int","void","while","if","else","return","float","string",
                    "char","break","const","do","struct","case","for","default" };		//关键字表
vector <string> P = { "-","/","(",")","==","<=","<","+","*",">","=",",",";","++",
                    "{","}","\'","\"",">=","--","&&","||","<<",">>","[","]","!","!=" };		//界符表
vector <char> Pdouble = { '=','<','+','>','-','&','|','!' };								//多字符界符开头符号表

vector <string> I;				//标识符表
vector <float> C;				//常数表
vector <string> S;				//字符或字符串常量表
vector <string> Token;			//Token序列

int WordAnalysis::count = 0;
int flag = 0;

int WordAnalysis::jmpBlank(string s, int i)
{
    int j = 0;
    while (s[i] == ' ' || s[i] == '\n' || s[i] == '\t')
    {
        i++;
        j++;
    }
    return j;
}

bool WordAnalysis::clear()
{
    I.clear();
    C.clear();
    Token.clear();
    WordAnalysis::count = 0;
    return true;
}

bool WordAnalysis::scan(string s, ReToken& tk)
{
    int i = count;
    if (flag == 1)
    {
        cout << "错误信息：词法分析错误" << endl;
        exit(0);
    }
    else if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t')
    {
        i += jmpBlank(s, i);
        if (s[i] == '#')
        {
            cout << "Token:";
            print(Token);
            cout << endl;
            cout << "K:";
            print(K);
            cout << endl;
            cout << "P:";
            print(P);
            cout << endl;
            cout << "I:";
            print(I);
            cout << endl;
            cout << "C:";
            print(C);
            cout << endl;
            cout << "S:";
            print(S);
            cout << endl;
            cout << endl;
            tk.type = 'O';
            tk.number = -1;
            tk.name = "#";
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
            tk.type = 'O';
            tk.number = -1;
            tk.name = "#";
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
int WordAnalysis::existT(T t, vector<T> v)
{
    for (int i = 0; i < (int)v.size(); i++)
    {
        if (t == v[i])
            return i + 1;//存在 返回表中地址地址
    }
    return 0;//不存在 返回0
}

int WordAnalysis::isI(string s, int i, ReToken& tk)		//标识符判断
{
    int start = i;
    while ((s[i] <= 'Z' && s[i] >= 'A') || (s[i] <= 'z' && s[i] >= 'a') || (s[i] == '_') || (s[i] <= '9' && s[i] >= '0'))
        i++;
    string str = s.substr(start, i - start);
    if (existT<string>(str, K) != 0)//是否为关键字
    {
        string temp = "(K " + to_string(existT<string>(str, K)) + ')';
        Token.push_back(temp);
        tk.type = 'K';
        tk.number = existT<string>(str, K);
        tk.name = str;
    }
    else//标识符查填表
    {
        if (!existT<string>(str, I))//不存在则填表
            I.push_back(str);
        string temp = "(I " + to_string(existT<string>(str, I)) + ')';
        Token.push_back(temp);
        tk.type = 'I';
        tk.number = existT<string>(str, I);
        tk.name = str;
    }
    return i - start;
}

int WordAnalysis::isC(string s, int i, ReToken& tk)
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
                cout << "错误";
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
                    cout << "错误";
                    return -1;
                }
            }
            i++;
        }
    }
    string temp = s.substr(start, i - start);
    if (flag == 0)
    {
        float check = (float)atoi(temp.c_str());
        if (!existT<float>(check, C))//不存在则填表
            C.push_back(check);
        string key = "(C " + to_string(existT<float>(check, C)) + ')';
        Token.push_back(key);
        tk.type = 'C';
        tk.number = existT<float>(check, C);
        tk.name = temp;
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
        string key = "(C " + to_string(existT<float>(check, C)) + ')';
        Token.push_back(key);
        tk.type = 'C';
        tk.number = existT<float>(check, C);
        tk.name = temp;
    }
    else if (flag == 3)
    {
        if (!existT<float>(sum, C))//不存在则填表
            C.push_back(sum);
        string key = "(C " + to_string(existT<float>(sum, C)) + ')';
        Token.push_back(key);
        tk.type = 'C';
        tk.number = existT<float>(sum, C);
        tk.name = to_string(sum);
    }
    return i - start;
}
float WordAnalysis::expOp(int start, int end, int loc, char w, string s, int n_ope)
{
    int front = 0; //记录整数部分长度
    int behind = 0;//记录小数部分长度
    int basenumber = 0;//基数
    for (int j = start; ; j++)
    {
        if (s[j] == '.')
        {
            front = j - start;
            behind = loc - j - 1;
            string temp = s.substr(start, loc - start);
            basenumber = (int)(atof(temp.c_str()) * pow(10, behind));
            break;
        }
        if (s[j] == 'e')
        {
            string temp = s.substr(start, loc - start);
            basenumber = (int)(atof(temp.c_str()) * pow(10, behind));
            break;
        }
    }
    string temp = s.substr(loc + 1 + n_ope, end - loc - 1 - n_ope);
    int times = 0;
    if (w == '+')
        times = atoi(temp.c_str()) - behind;
    if (w == '-')
        times = atoi(temp.c_str()) * (-1) - behind;
    return (float)(basenumber * pow(10, times));
}

int WordAnalysis::isP(string s, int i, ReToken& tk)
{
    int re = 0;
    if (!existT<char>(s[i], Pdouble))
    {
        string str = s.substr(i, 1);
        string key = "(P " + to_string(existT<string>(str, P)) + ')';
        Token.push_back(key);
        tk.type = 'P';
        tk.number = existT<string>(str, P);
        tk.name = str;
        re = 1;
    }
    else
    {
        string st = s.substr(i, 2);
        if (!existT<string>(st, P))
        {
            string str = s.substr(i, 1);
            string key = "(P " + to_string(existT<string>(str, P)) + ')';
            Token.push_back(key);
            tk.type = 'P';
            tk.number = existT<string>(str, P);
            tk.name = str;
            re = 1;
        }
        else
        {
            string key = "(P " + to_string(existT<string>(st, P)) + ')';
            Token.push_back(key);
            tk.type = 'P';
            tk.number = existT<string>(st, P);
            tk.name = st;
            re = 2;
        }
    }
    return re;
}

template<typename T>
void WordAnalysis::print(vector<T> v)
{
    for (int i = 0; i < v.size(); i++)
    {
        cout << v[i];
        if (i != v.size() - 1 && count != 1)
            cout << ",";
        else
            cout << endl;
    }
}

int WordAnalysis::val(char ch)
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

int WordAnalysis::isS(string s, int i, ReToken& tk)
{
    int start = i;
    if (s[i] == '\'')//字符常量
    {
        i++;
        i++;
        if (s[i] != '\'')
        {
            cout << "错误";
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
    string temp = s.substr(start + 1, i - start - 1);
    if (!existT<string>(temp, S))//不存在则填表
        S.push_back(temp);
    string key = "(S " + to_string(existT<string>(temp, S)) + ')';
    Token.push_back(key);
    tk.type = 'S';
    tk.number = existT<string>(temp, S);
    tk.name = temp;
}
