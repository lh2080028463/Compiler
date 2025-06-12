#include "wordana.h"
#include<QDebug>
#include<global.h>

// 关键字集合，存储编程语言中的关键字
const QVector<QString> K = {
    "int","void","while","if","else","return","float","string",
    "char","break","const","do","struct","case","for","default"
};

// 界符集合，存储编程语言中的界符
const QVector<QString> P = {
    "-","/","(",")","==","<=","<","+","*",">","=",",",";","++",
    "{","}","\'","\"",">=","--","&&","||","<<",">>","[","]","!","!="
};

// 标识符表，存储程序中出现的标识符
QVector <QString> I;
// 常数表，存储程序中出现的常数
QVector <float> C;
// 字符或字符串常量表，存储程序中出现的字符和字符串常量
QVector <QString> S;
// Token 序列，存储词法分析得到的 Token
QVector <QString> Tokens;

// 可能组成双字符界符的字符集合
QVector <QChar> Pdouble = { '=','<','+','>','-','&','|','!' };

// WordAna 类的静态成员变量，记录当前扫描的位置
int WordAna::count = 0;
// 错误标志，1 表示出现词法分析错误
int flag = 0;

/**
 * @brief 跳过字符串中的空白字符
 *
 * 从指定位置开始，跳过字符串中的空白字符（空格、换行符、制表符），
 * 返回跳过的字符数量。
 *
 * @param s 待处理的字符串
 * @param i 起始位置
 * @return int 跳过的字符数量
 */
int WordAna::jmpBlank(QString s, int i)
{
    int j = 0;
    // 循环跳过空白字符
    while (s[i] == ' ' || s[i] == '\n' || s[i] == '\t')
    {
        i++;
        j++;
    }
    return j;
}

/**
 * @brief 清空词法分析器的状态
 *
 * 清空标识符表、常数表、Token 序列，并将扫描位置重置为 0。
 *
 * @return bool 总是返回 true，表示清空操作成功
 */
bool WordAna::clear()
{
    I.clear();
    C.clear();
    Tokens.clear();
    WordAna::count = 0;
    return true;
}

/**
 * @brief 对输入的字符串进行词法扫描
 *
 * 从指定位置开始对输入字符串进行词法扫描，识别不同类型的 Token，
 * 并将结果存储在 Token 对象中。
 *
 * @param s 待扫描的字符串
 * @param tk 用于存储扫描结果的 Token 对象
 * @return bool 扫描成功返回 true，出现错误返回 false
 */
bool WordAna::scan(QString s, Token& tk)
{
    int i = count;
    if (flag == 1)
    {
        // 输出词法分析错误信息并退出程序
        qDebug() << "错误信息：词法分析错误" << Qt::endl;
        exit(0);
    }
    else if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t')
    {
        // 跳过空白字符
        i += jmpBlank(s, i);
        if (s[i] == '#')
        {
            // 设置 Token 类型为结束符
            tk.type = TokenType::END;
            tk.index = -1;
            tk.value = "#";
        }
        else if ((s[i] <= 'Z' && s[i] >= 'A') || (s[i] <= 'z' && s[i] >= 'a') || s[i] == '_')
        {
            // 处理标识符或关键字
            i += isI(s, i, tk);
        }
        else if (s[i] <= '9' && s[i] >= '0')
        {
            // 处理常数
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
            // 处理字符常量
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
            // 处理字符串常量
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
            // 处理界符
            i += isP(s, i, tk);
    }
    else
    {
        if (s[i] == '#')
        {
            tk.type = TokenType::END;
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
    // 更新扫描位置
    count = i;
    return true;
}

/**
 * @brief 检查元素是否存在于向量中
 *
 * 遍历向量，检查指定元素是否存在于向量中，如果存在则返回其在向量中的位置（从 1 开始），
 * 否则返回 0。
 *
 * @tparam T 元素的类型
 * @param t 要检查的元素
 * @param v 向量
 * @return int 元素存在返回其位置（从 1 开始），不存在返回 0
 */
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

/**
 * @brief 识别标识符或关键字
 *
 * 从指定位置开始识别输入字符串中的标识符或关键字，
 * 如果是关键字则将其标记为关键字类型的 Token，
 * 如果是标识符则将其添加到标识符表中并标记为标识符类型的 Token。
 *
 * @param s 输入字符串
 * @param i 起始位置
 * @param tk 用于存储识别结果的 Token 对象
 * @return int 识别的字符数量
 */
int WordAna::isI(QString s, int i, Token& tk)
{
    // 参数校验
    if (i < 0 || i >= s.length()) {
        return 0;
    }

    const int start = i;

    // 使用 QChar 方法代替直接字符比较
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

        // 使用 QString 的 arg() 方法格式化字符串
        Tokens.push_back(QString("(K %1)").arg(keywordIndex));
    }
    // 处理标识符
    else {
        // 检查并添加到标识符表
        const int identIndex = existT<QString>(ident, I);
        if (identIndex == 0) {
            I.push_back(ident);
        }

        tk.type = TokenType::I;
        tk.index = existT<QString>(ident, I);
        tk.value = ident;

        Tokens.push_back(QString("(I %1)").arg(tk.index));
    }

    return i - start;
}

/**
 * @brief 识别常数
 *
 * 从指定位置开始识别输入字符串中的常数，支持十进制、十六进制和科学计数法表示的常数，
 * 将识别结果存储在 Token 对象中，并添加到常数表中。
 *
 * @param s 输入字符串
 * @param i 起始位置
 * @param tk 用于存储识别结果的 Token 对象
 * @return int 识别的字符数量，出现错误返回 -1
 */
int WordAna::isC(QString s, int i, Token& tk)
{
    int start = i;
    int flag = 0;
    QChar ope;
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
    QString temp = s.mid(start, i - start);
    if (flag == 0)
    {
        float check = temp.toFloat();
        if (!existT<float>(check, C)) // 不存在则填表
            C.push_back(check);
        QString key = QStringLiteral("(C %1)").arg(existT<float>(check, C));
        Tokens.push_back(key);
        tk.type = TokenType::C;
        tk.index = existT<float>(check, C);
        tk.value = temp;
    }
    else if (flag == 1 || flag == 2)
    {
        float check = 0;
        if (flag == 1)
            check = temp.toFloat();
        if (flag == 2)
            check = static_cast<float>(expOp(start, i, location, ope, s, n_ope));
        if (!existT<float>(check, C)) // 不存在则填表
            C.push_back(check);
        QString key = QStringLiteral("(C %1)").arg(existT<float>(check, C));
        Tokens.push_back(key);
        tk.type = TokenType::C;
        tk.index = existT<float>(check, C);
        tk.value = temp;
    }
    else if (flag == 3)
    {
        if (!existT<float>(sum, C)) // 不存在则填表
            C.push_back(sum);
        QString key = QStringLiteral("(C %1)").arg(existT<float>(sum, C));
        Tokens.push_back(key);
        tk.type = TokenType::C;
        tk.index = existT<float>(sum, C);
        tk.value = QString::number(sum);
    }
    return i - start;
}

/**
 * @brief 计算科学计数法表示的常数的值
 *
 * 根据输入的起始位置、结束位置、指数位置、操作符等信息，计算科学计数法表示的常数的值。
 *
 * @param start 起始位置
 * @param end 结束位置
 * @param loc 指数位置
 * @param w 操作符（+ 或 -）
 * @param s 输入字符串
 * @param n_ope 操作符标志
 * @return float 计算得到的常数值
 */
float WordAna::expOp(int start, int end, int loc, QChar w, QString s, int n_ope)
{
    int front = 0; // 记录整数部分长度
    int behind = 0; // 记录小数部分长度
    int baseindex = 0; // 基数

    for (int j = start; ; j++)
    {
        if (s[j] == '.')
        {
            front = j - start;
            behind = loc - j - 1;
            QString temp = s.mid(start, loc - start);
            baseindex = static_cast<int>(temp.toDouble() * qPow(10, behind));
            break;
        }
        if (s[j] == 'e')
        {
            QString temp = s.mid(start, loc - start);
            baseindex = static_cast<int>(temp.toDouble() * qPow(10, behind));
            break;
        }
    }

    QString temp = s.mid(loc + 1 + n_ope, end - loc - 1 - n_ope);
    int times = 0;

    if (w == '+')
        times = temp.toInt() - behind;
    if (w == '-')
        times = temp.toInt() * (-1) - behind;

    return static_cast<float>(baseindex * qPow(10, times));
}

/**
 * @brief 识别界符
 *
 * 从指定位置开始识别输入字符串中的界符，处理单字符和双字符界符，
 * 将识别结果存储在 Token 对象中，并添加到 Token 序列中。
 *
 * @param s 输入字符串
 * @param i 起始位置
 * @param tk 用于存储识别结果的 Token 对象
 * @return int 识别的字符数量（1 或 2）
 */
int WordAna::isP(QString s, int i, Token& tk)
{
    int re = 0;
    if (!existT<QChar>(s[i], Pdouble))
    {
        QString str = s.mid(i, 1);
        QString key = QStringLiteral("(P %1)").arg(existT<QString>(str, P));
        Tokens.push_back(key);
        tk.type = TokenType::P;
        tk.index = existT<QString>(str, P);
        tk.value = str;
        re = 1;
    }
    else
    {
        QString st = s.mid(i, 2);
        if (!existT<QString>(st, P))
        {
            QString str = s.mid(i, 1);
            QString key = QStringLiteral("(P %1)").arg(existT<QString>(str, P));
            Tokens.push_back(key);
            tk.type = TokenType::P;
            tk.index = existT<QString>(str, P);
            tk.value = str;
            re = 1;
        }
        else
        {
            QString key = QStringLiteral("(P %1)").arg(existT<QString>(st, P));
            Tokens.push_back(key);
            tk.type = TokenType::P;
            tk.index = existT<QString>(st, P);
            tk.value = st;
            re = 2;
        }
    }
    return re;
}

/**
 * @brief 将十六进制字符转换为对应的整数值
 *
 * 将十六进制字符（0 - 9, a - f, A - F）转换为对应的整数值。
 *
 * @param ch 十六进制字符
 * @return int 对应的整数值
 */
int WordAna::val(QChar ch)
{
    // 使用 Unicode 感知的字符检查方法
    if (ch.isDigit()) {
        return ch.digitValue(); // 直接使用 Qt 提供的数字转换
    }

    // 处理十六进制字母 (不区分大小写)
    if (ch >= u'a' && ch <= u'f') {
        return 10 + (ch.toLower().unicode() - u'a');
    }
    if (ch >= u'A' && ch <= u'F') {
        return 10 + (ch.toUpper().unicode() - u'A');
    }

    return 0;
}

/**
 * @brief 识别字符或字符串常量
 *
 * 从指定位置开始识别输入字符串中的字符或字符串常量，
 * 将识别结果存储在 Token 对象中，并添加到字符或字符串常量表中。
 *
 * @param s 输入字符串
 * @param i 起始位置
 * @param tk 用于存储识别结果的 Token 对象
 * @return int 识别的字符数量，出现错误返回 -1
 */
int WordAna::isS(QString s, int i, Token& tk)
{
    int start = i;
    if (s[i] == u'\'') // 字符常量 (使用 Unicode 字符字面量)
    {
        i++;
        i++;
        if (s[i] != u'\'') // 检查结束引号
        {
            qDebug() << "错误：未闭合的字符常量";
            return -1;
        }
        i++;
    }
    else if (s[i] == u'\"') // 字符串常量
    {
        i++;
        while (i < s.length() && s[i] != u'\"') // 添加边界检查
            i++;

        if (i >= s.length()) // 检查是否找到闭合引号
        {
            qDebug() << "错误：未闭合的字符串常量";
            return -1;
        }
    }

    // 提取内容并处理
    QString temp = s.mid(start + 1, i - start - 1); // 使用 mid() 替代 substr()

    if (!existT<QString>(temp, S)) // 不存在则填表
        S.push_back(temp);

    // 使用 QStringLiteral 和 arg() 进行高效格式化
    QString key = QStringLiteral("(S %1)").arg(existT<QString>(temp, S));
    Tokens.push_back(key);

    tk.type = TokenType::S;
    tk.index = existT<QString>(temp, S);
    tk.value = temp;
}
