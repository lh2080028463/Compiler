#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include<Qstring>
#include<QVector>
#include<QStack>
#include<QDebug>
//#include<QFile>
//#include<QTextStream>
#include<fstream>

#define LEN 100//长度

class optimize{//优化类
public:
    //optimize(){};

    struct Token{//Token结构体
        QString name,type;//名称、种类
    };

    //四元式
    struct Quaternion{//四元式结构体
        int block;           //基本块标识
        int type;            //操作符类型 0 1 2 3
        QString op;           //操作符
        Token num1;         //操作数1
        Token num2;         //操作数2
        Token ans;          //结果
    };

    struct ACT_INF{//四元式活跃信息
        QString num1,num2,ans;//数1 数2 结果

        ACT_INF(QString a = "", QString b = "", QString c = ""){//构造函数
            num1 = a; num2 = b; ans = c;
        }
    };

    struct ACT_SYNBL{
        QString name,act_inf;//名称 四元式活跃信息

        ACT_SYNBL(QString a = "", QString b = ""){//构造函数
            name = a; act_inf = b;
        }
    };

    QVector<QVector<ACT_INF> >Act_inf;//活跃信息表
    QVector<QVector<ACT_SYNBL> >Act_synbl;
    QVector<QString>objcode;//目标代码
    QStack<QString>SEM; //保存待返填地址
    Quaternion qua[LEN];   //四元式数组
    QVector<Quaternion> Block[15];//存四元式的数组
    QVector<QVector<Quaternion>> ToTarget;//存四元式数组的数组

    //DAG
    struct Node{//DAG节点
        int left;                //左儿子
        int right;               //右儿子
        int id;                  //节点标号
        QString op;               //操作符
        Token m_sign;            //主标记
        Token sign[LEN];        //附加标记
    };
    struct DAG{//DAG结构体
        int num = 0;//节点数量
        Node node[LEN];//存放DAG结点
    }dag;

    //函数
    void run();//运行
    void initquat();//初始化四元式结构体数组
    void cleardag();//对dag清零
    void addsign(int n,QString A,QString typ);//将A标记添加至n节点上，并删除无用标记
    void makeleaf(QString B,QString typ);//构造叶子节点
    void delnode(int n);     //删除id为n的节点
    void blockqua(int block,std::fstream& File);   //生成block块中优化后的四元式
    void optqua(int block_num,std::fstream& File);    //优化四元式
    int getid(QString B);   //获取标记为B的节点的id
    int findnode(QString opl,QString B,QString C);   //在DAG中查找有无符合A=B op C的节点，返回节点id
    int makenode(QString opl,QString B,QString C);  //构造中间节点
    int divide();             //划分基本块
    QString calcu(QString opl,QString B,QString C);   //计算两个数运算的值，返回计算值
};

#endif // OPTIMIZE_H
