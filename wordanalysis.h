#ifndef WORDANALYSIS_H
#define WORDANALYSIS_H
#include<QString>
#include<QVector>
class ReToken
{
public:
    char type;//单词类别
    int number;//若存在则为表址 不存在为0
    QString name;//单词名称
};
class WordAnalysis
{
public:
    WordAnalysis();
    static int count;
    bool clear();
    bool scan(QString s, ReToken& tk);		//词法分析Token序列
    template<typename T>
    void print(QVector<T> v);
    template<typename T>
    int existT(T t, QVector<T> v);
    int isI(QString s, int i, ReToken& tk);		//标识符判断
    int isC(QString s, int i, ReToken& tk);
    int isP(QString s, int i, ReToken& tk);
    int isS(QString s, int i, ReToken& tk);
    int jmpBlank(QString s, int i);
    float expOp(int start, int end, int loc, char w, QString s, int n_ope);			//指数处理
    int val(char ch);
};

#endif // WORDANALYSIS_H
