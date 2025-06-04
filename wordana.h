#ifndef WORDANA_H
#define WORDANA_H

#include<global.h>

class WordAna
{
public:
    WordAna();
    static int count;
    bool clear();
    bool scan(QString s, Token& tk);		//词法分析Token序列
    template<typename T>
    void print(QVector<T> v);
    template<typename T>
    int existT(T t, QVector<T> v);
    int isI(QString s, int i, Token& tk);		//标识符判断
    int isC(QString s, int i, Token& tk);
    int isP(QString s, int i, Token& tk);
    int isS(QString s, int i, Token& tk);
    int jmpBlank(QString s, int i);
    float expOp(int start, int end, int loc, char w, QString s, int n_ope);	//指数处理
    int val(char ch);
};

#endif // WORDANA_H
