#ifndef SCANNER_H
#define SCANNER_H

#include <QVector>
#include <QString>
#include <QChar>
#include <QStringView>
#include <QDebug>
#include <algorithm>


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
    QLatin1String("int"), QLatin1String("void"), QLatin1String("break"), QLatin1String("float"),
    QLatin1String("while"), QLatin1String("do"), QLatin1String("struct"), QLatin1String("const"),
    QLatin1String("case"), QLatin1String("for"), QLatin1String("return"), QLatin1String("if"),
    QLatin1String("default"), QLatin1String("else")
};

// 界符集合
const QVector<QLatin1String> delimiters = {
    QLatin1String("-"), QLatin1String("/"), QLatin1String("("), QLatin1String(")"),
    QLatin1String("=="), QLatin1String("<="), QLatin1String("<"), QLatin1String("+"),
    QLatin1String("*"), QLatin1String(">"), QLatin1String("="), QLatin1String(","),
    QLatin1String(";"), QLatin1String("++"), QLatin1String("{"), QLatin1String("}")
};

// 扫描器
class Scanner {
public:
    Scanner(const QString& input) : input(input), pos(0) {}

    // 获取下一个Token
    Token getNextToken();


    // 获取所有Token
    QVector<Token> getAllTokens();

    // 获取标识符表
    const QVector<QString>& getITable() const {
        return ITable;
    }

    // 获取常整数表
    const QVector<QString>& getC1Table() const {
        return C1Table;
    }

    // 获取常实数表
    const QVector<QString>& getC2Table() const {
        return C2Table;
    }

    // 获取字符串常量表
    const QVector<QString>& getSTable() const {
        return STable;
    }

    // 获取字符常量表
    const QVector<QString>& getCTable() const {
        return CTable;
    }

private:
    QString input;
    int pos;
    QVector<QString> ITable;
    QVector<QString> C1Table;
    QVector<QString> C2Table;
    QVector<QString> STable;
    QVector<QString> CTable;

    // 跳过空白字符
    void skipWhitespace();


    // 判断是否为界符
    bool isDelimiter(QChar c) const;


    // 获取界符索引
    int getDelimiterIndex(const QString& value) const;

    // 获取关键字索引
    int getKeywordIndex(const QString& value) const;

    // 读取界符
    Token readDelimiter(int startPos);

    //读取数字
    Token readNumber(int startPos);

    // 读取标识符或关键字
    Token readIdentifierOrKeyword(int startPos);

    // 读取字符串常量
    Token readStringLiteral();

    // 读取字符常量
    Token readCharLiteral();

    // 将标识符添加到标识符表中并返回索引
    int addIdentifier(const QString& value);

    // 将常数添加到常数表中并返回索引
    int addConstant(const QString& value, QVector<QString>& table) {
        auto it = std::find(table.begin(), table.end(), value);
        if (it != table.end()) {
            return std::distance(table.begin(), it) + 1;
        }
        else {
            table.push_back(value);
            return table.size();
        }
    }
};

#endif // SCANNER_H
