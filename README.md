## ScpiParseTree
这个类用于注册和调用SCPI指令对应的函数

这里是两个目标函数用于测试
```c++
static void func1(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}

static int func2(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}

double add(int a,double b)
{
    std::cout<<__PRETTY_FUNCTION__<<a<<" "<<b<<std::endl<<std::flush;
    return a + b;
}

class Object
{
public:
    void func(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
    
    double subtract(double a,int b)
    {
        std::cout<<__PRETTY_FUNCTION__<<a<<" "<<b<<std::endl<<std::flush;
        return a - b;
    }
};
```

## 一：关于类ScpiParseTree的成员函数和使用方法介绍
ScpiParseTree作为暴露给其他用户直接使用的接口仅仅提供了两个必要的接口,一个是注册SCPI指令的注册接口,一个是解析和执行SCPI指令对应函数的执行接口,他们的定义和使用方法如下:<br />
### 1.绑定SCPI指令和目标函数
```c++
template <typename SetFunc,typename GetFunc,typename Obj>
registeScpi(const std::string& scpi,SetFunc setFunc,GetFunc getFunc,Obj* obj)//注册成员函数为SCPI指令调用

template <typename SetFunc,typename GetFunc>
registeScpi(const std::string& scpi,SetFunc setFunc,GetFunc getFunc)//注册自由函数为SCPI指令调用
```
有一些指令既可以设置参数也可以用来读取参数,但是有一些参数只能用来读取或者设置,对于不存在的目标函数,直接传递空指针即可
```c++
ScpiParseTree tree;
tree.registeScpi("A[:B][:C]:D", func1, nullptr);//将自由函数func1作为设置函数绑定到A:B[:C]:D
tree.registeScpi("X[:Y]:Z[:W]:T", nullptr, func2); //将自由函数func2作为查询函数绑定到X[:Y]:Z[:W]:T
tree.registeScpi("G[:H]:I:J", &Object::func, nullptr,&obj); //将成员函数Object::func作为设置函数绑定到G[:H]:I:J
```
### 2.解析SCPI指令并调用目标函数
```c++
std::string excute(const std::string& scpi)
```
直接将接收到的SCPI字符串传递给excute函数即可完成对应函数的调用<br />
```c++
tree.excute("A:D");             // 全部省略
tree.excute("A:B:D");           // 包含B
tree.excute("A:C:D");           // 包含C
tree.excute("A:B:C:D");         // 全部包含
```

### 注:以上接口就是业务开发者需要用到的全部接口,关于原始指令的注册、输入指令的查找和匹配,函数参数的解析和调用这部分内容会在后续的文档中进行详细说明，这些内容都属于解析树的内部实现，非解析树开发或维护人员无需关心这些具体的细节。
### 以下为解析树内部实现原理和接口说明

## 二.关于类ScpiParseNode的成员函数和内部原理说明
ScpiParseNode是解析树的核心数据结构,每一条原始SCPI指令都可以拆分为若干条子命令,每一条子命令都有一个对应的ScpiParseNode节点,由一个root节点和若干个ScpiParseNode构成一颗完整的SCPI解析树,每一条原始SCPI指令都对应着一条从root节点触发的唯一的分支。<br />
假设存在以下几条原始SCPI指令:
```txt
A[:B][:C]:D
A[:B][:C]:F:G
A:E:I:O
X[:Y]:Z[:W]:T
X[:Y]:M
X[:Y]:N
P:Q[:R]:S
```
对应的解析树为:<br />
![alt text](pic/fig1.png)<br />

1.原始指令的注册<br />
SCPI指令节点通常以':'为分隔符,一条基础的SCPI指令格式如下:<br />
#1 FIRSt:SECond:THIRd:FOURth<br />
这条指令不包含可省略节点,也不携带指令节点参数,更没有嵌套语法,对于这种基础SCPI指令,直接按照':'分割即可<br />
如果指令携带了节点参数而且包含可省略节点:
#2 FIRSt\<n>:SECond[:THIRd]:FOURth<br />
这条语句中的第一个节点携带了一个节点参数\<n>,而且第三个节点可省略,甚至更复杂一点嵌套语句:<br />
#3 [FIRSt[\<n>]]:SECond[:THIRd]:FOURth<br />
这条语句的第一个节点携带了一个可省略的节点参数,而且这个节点自身也是可以省略的.但是总体来说,原始指令注册的时候只需要区分节点是否可省略即可,节点内部携带的额外信息不需要在注册的时候处理。
因此在原始SCPI指令注册时只存在以下两种情况:
1.以':'开头的普通节点<br />
对于#1这样的SCPI指令,直接按':'切割可以得到以下节点:
2.以'['开头的可省略节点<br />
对于#3这样的SCPI指令,直接按':'切割可以得到以下节点:

2.输入指令的切割<br />

3.输入指令和原始指令节点的查找和匹配<br />

4.额外参数的处理<br />

5.函数的调用<br />

**注册时判断子分支冲突:一个节点下不能同时存在一条以上可全部省略的分支
在注册是会先将原始SCPI指令拆分为N级,当最后的M级指令全部都可省略时,则将这M级对应的指令标记为[存在可省略分支],后续注册时如果再次遇到最后M级指令可省略,而当前节点已经被标记为[存在可省略分支]则直接退出程序,这种情况需要修改原始SCPI指令中存在的冲突。
在注册拆解原始SCPI指令时,会对拆分好的指令进行判断,逆向遍历并将最后M级可省略的指令按顺序保存到一个全局数组中,在生成节点时判断该节点对应的指令是否存在于这个全局数组中，如果存在就将这个节点标记为[存在可省略分支]节点