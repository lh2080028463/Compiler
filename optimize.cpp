#include "optimize.h"
#include "createquat.h"


extern quat qt[1000];//四元式

void optimize::initquat(){//四元式数组转成结构体，以qt为处理对象，初始化qua
    for (int i = 0; qt[i].op != ""; i++){//遍历所有有操作符的四元式，转移至qua中
        qua[i].op = qt[i].op;
        qua[i].num1.name = qt[i].arg1;
        qua[i].num2.name = qt[i].arg2;
        qua[i].ans.name = qt[i].result;

        //对类型做初始化
        if (qt[i].op == "="){//操作符初始化
            qua[i].type = 0;//赋值操作符
        }
        else if (qt[i].op == ">" || qt[i].op == "<" || qt[i].op == "+" || qt[i].op == "-" || qt[i].op == "*" || qt[i].op == "/"){
            qua[i].type = 2;//运算操作符
        }
        else
            qua[i].type = 3;//其他

        //操作数1初始化(string)
        if ((qt[i].arg1[0] >= 'a' && qt[i].arg1[0] <= 'z') || (qt[i].arg1[0] >= 'A' && qt[i].arg1[0] <= 'Z')){
            if (qt[i].arg1[1] >= '1' && qt[i].arg1[1] <= '9'){//识别t1,t2等标识符
                qua[i].num1.type = "2";//标为临时变量
            }
            else{//识别其他标识符
                qua[i].num1.type = "1";//标非临时变量
            }
        }
        else if (qt[i].arg1[0] >= '0' && qt[i].arg1[0] <= '9'){
            qua[i].num1.type = "3";//数字
        }
        else if (qt[i].arg1 == "_"){
            qua[i].num1.type = "";//下划线
        }

        //操作数2初始化
        if ((qt[i].arg2[0] >= 'a' && qt[i].arg2[0] <= 'z') || (qt[i].arg2[0] >= 'A' && qt[i].arg2[0] <= 'Z')){
            if (qt[i].arg2[1] >= '1' && qt[i].arg2[1] <= '9'){
                qua[i].num2.type = "2";
            }
            else{
                qua[i].num2.type = "1";
            }
        }
        else if (qt[i].arg2[0] >= '0' && qt[i].arg2[0] <= '9'){
            qua[i].num2.type = "3";
        }
        else if (qt[i].arg2 == "_"){
            qua[i].num2.type = "";
        }

        // 结果初始化
        if ((qt[i].result[0] >= 'a' && qt[i].result[0] <= 'z') || (qt[i].result[0] >= 'A' && qt[i].result[0] <= 'Z')){
            if (qt[i].result[1] >= '1' && qt[i].result[1] <= '9'){
                qua[i].ans.type = "2";
            }
            else{
                qua[i].ans.type = "1";
            }
        }
        else if (qt[i].result[0] >= '0' && qt[i].result[0] <= '9'){
            qua[i].ans.type = "3";
        }
        else if (qt[i].result == "_"){
            qua[i].ans.type = "";
        }
    }
}

void optimize::run(){
    int block_num;  //块的总数
    initquat();//初始化
    block_num = divide();//划分基本块
    std::fstream File("TextFile\\optQuat.txt", std::fstream::out);
    optqua(block_num, File);
    File.close();
    qDebug() << "优化后四元式已输出。";
}

void optimize::cleardag(){//清空dag
    for (int i = 0; i < dag.num; i++){
        dag.node[i].id = 0;
        dag.node[i].left = 0;
        dag.node[i].m_sign.name = "";
        dag.node[i].m_sign.type = "";
        dag.node[i].op = "";
        dag.node[i].right = 0;
        for (int j = 0; j < LEN; j++){
            dag.node[i].sign[j].name = "";
            dag.node[i].sign[j].type = "";
        }
    }
    dag.num = 0;
}

void optimize::addsign(int n, QString A, QString typ){//将A标记添加至n节点上，并删除无用标记
    int k;
    k = getid(A);//记录A在DAG中的节点标识
    if (k != 0){//如果getnode(A)不为空
        for (int j = 0; j < LEN; j++){//在节点getnode(A)上删除附标记
            if (dag.node[k - 1].sign[j].name == A){//若A是附加标记则删除
                dag.node[k - 1].sign[j].name = "";
                dag.node[k - 1].sign[j].type = "";
            }
        }
    }
    if (dag.node[n - 1].m_sign.name != ""){//主标记名字不为空，把A添加到附加标记中
        for (int j = 0; j < LEN; j++){//在节点n上添加附标记
            if (dag.node[n - 1].sign[j].name == ""){
                dag.node[n - 1].sign[j].name = A;
                dag.node[n - 1].sign[j].type = typ;
                break;
            }
        }
    }
    else{//主标记名字为空
        dag.node[n - 1].m_sign.name = A;
        dag.node[n - 1].m_sign.type = typ;
    }

    //若主标记为临时变量，附标记为非临时，则互换
    if (dag.node[n - 1].m_sign.type == "2"){
        for (int j = 0; j < LEN; j++){
            if (dag.node[n - 1].sign[j].type == "1"){
                QString a;//实现互换的中间变量
                a = dag.node[n - 1].sign[j].name;
                dag.node[n - 1].sign[j].name = dag.node[n - 1].m_sign.name;
                dag.node[n - 1].m_sign.name = a;
                dag.node[n - 1].sign[j].type = "2";
                dag.node[n - 1].m_sign.type = "1";
            }
        }
    }
}

void optimize::makeleaf(QString B, QString typ){//构造叶节点
    dag.num++;//节点总数加1
    dag.node[dag.num - 1].m_sign.name = B;//令主标记内容为当前字符串
    dag.node[dag.num - 1].m_sign.type = typ;//定义类型-临时变量、非临时变量、数字
    dag.node[dag.num - 1].id = dag.num;//定义节点标识
}

void optimize::delnode(int n){//删除节点n
    dag.node[n - 1].m_sign.name = "";
    dag.node[n - 1].id = 0;
    for (int i = n - 1; i < dag.num - 1; i++){//后面结点向前平移
        dag.node[i].id = dag.node[i + 1].id;
        dag.node[i].left = dag.node[i + 1].left;
        dag.node[i].m_sign.name = dag.node[i + 1].m_sign.name;
        dag.node[i].m_sign.type = dag.node[i + 1].m_sign.type;
        dag.node[i].op = dag.node[i + 1].op;
        dag.node[i].right = dag.node[i + 1].right;
        for (int j = 0; j < LEN; j++){
            dag.node[i].sign[j].name = dag.node[i + 1].sign[j].name;
            dag.node[i].sign[j].type = dag.node[i + 1].sign[j].type;
        }
    }

    //将最后一个结点置空
    dag.node[dag.num - 1].id = 0;
    dag.node[dag.num - 1].left = 0;
    dag.node[dag.num - 1].m_sign.name = "";
    dag.node[dag.num - 1].m_sign.type = "";
    dag.node[dag.num - 1].op = "";
    dag.node[dag.num - 1].right = 0;

    for (int j = 0; j < LEN; j++){//清除附加标记
        dag.node[dag.num - 1].sign[j].name = "";
        dag.node[dag.num - 1].sign[j].type = "";
    }
    dag.num--;//结点总数减1
}

void optimize::blockqua(int block, std::fstream& File){//生成block块中优化后的四元式
    //stringstream ss;
    int i, j, k = 0;
    int numB = 0, l, r;
    for (i = 0; i < dag.num; i++){//遍历DAG
        k = 0;
        if (dag.node[i].op == ""){//是叶子节点，赋值语句
            for (j = 0; j < LEN; j++){
                if (dag.node[i].sign[j].type == "1"){//赋值语句
                    k = 1;
                    numB = j;//记录j，后续用于操作数1赋值
                }
            }
            if (dag.node[i].sign[numB].name != ""){//附加标记不为空
                //主标记为数字或主标记为非临时变量且附标记有非临时变量
                if (dag.node[i].m_sign.type == "3" || (dag.node[i].m_sign.type == "1" && k == 1)){
                    File << block << " (=," << dag.node[i].m_sign.name.toStdString() << ",_," << dag.node[i].sign[numB].name.toStdString() << ")\n"; //( = B _ A)
                    // ss.str("");
                    // ss << "( =, " << dag.node[i].m_sign.name << ", _, " << dag.node[i].sign[numB].name << " )" << endl;
                    // cout << ss.str() << endl;
                    QString ss=QString("( =, %1, _, %2 )").arg(dag.node[i].m_sign.name).arg(dag.node[i].sign[numB].name);
                    qDebug() << ss;
                    qua[0].ans.name = dag.node[i].m_sign.name;
                    qua[0].ans.type = dag.node[i].m_sign.type;
                    qua[0].block = block;
                    qua[0].num1.name = dag.node[i].sign[numB].name;
                    qua[0].num1.type = dag.node[i].sign[numB].type;
                    qua[0].num2.name = "";
                    qua[0].num2.type = "";
                    qua[0].op = "=";//赋值语句
                    qua[0].type = 0;
                    Block[block].push_back(qua[0]);
                }
            }
        }
        else{
            l = dag.node[i].left;//记录左节点标记
            r = dag.node[i].right;//记录右节点标记
            File << block << " (" << dag.node[i].op.toStdString() << "," << dag.node[l - 1].m_sign.name.toStdString() << "," << dag.node[r - 1].m_sign.name.toStdString() << "," << dag.node[i].m_sign.name.toStdString() << ")\n";
            // ss.str("");
            // ss << "( " << dag.node[i].op << ", " << dag.node[l - 1].m_sign.name << ", " << dag.node[r - 1].m_sign.name << ", " << dag.node[i].m_sign.name << " )" << endl;
            // cout << ss.str() << endl;
            QString ss=QString("( %1, %2, %3, %4 )").arg(dag.node[i].op).arg(dag.node[l - 1].m_sign.name).arg(dag.node[r - 1].m_sign.name).arg(dag.node[i].m_sign.name);
            qDebug() << ss;
            qua[0].ans.name = dag.node[i].m_sign.name; qua[0].ans.type = dag.node[i].m_sign.type;
            qua[0].block = block;
            qua[0].num1.name = dag.node[l - 1].m_sign.name; qua[0].num1.type = dag.node[l - 1].m_sign.type;
            qua[0].num2.name = dag.node[r - 1].m_sign.name; qua[0].num2.type = dag.node[r - 1].m_sign.type;
            qua[0].op = dag.node[i].op;
            qua[0].type = 2;//+-*/><

            Block[block].push_back(qua[0]);//存入数组中
        }
    }
}

void optimize::optqua(int block_num, std::fstream& File){//优化四元式
    //stringstream ss;
    int newleft, newright;        //新建节点id
    int i = 0, j, k;
    int n;
    QString p;                   //若B,C为数字，p=B op C
    QString A, B, C;               //（，B，C，A）
    QVector<int> type3;
    for (k = 1; k <= block_num; k++){//循环
        type3.clear();//每次清空数组
        if (qua[i].type == 3){//操作符类别为if,el,ie,wh,do,we,lb,gt,ret,fun,endfun等
            File << qua[i].block << " (" << qua[i].op.toStdString() << "," << qua[i].num1.name.toStdString() << "," << qua[i].num2.name.toStdString() << "," << qua[i].ans.name.toStdString() << ")\n";//生成四元式语句在文件中
            // ss.str("");
            // ss << "( " << qua[i].op << ", " << qua[i].num1.name << ", " << qua[i].num2.name << ", " << qua[i].ans.name << " )" << endl;
            // cout << ss.str() << endl;
            QString ss=QString("( %1, %2, %3, %4 )").arg(qua[i].op).arg(qua[i].num1.name).arg(qua[i].num2.name).arg(qua[i].ans.name);
            qDebug() << ss;
            Block[qua[i].block].push_back(qua[i]);//将该四元式放入到对应语句块位置的四元式数组中
            i++;
            k = qua[i].block;//记录下一个四元式位置
        }
        while (qua[i].op != "" && qua[i].block == k){//当基本块还有未处理的四元式且是同一块时，循环
            if (qua[i].type == 3){//其他运算符如if,el,ie,wh,do,we,lb,gt,ret,fun,endfun等
                type3.push_back(i);//将该下标存入数组
            }
            A = qua[i].ans.name;//结果
            B = qua[i].num1.name;//数1
            C = qua[i].num2.name;//数2
            newleft = 0;//左结点
            newright = 0;//右节点
            if (getid(B) == 0 && qua[i].type != 3){//没被定义过且该四元式可以生成节点
                makeleaf(B, qua[i].num1.type);//构造B的叶子节点
                newleft = getid(B);//B作为左节点数字
            }
            switch (qua[i].type){//操作符类别
            case 0:   //赋值
                n = getid(B);//记录B的节点标识
                addsign(n, A, qua[i].ans.type);//把A添加到该节点主标识或附加标识上
                break;
            case 2:   //+-*/<>
                if (getid(C) == 0){
                    makeleaf(C, qua[i].num2.type); //构造c的叶子节点
                    newright = getid(C);//c作为右节点数字
                }
                j = 0;
                if ((qua[i].op == "*") || (qua[i].op == "/") || (qua[i].op == "+") || (qua[i].op == "-"))//操作符为加减乘除
                    j = 1;//记录
                if ((qua[i].num1.type == "3") && (qua[i].num2.type == "3") && j == 1){//两个操作数都为数字且可以运算得出结果
                    p = calcu(qua[i].op, B, C);//计算
                    if (newleft != 0) delnode(newleft);  //是新节点，则删除
                    if (newright != 0) delnode(newright);
                    if ((n = getid(p)) == 0){
                        makeleaf(p, "3");//构造结果的DAG节点
                        n = getid(p);//记录节点标识
                    }
                }
                else{
                    if ((n = findnode(qua[i].op, B, C)) == 0)  //在DAG中查找有无符合A=B op C的节点
                        n = makenode(qua[i].op, B, C);//若没有则添加新节点
                }
                addsign(n, A, qua[i].ans.type);                  //添加标记（主或附加）
                break;
            case 3://其他的退出
                break;
            }
            i++;
        }
        blockqua(k, File);      //生成优化的四元式
        for (auto j = type3.begin(); j != type3.end(); j++)//对特殊语句进行遍历
        {
            int i = *j;
            File << qua[i].block << " (" << qua[i].op.toStdString() << "," << qua[i].num1.name.toStdString() << "," << qua[i].num2.name.toStdString() << "," << qua[i].ans.name.toStdString() << ")\n";
            // ss.str("");
            // ss << "( " << qua[i].op << ", " << qua[i].num1.name << ", " << qua[i].num2.name << ", " << qua[i].ans.name << " )" << endl;
            // cout << ss.str() << endl;
            QString ss=QString("( %1, %2, %3, %4 )").arg(qua[i].op).arg(qua[i].num1.name).arg(qua[i].num2.name).arg(qua[i].ans.name);
            qDebug() << ss;
            Block[qua[i].block].push_back(qua[i]);//存入数组中
        }
        cleardag();    //对dag清零
    }
}

int optimize::getid(QString B){//获取标记为B的节点的id
    int n = 0;      //节点id
    for (int i = 0; i < dag.num; i++){//遍历所有节点
        if (dag.node[i].m_sign.name == B)//在主节点中找到
            n = i + 1;//返回结点数字（该节点数字从1开始计数，因而返回i+1
        for (int j = 0; j < LEN; j++){
            if (dag.node[i].sign[j].name == B)//在附加结点中找到
                n = i + 1;
        }
    }
    return n;
}

int optimize::findnode(QString opl, QString B, QString C){//在DAG中查找有无符合A=B op C的节点
    int n = 0;
    int l, r, k = 0;
    l = getid(B);//获取B节点的id
    r = getid(C);//获取C节点的id
    for (int i = 0; i < dag.num; i++){
        k = 0;
        if (dag.node[i].op == opl){//op满足
            l = dag.node[i].left;//B放在左子节点
            for (int j = 0; j < LEN; j++){
                if (dag.node[l - 1].sign[j].name == B) //附标记有B
                    k = 1;
            }
            if ((dag.node[l - 1].m_sign.name == B) || k == 1){//B满足
                k = 0;
                r = dag.node[i].right;//C放在右子节点
                for (int j = 0; j < LEN; j++){
                    if (dag.node[r - 1].sign[j].name == C) //附标记有C
                        k = 1;
                }
                if ((dag.node[r - 1].m_sign.name == C) || k == 1){//C满足
                    n = i + 1;
                    break;//跳出循环
                }
            }
        }
    }
    return n;//返回找到的节点标号
}

int optimize::makenode(QString opl, QString B, QString C){//构造中间节点
    dag.num++;//节点数量加1
    dag.node[dag.num - 1].id = dag.num;
    dag.node[dag.num - 1].left = getid(B);
    dag.node[dag.num - 1].right = getid(C);
    dag.node[dag.num - 1].op = opl;
    return dag.num;//返回节点数量
}

int optimize::divide(){//划分基本块
    int i = 0;
    int num = 1;
    while (qua[i].op != ""){//当还有未处理的四元式，循环
        if (qua[i].type != 3 || qua[i].op == "lb" || qua[i].op == "wh" || qua[i].op == "fun" || qua[i].op == "ret" || qua[i].op == "endfun")
            qua[i].block = num;//当前语句所在位置即为基本块划分位置
        if (qua[i].op == "if" || qua[i].op == "el" || qua[i].op == "ie" || qua[i].op == "do" || qua[i].op == "we" || qua[i].op == "gt"){
            qua[i].block = num;//当前语句所在位置即为基本块划分位置
            num++;//基本块加1
        }
        i++;
    }
    //qua[i].op==""
    if (qua[i - 1].op == "if" || qua[i - 1].op == "el" || qua[i - 1].op == "ie" || qua[i - 1].op == "do" || qua[i - 1].op == "we" || qua[i - 1].op == "gt")//结束语句在最后
        num--;//特殊情况，基本块减1
    return num;//返回语句数量
}

QString optimize::calcu(QString opl, QString B, QString C)   //计算两个数运算的值
{
    int val;
    if(opl[0]=='+'){
        val = B.toInt() + C.toInt();
    }
    else if(opl[0]=='-'){
        val = B.toInt() - C.toInt();
    }
    else if(opl[0]=='*'){
        val = B.toInt() * C.toInt();
    }
    else if(opl[0]=='/'){
        val = B.toInt() / C.toInt();
    }
    // a = opl[0];//操作符
    // switch (a)
    // {
    // case '+':
    //     val = atof(B.c_str()) + atof(C.c_str());
    //     break;
    // case '-':
    //     val = atof(B.c_str()) - atof(C.c_str());
    //     break;
    // case '*':
    //     val = atof(B.c_str()) * atof(C.c_str());
    //     break;
    // case '/':
    //     val = atof(B.c_str()) / atof(C.c_str());
    //     break;
    // }
    QString p = QString::number(val);
    return p;//返回计算值
}
