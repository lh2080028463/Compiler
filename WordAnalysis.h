#ifndef WORDANALYSIS_H
#define WORDANALYSIS_H

#endif // WORDANALYSIS_H
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

class ReToken
{
public:
    char type;//单词类别
    int number;//若存在则为表址 不存在为0
    string name;//单词名称
};

class WordAnalysis
{
public:
    static int count;
    bool clear();
    bool scan(string s, ReToken& tk);		//词法分析Token序列
    template<typename T>
    void print(vector<T> v);
    template<typename T>
    int existT(T t, vector<T> v);
    int isI(string s, int i, ReToken& tk);		//标识符判断
    int isC(string s, int i, ReToken& tk);
    int isP(string s, int i, ReToken& tk);
    int isS(string s, int i, ReToken& tk);
    int jmpBlank(string s, int i);
    float expOp(int start, int end, int loc, char w, string s, int n_ope);			//指数处理
    int val(char ch);
};
