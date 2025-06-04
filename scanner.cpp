#include "scanner.h"


Token Scanner::getNextToken() {
    skipWhitespace();

    if (pos >= input.length()) {
        return Token(TokenType::P, QString(), -1); // 结束标记
    }

    int startPos = pos;

    if (isDelimiter(input[pos])) {
        return readDelimiter(startPos);
    }

    if (input[pos].isDigit()) {
        return readNumber(startPos);
    }

    if (input[pos].isLetter()) {
        return readIdentifierOrKeyword(startPos);
    }

    if (input[pos] == QLatin1Char('"')) {
        return readStringLiteral();
    }

    if (input[pos] == QLatin1Char('\'')) {
        return readCharLiteral();
    }

    // 未知字符
    pos++;
    return Token(TokenType::UNKNOWN, QString(), -1);
}

QVector<Token> Scanner::getAllTokens() {
    QVector<Token> tokens;
    while (pos < input.length()) {
        Token token = getNextToken();
        if (token.type == TokenType::UNKNOWN) {
            qDebug() << "ERROR";
            return {};
        }
        if (!token.value.isEmpty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

// 跳过空白字符
void Scanner::skipWhitespace() {
    while (pos < input.length() && input[pos].isSpace()) {
        pos++;
    }
}

// 判断是否为界符
bool Scanner::isDelimiter(QChar c) const {
    return std::any_of(delimiters.begin(), delimiters.end(),
                       [c](const QLatin1String& delimiter) { return delimiter[0] == c; });
}

// 获取界符索引
int Scanner::getDelimiterIndex(const QString& value) const {
    auto it = std::find(delimiters.begin(), delimiters.end(), value);
    return (it != delimiters.end()) ? std::distance(delimiters.begin(), it) + 1 : -1;
}

// 获取关键字索引
int Scanner::getKeywordIndex(const QString& value) const {
    auto it = std::find(keywords.begin(), keywords.end(), value);
    return (it != keywords.end()) ? std::distance(keywords.begin(), it) + 1 : -1;
}

// 读取界符
Token Scanner::readDelimiter(int startPos) {
    QString currentChar(input[pos]);
    QString nextChar = (pos + 1 < input.length()) ? QString(input[pos + 1]) : QString();

    if (!nextChar.isEmpty() && getDelimiterIndex(currentChar + nextChar) != -1) {
        pos += 2;
        return Token(TokenType::P, currentChar + nextChar, getDelimiterIndex(currentChar + nextChar));
    }

    if (getDelimiterIndex(currentChar) != -1) {
        pos++;
        return Token(TokenType::P, currentChar, getDelimiterIndex(currentChar));
    }

    return Token(TokenType::UNKNOWN, QString(), -1);
}

//读取数字
Token Scanner::readNumber(int startPos) {
    QString value;
    bool isFloat = false;
    bool hasDot = false;
    bool hasExponent = false;
    int originalPos = startPos;

    // 检查十六进制格式
    if (input[startPos] == QLatin1Char('0') &&
        (startPos + 1 < input.length()) &&
        (input[startPos + 1] == QLatin1Char('x') || input[startPos + 1] == QLatin1Char('X'))) {
        startPos += 2;
        QString hexValue;
        while (startPos < input.length() && input[startPos].isDigit() ||
               (input[startPos] >= QLatin1Char('a') && input[startPos] <= QLatin1Char('f')) ||
               (input[startPos] >= QLatin1Char('A') && input[startPos] <= QLatin1Char('F'))) {
            hexValue += input[startPos];
            startPos++;
        }

        pos = startPos;

        if (hexValue.isEmpty()) {
            return Token(TokenType::UNKNOWN, QString(), -1);
        }

        // 转换成十进制字符串
        bool ok;
        int decimalValue = hexValue.toInt(&ok, 16);
        if (!ok) {
            return Token(TokenType::UNKNOWN, QString(), -1);
        }
        QString decimalStr = QString::number(decimalValue);

        int index = addConstant(decimalStr, C1Table);
        return Token(TokenType::C1, decimalStr, index);
    }

    // 否则按浮点数/十进制整数处理
    while (startPos < input.length()) {
        QChar currentChar = input[startPos];

        if (currentChar.isDigit()) {
            value += currentChar;
            startPos++;
        }
        else if (currentChar == QLatin1Char('.')) {
            if (hasDot || hasExponent) break;
            hasDot = true;
            isFloat = true;
            value += currentChar;
            startPos++;
        }
        else if (currentChar == QLatin1Char('e') || currentChar == QLatin1Char('E')) {
            if (hasExponent) break;
            hasExponent = true;
            isFloat = true;
            value += currentChar;
            startPos++;
            // 检查指数的符号
            if (startPos < input.length() && (input[startPos] == QLatin1Char('+') || input[startPos] == QLatin1Char('-'))) {
                value += input[startPos];
                startPos++;
            }
            // 检查指数后是否有数字
            if (startPos >= input.length() || !input[startPos].isDigit()) {
                return Token(TokenType::UNKNOWN, input.mid(originalPos, startPos - originalPos), -1);
            }
        }
        else {
            break;
        }
    }
    pos = startPos;

    if (value.isEmpty()) {
        return Token(TokenType::UNKNOWN, QString(), -1);
    }

    if (isFloat) {
        int index = addConstant(value, C2Table);
        return Token(TokenType::C2, value, index);
    }
    else {
        int index = addConstant(value, C1Table);
        return Token(TokenType::C1, value, index);
    }
}

// 读取标识符或关键字
Token Scanner::readIdentifierOrKeyword(int startPos) {
    QString value;
    while (pos < input.length() && (input[pos].isLetterOrNumber() || input[pos] == QLatin1Char('_'))) {
        value += input[pos];
        pos++;
    }

    int index = getKeywordIndex(value);
    if (index != -1) {
        return Token(TokenType::K, value, index);
    }
    else {
        index = addIdentifier(value);
        return Token(TokenType::I, value, index);
    }
}

// 读取字符串常量
Token Scanner::readStringLiteral() {
    pos++; // 跳过开头的双引号
    QString value;
    while (pos < input.length() && input[pos] != QLatin1Char('"')) {
        value += input[pos];
        pos++;
    }
    if (pos >= input.length() || input[pos] != QLatin1Char('"')) {
        return Token(TokenType::UNKNOWN, QString(), -1);
    }
    pos++; // 跳过结尾的双引号
    int index = addConstant(value, STable);
    return Token(TokenType::ST, value, index);
}

// 读取字符常量
Token Scanner::readCharLiteral() {
    pos++; // 跳过开头的单引号
    QString value;
    if (pos < input.length() && input[pos] != QLatin1Char('\'')) {
        value += input[pos];
        pos++;
    }
    if (pos >= input.length() || input[pos] != QLatin1Char('\'')) {
        return Token(TokenType::UNKNOWN, QString(), -1);
    }
    pos++; // 跳过结尾的单引号
    int index = addConstant(value, CTable);
    return Token(TokenType::CT, value, index);
}

// 将标识符添加到标识符表中并返回索引
int Scanner::addIdentifier(const QString& value) {
    auto it = std::find(ITable.begin(), ITable.end(), value);
    if (it != ITable.end()) {
        return std::distance(ITable.begin(), it) + 1;
    }
    else {
        ITable.push_back(value);
        return ITable.size();
    }
}

// 将常数添加到常数表中并返回索引
int Scanner::addConstant(const QString& value, QVector<QString>& table) {
    auto it = std::find(table.begin(), table.end(), value);
    if (it != table.end()) {
        return std::distance(table.begin(), it) + 1;
    }
    else {
        table.push_back(value);
        return table.size();
    }
}
