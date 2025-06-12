#include "createquat.h"
#include<QStack>
#include<QFile>
#include<QTextStream>
#include<QDebug>

// 定义一个长度为 100 的四元式数组，用于存储生成的四元式
quat qt[100];//四元式区
// ttable 数组用于存储临时变量名，tnow 用于记录当前的临时变量名
QString ttable[100], tnow;//t表,当前t
// 语义栈，用于在生成四元式过程中存储中间结果和操作数
QStack<QString>sem;//语义栈

// 静态成员变量，记录四元式表的当前长度
int createquat::q = 0;//记录四元式表的长度
// 静态成员变量，作为活跃变量的 t 后面跟着的数字，用于生成唯一的临时变量名
int createquat::t = 0;//作为活跃变量的t后面跟着的数字，用作记录

/**
 * @brief 清空四元式生成器的状态
 *
 * 该函数将当前临时变量名清空，同时将四元式表长度和临时变量编号重置为 0。
 */
void createquat::clear()
{
    tnow.clear();
    createquat::q = 0;
    createquat::t = 0;
}

/**
 * @brief 根据传入的操作符执行相应的四元式生成动作
 *
 * 根据 top 参数的值，调用不同的成员函数来生成对应的四元式。
 *
 * @param top 操作符标识，用于判断要执行的操作类型
 * @param temppush 辅助参数，在某些操作（如入栈）中使用
 */
void createquat::action(QString top, QString temppush)
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

/**
 * @brief 生成加法运算的四元式
 *
 * 从语义栈中弹出两个操作数，生成一个加法运算的四元式，
 * 并将结果存储在一个新的临时变量中，最后将该临时变量压入语义栈。
 */
void createquat::add()
{
    int ti, qi;
    // 生成新的临时变量编号
    ti = t + 1;
    // 获取当前四元式的索引
    qi = q;
    // 设置四元式的操作符为加法
    qt[qi].op = "+";
    // 从语义栈中弹出第二个操作数
    qt[qi].arg2 = sem.top();
    sem.pop();
    // 从语义栈中弹出第一个操作数
    qt[qi].arg1 = sem.top();
    sem.pop();

    // 生成新的临时变量名
    tnow = QString("t%1").arg(ti);

    // 将新的临时变量名存入 t 表
    ttable[ti - 1] = tnow;
    // 设置四元式的结果为新的临时变量名
    qt[qi].result = tnow;

    // 将新的临时变量名压入语义栈
    sem.push(tnow);//t进栈
    // 四元式索引加 1
    qi++;
    // 更新临时变量编号
    t = ti;
    // 更新四元式表长度
    q = qi;
}

/**
 * @brief 生成减法运算的四元式
 *
 * 从语义栈中弹出两个操作数，生成一个减法运算的四元式，
 * 并将结果存储在一个新的临时变量中，最后将该临时变量压入语义栈。
 */
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

    tnow = QString("t%1").arg(ti);

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

/**
 * @brief 生成乘法运算的四元式
 *
 * 从语义栈中弹出两个操作数，生成一个乘法运算的四元式，
 * 并将结果存储在一个新的临时变量中，最后将该临时变量压入语义栈。
 */
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

    tnow = QString("t%1").arg(ti);

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

/**
 * @brief 生成除法运算的四元式
 *
 * 从语义栈中弹出两个操作数，生成一个除法运算的四元式，
 * 并将结果存储在一个新的临时变量中，最后将该临时变量压入语义栈。
 */
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

    tnow = QString("t%1").arg(ti);

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

/**
 * @brief 将常数或标识符压入语义栈
 *
 * @param str 要压入语义栈的常数或标识符
 */
void createquat::pushi(QString str)//算术表达式常数、标志符进栈，在此之前str保存当前所需压入的nowWord.s
{
    sem.push(str);
}

/**
 * @brief 生成赋值操作的四元式
 *
 * 从语义栈中弹出操作数，生成一个赋值操作的四元式。
 */
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

/**
 * @brief 生成 if 语句的四元式
 *
 * 从语义栈中弹出条件表达式，生成一个 if 语句的四元式。
 */
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

/**
 * @brief 生成 else 语句的四元式
 *
 * 生成一个 else 语句的四元式。
 */
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

/**
 * @brief 生成 if 语句结束的四元式
 *
 * 生成一个 if 语句结束的四元式。
 */
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

/**
 * @brief 生成 while 语句的四元式
 *
 * 生成一个 while 语句的四元式。
 */
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

/**
 * @brief 生成 do 语句的四元式
 *
 * 从语义栈中弹出操作数，生成一个 do 语句的四元式。
 */
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

/**
 * @brief 生成 while 语句结束的四元式
 *
 * 生成一个 while 语句结束的四元式。
 */
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

/**
 * @brief 生成比较语句的四元式
 *
 * 从语义栈中弹出操作数和比较操作符，生成一个比较语句的四元式，
 * 并将结果存储在一个新的临时变量中，最后将该临时变量压入语义栈。
 */
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
    tnow = QString("t%1").arg(ti);

    ttable[ti - 1] = tnow;
    qt[qi].result = tnow;

    sem.push(tnow);//t进栈
    qi++;
    t = ti;
    q = qi;
}

/**
 * @brief 生成函数声明的四元式
 *
 * 从语义栈中弹出函数名，生成一个函数声明的四元式。
 */
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

/**
 * @brief 生成函数结束的四元式
 *
 * 生成一个函数结束的四元式。
 */
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

/**
 * @brief 生成函数返回语句的四元式
 *
 * 从语义栈中弹出返回值，生成一个函数返回语句的四元式。
 */
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

/**
 * @brief 将生成的四元式表输出到文件
 *
 * 打开指定文件，将四元式表中的内容按格式写入文件，最后关闭文件并输出提示信息。
 */
void createquat::PRINT()//打印四元式表
{
    QFile File("TextFile/Quaternion.txt");
    if (!File.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&File);
    int qi = 0;
    while (qi != q)
    {
        out << "(" << qi + 1 << ")" << "("
            << qt[qi].op << ", "
            << qt[qi].arg1 << ", "
            << qt[qi].arg2 << ", "
            << qt[qi].result << ")" << "\n";
        qi++;
    }
    File.close();

    qDebug() << "四元式已输出. . .";
}
