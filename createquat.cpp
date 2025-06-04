#include "createquat.h"
#include <iostream>
#include <list>
#include <stack>
#include <sstream>
#include <string>
#include <fstream>
using namespace std;

quat qt[100];//四元式区
string ttable[100], tnow;//t表,当前t
stack<string>sem;//语义栈

int createquat::q = 0;//记录四元式表的长度
int createquat::t = 0;//作为活跃变量的t后面跟着的数字，用作记录

void createquat::clear()
{
    tnow.clear();
    createquat::q = 0;
    createquat::t = 0;
}

void createquat::action(string top, string temppush)
{
    if (top == "$PUSHi$")pushi(temppush);//入栈
    else if (top == "$GEQadd$")add();//+
    else if (top == "$GEQsub$")sub();//-
    else if (top == "$GEQmul$")mul();//*
    else if (top == "$GEQdiv$")div();///
    else if (top == "$ASSI$")assign();//=
    else if (top == "$IF$")IF();//if
    else if (top == "$IE$")IE();//ifend
    else if (top == "$EL$")EL();//else
    else if (top == "$WH$")WH();//while
    else if (top == "$DO$")DO();//do
    else if (top == "$WE$")WE();//while end
    else if (top == "$LOGIC$")logic();//> <
    else if (top == "$FUN$")FUN();//fun
    else if (top == "$RET$")RET();//ret
    else if (top == "$FUNEND$")FUNEND();//funend
}

void createquat::add()
{
    int ti, qi;
    ti = t + 1;
    qi = q;
    qt[qi].op = "+";
    qt[qi].arg2 = sem.top();
    sem.pop();
    qt[qi].arg1 = sem.top();
    sem.pop();

    tnow = "t";//生成新的t
    stringstream temp;
    temp << ti;
    tnow.append(temp.str());//t后记录数字

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

void createquat::sub()
{
    int ti, qi;
    ti = t + 1;
    qi = q;
    qt[qi].op = "-";
    qt[qi].arg2 = sem.top();
    sem.pop();
    qt[qi].arg1 = sem.top();
    sem.pop();

    tnow = "t";//生成新的t
    stringstream temp;
    temp << ti;
    tnow.append(temp.str());//t后记录数字

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

void createquat::mul()
{
    int ti, qi;
    ti = t + 1;
    qi = q;
    qt[qi].op = "*";
    qt[qi].arg2 = sem.top();
    sem.pop();
    qt[qi].arg1 = sem.top();
    sem.pop();

    tnow = "t";//生成新的t
    stringstream temp;
    temp << ti;
    tnow.append(temp.str());//t后记录数字

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

void createquat::div()
{
    int ti, qi;
    ti = t + 1;
    qi = q;
    qt[qi].op = "/";
    qt[qi].arg2 = sem.top();
    sem.pop();
    qt[qi].arg1 = sem.top();
    sem.pop();

    tnow = "t";//生成新的t
    stringstream temp;
    temp << ti;
    tnow.append(temp.str());//t后记录数字

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

void createquat::pushi(string str)//算术表达式常数、标志符进栈，在此之前str保存当前所需压入的nowWord.s
{
    sem.push(str);
}

void createquat::assign()
{
    int qi;
    qi = q;

    qt[qi].op = "=";
    qt[qi].arg1 = sem.top();
    sem.pop();
    qt[qi].arg2 = "_";
    qt[qi].result = sem.top();
    sem.push(sem.top());
    sem.pop();

    qi++;
    q = qi;
}

void createquat::IF()
{
    int qi;
    qi = q;

    qt[qi].op = "if";
    qt[qi].arg1 = sem.top();
    sem.pop();
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void createquat::EL()
{
    int qi;
    qi = q;

    qt[qi].op = "el";
    qt[qi].arg1 = "_";
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void createquat::IE()
{
    int qi;
    qi = q;

    qt[qi].op = "ie";
    qt[qi].arg1 = "_";
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void createquat::WH()
{
    int qi;
    qi = q;

    qt[qi].op = "wh";
    qt[qi].arg1 = "_";
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void createquat::DO()
{
    int qi;
    qi = q;

    qt[qi].op = "do";
    qt[qi].arg1 = sem.top();
    sem.pop();
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void createquat::WE()
{
    int qi;
    qi = q;

    qt[qi].op = "we";
    qt[qi].arg1 = "_";
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void createquat::logic()//比较语句
{
    int ti, qi;
    ti = t + 1;
    qi = q;
    qt[qi].arg2 = sem.top();
    sem.pop();
    qt[qi].op = sem.top();
    sem.pop();
    qt[qi].arg1 = sem.top();
    sem.pop();

    tnow = "t";//生成新的t
    stringstream temp;
    temp << ti;
    tnow.append(temp.str());//t后记录数字

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

void createquat::FUN()
{
    int qi;
    qi = q;

    qt[qi].op = "fun";
    qt[qi].arg1 = sem.top();
    sem.pop();
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void createquat::FUNEND()
{
    int qi;
    qi = q;

    qt[qi].op = "endfun";
    qt[qi].arg1 = "_";
    qt[qi].arg2 = "_";
    qt[qi].result = "_";

    qi++;
    q = qi;
}

void createquat::RET()
{
    int qi;
    qi = q;

    qt[qi].op = "ret";
    qt[qi].result = "_";
    qt[qi].arg2 = "_";
    qt[qi].arg1 = sem.top();;
    sem.pop();

    qi++;
    q = qi;
}

void createquat::PRINT()//打印四元式表
{
    fstream File("TextFile\\Quaternion.txt", fstream::out);
    int qi = 0;
    while (qi != createquat::q)
    {
        File << "(" << qi + 1 << ")" << "(" << qt[qi].op << ", " << qt[qi].arg1 << ", " << qt[qi].arg2 << ", " << qt[qi].result << ")" << endl;
        qi++;
    }
    File.close();
    cout << "四元式已输出. . ." << endl;
}

