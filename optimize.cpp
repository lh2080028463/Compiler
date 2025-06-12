#include "optimize.h"
#include "createquat.h"
#include<QFile>

// 声明外部四元式数组，该数组在其他文件中定义
extern quat qt[100];//四元式

/**
 * @brief 将四元式数组转换为结构体，初始化 qua 数组
 *
 * 遍历所有有操作符的四元式，将四元式的操作符、操作数和结果信息转移至 qua 数组中，
 * 同时对操作符类型、操作数类型和结果类型进行初始化。
 */
void optimize::initquat(){//四元式数组转成结构体，以qt为处理对象，初始化qua
    // 遍历所有有操作符的四元式，转移至 qua 中
    for (int i = 0; qt[i].op != ""; i++){
        // 复制操作符
        qua[i].op = qt[i].op;
        // 复制操作数 1 的名称
        qua[i].num1.name = qt[i].arg1;
        // 复制操作数 2 的名称
        qua[i].num2.name = qt[i].arg2;
        // 复制结果的名称
        qua[i].ans.name = qt[i].result;

        // 对类型做初始化
        // 操作符初始化
        if (qt[i].op == "="){
            // 赋值操作符
            qua[i].type = 0;
        }
        else if (qt[i].op == ">" || qt[i].op == "<" || qt[i].op == "+" || qt[i].op == "-" || qt[i].op == "*" || qt[i].op == "/"){
            // 运算操作符
            qua[i].type = 2;
        }
        else
            // 其他操作符
            qua[i].type = 3;

        // 操作数 1 初始化
        if ((qt[i].arg1[0] >= 'a' && qt[i].arg1[0] <= 'z') || (qt[i].arg1[0] >= 'A' && qt[i].arg1[0] <= 'Z')){
            if (qt[i].arg1.length()>1&&qt[i].arg1[1] >= '1' && qt[i].arg1[1] <= '9'){
                // 识别 t1,t2 等临时变量标识符
                qua[i].num1.type = "2";
            }
            else{
                // 识别其他非临时变量标识符
                qua[i].num1.type = "1";
            }
        }
        else if (qt[i].arg1[0] >= '0' && qt[i].arg1[0] <= '9'){
            // 数字
            qua[i].num1.type = "3";
        }
        else if (qt[i].arg1 == "_"){
            // 下划线
            qua[i].num1.type = "";
        }

        // 操作数 2 初始化
        if ((qt[i].arg2[0] >= 'a' && qt[i].arg2[0] <= 'z') || (qt[i].arg2[0] >= 'A' && qt[i].arg2[0] <= 'Z')){
            if (qt[i].arg2.length()>1&&qt[i].arg2[1] >= '1' && qt[i].arg2[1] <= '9'){
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
            if (qt[i].result.length()>1&&qt[i].result[1] >= '1' && qt[i].result[1] <= '9'){
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

/**
 * @brief 执行四元式优化操作
 *
 * 初始化四元式信息，划分基本块，将优化后的四元式输出到文件中。
 */
void optimize::run(){
    // 块的总数
    int block_num;
    // 初始化四元式信息
    initquat();
    // 划分基本块
    block_num = divide();
    // 打开文件用于写入优化后的四元式
    std::fstream File("TextFile\\optQuat.txt", std::fstream::out);
    // 执行四元式优化
    optqua(block_num, File);
    // 关闭文件
    File.close();
    // 输出优化完成信息
    qDebug() << "优化后四元式已输出。";
}

/**
 * @brief 清空 DAG（有向无环图）信息
 *
 * 将 DAG 中所有节点的信息清零，包括节点标识、左右子节点、标记和操作符等，
 * 并将节点总数置为 0。
 */
void optimize::cleardag(){//清空dag
    for (int i = 0; i < dag.num; i++){
        // 节点标识置为 0
        dag.node[i].id = 0;
        // 左子节点置为 0
        dag.node[i].left = 0;
        // 主标记名称清空
        dag.node[i].m_sign.name.clear();
        // 主标记类型清空
        dag.node[i].m_sign.type.clear();
        // 操作符清空
        dag.node[i].op.clear();
        // 右子节点置为 0
        dag.node[i].right = 0;
        for (int j = 0; j < LEN; j++){
            // 附加标记名称清空
            dag.node[i].sign[j].name.clear();
            // 附加标记类型清空
            dag.node[i].sign[j].type.clear();
        }
    }
    // 节点总数置为 0
    dag.num = 0;
}

/**
 * @brief 将标记 A 添加至指定节点 n 上，并删除无用标记
 *
 * 若标记 A 已存在于 DAG 中其他节点上，则删除该节点上的标记 A。
 * 将标记 A 添加到节点 n 的主标记或附加标记中
 * 若主标记为临时变量，附加标记为非临时变量，则互换两者。
 *
 * @param n 要添加标记的节点编号
 * @param A 要添加的标记名称
 * @param typ 标记的类型
 */
void optimize::addsign(int n, QString A, QString typ){//将A标记添加至n节点上，并删除无用标记
    // 记录 A 在 DAG 中的节点标识
    int k;
    k = getid(A);
    if (k){
        // 在节点 getnode(A) 上删除附标记
        for (int j = 0; j < LEN; j++){
            if (dag.node[k - 1].sign[j].name == A){
                // 若 A 是附加标记则删除
                dag.node[k - 1].sign[j].name.clear();
                dag.node[k - 1].sign[j].type.clear();
            }
        }
    }
    if (dag.node[n - 1].m_sign.name.length()){
        // 主标记名字不为空，把 A 添加到附加标记中
        for (int j = 0; j < LEN; j++){
            if (!dag.node[n - 1].sign[j].name.length()){
                dag.node[n - 1].sign[j].name = A;
                dag.node[n - 1].sign[j].type = typ;
                break;
            }
        }
    }
    else{
        // 主标记名字为空
        dag.node[n - 1].m_sign.name = A;
        dag.node[n - 1].m_sign.type = typ;
    }

    // 若主标记为临时变量，附标记为非临时，则互换
    if (dag.node[n - 1].m_sign.type == "2"){
        for (int j = 0; j < LEN; j++){
            if (dag.node[n - 1].sign[j].type == "1"){
                // 实现互换的中间变量
                QString a;
                a = dag.node[n - 1].sign[j].name;
                dag.node[n - 1].sign[j].name = dag.node[n - 1].m_sign.name;
                dag.node[n - 1].m_sign.name = a;
                dag.node[n - 1].sign[j].type = "2";
                dag.node[n - 1].m_sign.type = "1";
            }
        }
    }
}

/**
 * @brief 构造叶节点
 *
 * 在 DAG 中创建一个新的叶节点，设置节点的主标记内容、类型和节点标识。
 *
 * @param B 节点的主标记内容
 * @param typ 节点的类型（临时变量、非临时变量、数字）
 */
void optimize::makeleaf(QString B, QString typ){//构造叶节点
    // 节点总数加 1
    dag.num++;
    // 令主标记内容为当前字符串
    dag.node[dag.num - 1].m_sign.name = B;
    // 定义类型 - 临时变量、非临时变量、数字
    dag.node[dag.num - 1].m_sign.type = typ;
    // 定义节点id
    dag.node[dag.num - 1].id = dag.num;
}

/**
 * @brief 删除指定节点 n
 *
 * 将节点 n 的信息清空，并将后面的节点向前平移，最后一个节点置空，
 * 节点总数减 1。
 *
 * @param n 要删除的节点编号
 */
void optimize::delnode(int n){//删除节点n
    // 主标记名称置为空
    dag.node[n - 1].m_sign.name.clear();
    // 节点标识置为 0
    dag.node[n - 1].id = 0;
    // 后面结点向前平移
    for (int i = n - 1; i < dag.num - 1; i++){
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

    // 将最后一个结点置空
    dag.node[dag.num - 1].id = 0;
    dag.node[dag.num - 1].left = 0;
    dag.node[dag.num - 1].m_sign.name.clear();
    dag.node[dag.num - 1].m_sign.type.clear();
    dag.node[dag.num - 1].op.clear();
    dag.node[dag.num - 1].right = 0;

    for (int j = 0; j < LEN; j++){
        // 清除附加标记
        dag.node[dag.num - 1].sign[j].name.clear();
        dag.node[dag.num - 1].sign[j].type.clear();
    }
    // 结点总数减 1
    dag.num--;
}

/**
 * @brief 生成指定块中优化后的四元式
 *
 * 遍历 DAG，根据节点类型生成相应的四元式并输出到文件中，
 * 同时将四元式信息存入 Block 数组中。
 *
 * @param block 要处理的块编号
 * @param File 用于输出四元式的文件流
 */
void optimize::blockqua(int block, std::fstream& File){//生成block块中优化后的四元式
    int i, j, k = 0;
    int numB = 0, l, r;
    // 遍历 DAG
    for (i = 0; i < dag.num; i++){
        k = 0;
        if (!dag.node[i].op.length()){
            // 是叶子节点，赋值语句
            for (j = 0; j < LEN; j++){
                if (dag.node[i].sign[j].type == "1"){
                    // 赋值语句
                    k = 1;
                    // 记录 j，后续用于操作数 1 赋值
                    numB = j;
                }
            }
            if (dag.node[i].sign[numB].name != ""){
                // 附加标记不为空
                // 主标记为数字或主标记为非临时变量且附标记有非临时变量
                if (dag.node[i].m_sign.type == "3" || (dag.node[i].m_sign.type == "1" && k == 1)){
                    // 输出赋值四元式到文件
                    File << block << " (=," << dag.node[i].m_sign.name.toStdString() << ",_," << dag.node[i].sign[numB].name.toStdString() << ")\n";
                    QString ss=QString("( =, %1, _, %2 )").arg(dag.node[i].m_sign.name).arg(dag.node[i].sign[numB].name);
                    // 输出四元式信息到调试窗口
                    qDebug() << ss;
                    // 记录四元式信息
                    qua[0].ans.name = dag.node[i].m_sign.name;
                    qua[0].ans.type = dag.node[i].m_sign.type;
                    qua[0].block = block;
                    qua[0].num1.name = dag.node[i].sign[numB].name;
                    qua[0].num1.type = dag.node[i].sign[numB].type;
                    qua[0].num2.name = "";
                    qua[0].num2.type = "";
                    // 赋值语句
                    qua[0].op = "=";
                    qua[0].type = 0;
                    // 将四元式存入 Block 数组中
                    Block[block].push_back(qua[0]);
                }
            }
        }
        else{
            // 记录左节点标记
            l = dag.node[i].left;
            // 记录右节点标记
            r = dag.node[i].right;
            // 输出运算四元式到文件
            File << block << " (" << dag.node[i].op.toStdString() << "," << dag.node[l - 1].m_sign.name.toStdString() << "," << dag.node[r - 1].m_sign.name.toStdString() << "," << dag.node[i].m_sign.name.toStdString() << ")\n";
            QString ss=QString("( %1, %2, %3, %4 )").arg(dag.node[i].op).arg(dag.node[l - 1].m_sign.name).arg(dag.node[r - 1].m_sign.name).arg(dag.node[i].m_sign.name);
            // 输出四元式信息到调试窗口
            qDebug() << ss;
            // 记录四元式信息
            qua[0].ans.name = dag.node[i].m_sign.name; qua[0].ans.type = dag.node[i].m_sign.type;
            qua[0].block = block;
            qua[0].num1.name = dag.node[l - 1].m_sign.name; qua[0].num1.type = dag.node[l - 1].m_sign.type;
            qua[0].num2.name = dag.node[r - 1].m_sign.name; qua[0].num2.type = dag.node[r - 1].m_sign.type;
            qua[0].op = dag.node[i].op;
            // +-*/><
            qua[0].type = 2;

            // 存入数组中
            Block[block].push_back(qua[0]);
        }
    }
}

/**
 * @brief 优化四元式
 *
 * 按基本块对四元式进行优化，构建 DAG 图，合并公共子表达式，
 * 并将优化后的四元式输出到文件中。
 *
 * @param block_num 基本块的总数
 * @param File 用于输出四元式的文件流
 */
void optimize::optqua(int block_num, std::fstream& File){//优化四元式
    // 新建节点 id
    int newleft, newright;
    int i = 0, j, k;
    int n;
    // 若 B,C 为数字，p = B op C
    QString p;
    // （op，B，C，A）
    QString A, B, C;
    QVector<int> type3;
    // 循环处理每个基本块
    for (k = 1; k <= block_num; k++){
        // 每次清空数组
        type3.clear();
        if (qua[i].type == 3){
            // 操作符类别为 if,el,ie,wh,do,we,lb,gt,ret,fun,endfun 等
            // 生成四元式语句在文件中
            File << qua[i].block << " (" << qua[i].op.toStdString() << "," << qua[i].num1.name.toStdString() << "," << qua[i].num2.name.toStdString() << "," << qua[i].ans.name.toStdString() << ")\n";
            QString ss=QString("( %1, %2, %3, %4 )").arg(qua[i].op).arg(qua[i].num1.name).arg(qua[i].num2.name).arg(qua[i].ans.name);
            // 输出四元式信息到调试窗口
            qDebug() << ss;
            // 将该四元式放入到对应语句块位置的四元式数组中
            Block[qua[i].block].push_back(qua[i]);
            i++;
            // 记录下一个四元式位置
            k = qua[i].block;
        }
        // 当基本块还有未处理的四元式且是同一块时，循环
        while (qua[i].op != "" && qua[i].block == k){
            if (qua[i].type == 3){
                // 其他运算符如 if,el,ie,wh,do,we,lb,gt,ret,fun,endfun 等
                // 将该下标存入数组
                type3.push_back(i);
            }
            // 结果
            A = qua[i].ans.name;
            // 数 1
            B = qua[i].num1.name;
            // 数 2
            C = qua[i].num2.name;
            // 左结点
            newleft = 0;
            // 右节点
            newright = 0;
            if (getid(B) == 0 && qua[i].type != 3){
                // 没被定义过且该四元式可以生成节点
                // 构造 B 的叶子节点
                makeleaf(B, qua[i].num1.type);
                // B 作为左节点数字
                newleft = getid(B);
            }
            // 操作符类别
            switch (qua[i].type){
            case 0:   // 赋值
                // 记录 B 的节点标识
                n = getid(B);
                // 把 A 添加到该节点主标识或附加标识上
                addsign(n, A, qua[i].ans.type);
                break;
            case 2:   // +-*/<>
                if (getid(C) == 0){
                    // 构造 C 的叶子节点
                    makeleaf(C, qua[i].num2.type);
                    // C 作为右节点数字
                    newright = getid(C);
                }
                j = 0;
                if ((qua[i].op == "*") || (qua[i].op == "/") || (qua[i].op == "+") || (qua[i].op == "-")){
                    // 操作符为加减乘除
                    j = 1;
                }
                if ((qua[i].num1.type == "3") && (qua[i].num2.type == "3") && j == 1){
                    // 两个操作数都为数字且可以运算得出结果
                    // 计算
                    p = calcu(qua[i].op, B, C);
                    if (newleft != 0) {
                        // 是新节点，则删除
                        delnode(newleft);
                    }
                    if (newright != 0) {
                        delnode(newright);
                    }
                    if ((n = getid(p)) == 0){
                        // 构造结果的 DAG 节点
                        makeleaf(p, "3");
                        // 记录节点标识
                        n = getid(p);
                    }
                }
                else{
                    // 在 DAG 中查找有无符合 A = B op C 的节点
                    if ((n = findnode(qua[i].op, B, C)) == 0)  {
                        // 若没有则添加新节点
                        n = makenode(qua[i].op, B, C);
                    }
                }
                // 添加标记（主或附加）
                addsign(n, A, qua[i].ans.type);
                break;
            case 3:// 其他的退出
                break;
            }
            i++;
        }
        // 生成优化的四元式
        blockqua(k, File);
        // 对特殊语句进行遍历
        for (auto j = type3.begin(); j != type3.end(); j++){
            int i = *j;
            File << qua[i].block << " (" << qua[i].op.toStdString() << "," << qua[i].num1.name.toStdString() << "," << qua[i].num2.name.toStdString() << "," << qua[i].ans.name.toStdString() << ")\n";
            QString ss=QString("( %1, %2, %3, %4 )").arg(qua[i].op).arg(qua[i].num1.name).arg(qua[i].num2.name).arg(qua[i].ans.name);
            qDebug() << ss;
            // 存入数组中
            Block[qua[i].block].push_back(qua[i]);
        }
        // 对 dag 清零
        cleardag();
    }
}

/**
 * @brief 获取标记为 B 的节点的 id
 *
 * 遍历 DAG 中的所有节点，查找主标记和附加标记中是否存在标记 B，
 * 若存在则返回该节点的 id，否则返回 0。
 *
 * @param B 要查找的标记名称
 * @return 标记为 B 的节点的 id，若不存在则返回 0
 */
int optimize::getid(QString B){//获取标记为B的节点的id
    // id 初始化为 0
    int pos = 0;
    for (int i = 0; i < dag.num; i++){
        if (dag.node[i].m_sign.name == B){
            // 在主节点中找到
            // 返回结点 id（该节点数字从 1 开始计数，因而返回 i + 1
            pos = i + 1;
        }
        for (int j = 0; j < LEN; j++){
            if (dag.node[i].sign[j].name == B){
                // 在附加结点中找到
                pos = i + 1;
            }
        }
    }
    return pos;
}

/**
 * @brief 在 DAG 中查找有无符合 A = B op C 的节点
 *
 * 遍历 DAG 中的所有节点，检查是否存在操作符为 opl，
 * 左子节点包含 B，右子节点包含 C 的节点，若存在则返回该节点的编号，
 * 否则返回 0。
 *
 * @param opl 操作符
 * @param B 操作数 1
 * @param C 操作数 2
 * @return 符合条件的节点编号，若不存在则返回 0
 */
int optimize::findnode(QString opl, QString B, QString C){//在DAG中查找有无符合A=B op C的节点
    int pos = 0;
    int l, r, k = 0;
    // 获取 B 节点的 id
    l = getid(B);
    // 获取 C 节点的 id
    r = getid(C);
    for (int i = 0; i < dag.num; i++){
        k = 0;
        if (dag.node[i].op == opl){
            // op 满足
            // B 放在左子节点
            l = dag.node[i].left;
            for (int j = 0; j < LEN; j++){
                if (dag.node[l - 1].sign[j].name == B) {
                    // 附标记有 B
                    k = 1;
                }
            }
            if ((dag.node[l - 1].m_sign.name == B) || k == 1){
                // B 满足
                k = 0;
                // C 放在右子节点
                r = dag.node[i].right;
                for (int j = 0; j < LEN; j++){
                    if (dag.node[r - 1].sign[j].name == C) {
                        // 附标记有 C
                        k = 1;
                    }
                }
                if ((dag.node[r - 1].m_sign.name == C) || k == 1){
                    // C 满足
                    pos = i + 1;
                    // 跳出循环
                    break;
                }
            }
        }
    }
    // 返回找到的节点标号
    return pos;
}

/**
 * @brief 构造中间节点
 *
 * 在 DAG 中创建一个新的中间节点，设置节点的标识、左右子节点和操作符，
 * 节点总数加 1，返回新节点的编号。
 *
 * @param opl 操作符
 * @param B 操作数 1
 * @param C 操作数 2
 * @return 新节点的编号
 */
int optimize::makenode(QString opl, QString B, QString C){//构造中间节点
    // 节点数量加 1
    dag.num++;
    dag.node[dag.num - 1].id = dag.num;
    dag.node[dag.num - 1].left = getid(B);
    dag.node[dag.num - 1].right = getid(C);
    dag.node[dag.num - 1].op = opl;
    // 返回节点数量
    return dag.num;
}

/**
 * @brief 划分基本块
 *
 * 遍历四元式数组，根据操作符类型划分基本块，返回基本块的总数。
 *
 * @return 基本块的总数
 */
int optimize::divide(){//划分基本块
    int i = 0;
    // 数目初始化为 1
    int num = 1;
    for(int i=0;qua[i].op != "";i++){
        if (qua[i].type != 3 || qua[i].op == "lb" || qua[i].op == "wh" || qua[i].op == "fun" || qua[i].op == "ret" || qua[i].op == "endfun"){
            // 当前语句所在位置即为基本块划分位置
            qua[i].block = num;
        }
        if (qua[i].op == "if" || qua[i].op == "el" || qua[i].op == "ie" || qua[i].op == "do" || qua[i].op == "we" || qua[i].op == "gt"){
            // 当前语句所在位置即为基本块划分位置
            qua[i].block = num;
            // 块数加 1
            num++;
        }
    }
    // qua[i].op==""
    if (qua[i - 1].op == "if" || qua[i - 1].op == "el" || qua[i - 1].op == "ie" || qua[i - 1].op == "do" || qua[i - 1].op == "we" || qua[i - 1].op == "gt"){
        // 结束语句在最后
        // 特殊情况，块数减 1
        num--;
    }
    // 返回块的数量
    return num;
}

/**
 * @brief 计算两个数运算的值
 *
 * 根据操作符对两个操作数进行相应的运算，返回运算结果的字符串表示。
 *
 * @param opl 操作符
 * @param B 操作数 1
 * @param C 操作数 2
 * @return 运算结果的字符串表示
 */
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

    // 返回计算值
    return QString::number(val);
}
