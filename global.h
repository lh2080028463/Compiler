#ifndef GLOBAL_H
#define GLOBAL_H

#include <QVector>
#include <QString>
#include <QChar>
#include <QStringView>

using namespace std;

// Token类型
enum class TokenType {
    K,   // 关键字
    P,   // 界符 
    C1,  // 常整数
    C2,  // 常实数
    CT,  // 字符常量
    ST,
    O,
    I,// 标识符
    C,//无符号整数
    S,// 字符串常量
    UNKNOWN
};

// Token结构体
struct Token {
    TokenType type;
    QString value;
    int index;

};

// 关键字集合
const QVector<QString> K = {
    "int","void","while","if","else","return","float","string",
    "char","break","const","do","struct","case","for","default"
};

// 界符集合
const QVector<QString> P = {
    "-","/","(",")","==","<=","<","+","*",">","=",",",";","++",
    "{","}","\'","\"",">=","--","&&","||","<<",">>","[","]","!","!="
};

QVector <QString> I;				//标识符表
QVector <float> C;                  //常数表
QVector <QString> S;				//字符或字符串常量表
QVector <QString> Tokens;			//Token序列

QVector <QChar> Pdouble = { '=','<','+','>','-','&','|','!' };

#endif // GLOBAL_H
