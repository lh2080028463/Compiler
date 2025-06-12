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
    I,// 标识符
    C,//无符号整数
    S,// 字符串常量
    END,
    UNKNOWN
};

// Token结构体
struct Token {
    TokenType type;
    QString value;
    int index;
};


#endif // GLOBAL_H
