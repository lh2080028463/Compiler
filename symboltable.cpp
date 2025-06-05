
#include "symboltable.h"
#include <sstream>
#include <fstream>
#include <iomanip>
using namespace std;
ostream& operator<<(ostream& out, const SYNBLEntry& entry)
{
    out << setw(8) << entry.name << setw(12) << entry.type
        << setw(8) << entry.category << '\t' << setw(8) << entry.address << endl;
    return out;
}

ostream& operator<<(ostream& out, const TAPELEntry& entry)
{
    out << entry.typeValue << '\t' << setw(12) << entry.typePoint << endl;
    return out;
}

ostream& operator<<(ostream& out, const PFINFLEntry& entry)
{
    out << setw(5) << entry.level << '\t' << setw(4) << entry.offset << '\t'
        << setw(8) << entry.paraNum << '\t' << setw(9) << entry.paraTable << endl;
    return out;
}

map<string, SymbolTable::Act> SymbolTable::ACTIONL =
    {
        {"\"函数名\"", &SymbolTable::func}, {"\"int形参名\"", &SymbolTable::formalParaVar},
        {"\"int变量名\"", &SymbolTable::locVariable}, {"\"参数串结束\"", &SymbolTable::inputDisp}
};

SymbolTable::SymbolTable()
{
    TAPEL.push_back({ "int", "TAPEL[0]" });
    VALL.push_back("OLD SP");
    VALL.push_back("返回地址");
    VALL.push_back("全局display地址");
}

int SymbolTable::callAction(string action, ReToken tk)
{
    return (this->*ACTIONL[action])(tk);
}

int SymbolTable::printTable()
{
    fstream file("TextFile\\SymbolTable.txt", fstream::out);
    int j = 0;
    file << "符号表总表\n序号\t标识符名\t类型\t种类\t地址指针" << endl;
    cout << "符号表总表\n序号\t标识符名\t类型\t种类\t地址指针" << endl;
    for (auto i = SYNBL.begin(); i != SYNBL.end(); i++)
    {
        cout << setw(2) << j << "\t" << *i;
        file << setw(2) << j++ << "\t" << *i;
    }
    j = 0;
    file << "\n\n类型表\n序号\t类型\t类型信息指针" << endl;
    cout << "\n\n类型表\n序号\t类型\t类型信息指针" << endl;
    for (auto i = TAPEL.begin(); i != TAPEL.end(); i++)
    {
        cout << setw(2) << j << "\t" << *i;
        file << setw(2) << j++ << "\t" << *i;
    }
    j = 0;
    file << "\n\n函数表\n序号\t层次号\t区距\t参数个数\t参数表指针" << endl;
    cout << "\n\n函数表\n序号\t层次号\t区距\t参数个数\t参数表指针" << endl;
    for (auto i = PFINFL.begin(); i != PFINFL.end(); i++)
    {
        cout << setw(2) << j << "\t" << *i;
        file << setw(2) << j++ << "\t" << *i;
    }
    j = VALL.size() - 1;
    file << "\n\n活动记录表VALL\n" << endl;
    cout << "\n\n活动记录表VALL\n" << endl;
    for (auto i = VALL.rbegin(); i != VALL.rend(); i++)
    {
        cout << j << "   " << *i << endl;
        file << j-- << "   " << *i << endl;
    }
    file.close();
    cout << "符号表已输出。" << endl;
    return 1;
}

int SymbolTable::func(ReToken tk)
{
    if (tk.type == 'I')
    {
        VALL.push_back("0");   //填写VALL表参数个数
        PFINFL.push_back({ 0,3,0,"TAPEL[0]" });   //填写函数表
        stringstream addr;
        addr << "PFINFL[" << PFINFL.size() - 1 << "]";
        SYNBL.push_back({ tk.name,"TAPEL[0]","f",addr.str() });  //填写符号表总表
    }
    return 1;
}

int SymbolTable::formalParaVar(ReToken tk)
{
    if (tk.type == 'I')
    {
        VALL.push_back(tk.name);   //填写VALL表
        stringstream addr;
        addr << "VALL[" << VALL.size() - 1 << "]";
        SYNBL.push_back({ tk.name,"TAPEL[0]","vf",addr.str() });  //填写符号表总表
        (PFINFL.rbegin())->addParaNum();   //递增函数表该函数形参个数
        addr.str("");
        addr << (PFINFL.rbegin())->getParaNum();
        VALL[3] = addr.str();  //修改VALL表中参数个数
        addr.str("");
        addr << "SYNBL[" << SYNBL.size() - 1 << "]";
        (PFINFL.rbegin())->modParaTable(addr.str());  //修改函数表该函数参数表指针
    }
    return 1;
}

int SymbolTable::inputDisp(ReToken token)
{
    VALL.push_back("Display表");
    return 1;
}

int SymbolTable::locVariable(ReToken tk)
{
    if (tk.type == 'I')
    {
        VALL.push_back(tk.name);   //填写VALL表
        stringstream addr;
        addr << "VALL[" << VALL.size() - 1 << "]";
        SYNBL.push_back({ tk.name,"TAPEL[0]","v",addr.str() });  //填写符号表总表
    }
    return 1;
}
