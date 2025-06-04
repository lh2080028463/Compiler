#ifndef GLOBAL_H
#define GLOBAL_H

#include <QVector>
#include <QString>
#include <QChar>
#include <QStringView>

// Token类型
enum class TokenType {
    K,   // 关键字
    P,   // 界符
    I,   // 标识符
    C1,  // 常整数
    C2,  // 常实数
    CT,  // 字符常量
    ST,  // 字符串常量
    UNKNOWN
};

// Token结构体
struct Token {
    TokenType type;
    QString value;
    int index;

    Token(TokenType t, const QString& v, int i) : type(t), value(v), index(i) {}
};

// 关键字集合
const QVector<QLatin1String> keywords = {
    QLatin1String("program"), QLatin1String("var"), QLatin1String("integer"), QLatin1String("real"),
    QLatin1String("char"), QLatin1String("begin"), QLatin1String("end")
};

// 界符集合
const QVector<QLatin1String> delimiters = {
    QLatin1String(","), QLatin1String(":"), QLatin1String(";"), QLatin1String(":="),
    QLatin1String("*"), QLatin1String("/"), QLatin1String("+"), QLatin1String("-"),
    QLatin1String("."), QLatin1String("("), QLatin1String(")")
};

#endif // GLOBAL_H
