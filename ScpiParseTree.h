#ifndef SCPIPARSETREE_H
#define SCPIPARSETREE_H

#include "ScpiParseNode.h"

///解析流程
///1.先将接收到的SCPI指令字符串拆分为[指令]和[参数]两部分
///2.将[指令]部分按":"分割成若干个子命令
///3.按顺序对子命令进行逐级递归查找
///4.在查找过程中将可省略的函数参数记录下来
///5.查找完成之后返回可调用的[函数包装器],同时将被省略的函数参数按顺序添加到[参数]前方
///6.将组合好的[参数]传递给[函数包装器]执行并返回结果字符串
class ScpiParseTree
{
public:
    ScpiParseTree();

    ~ScpiParseTree();

    template <typename Func>
    struct FuncOrNull
    {
        using FuncType = typename std::remove_pointer<Func>::type;
        static constexpr bool value = std::is_function<FuncType>::value || std::is_null_pointer<Func>::value;
    };

    template <typename SetFunc,typename GetFunc,typename Obj>
    typename std::enable_if<std::is_object<Obj>::value>::type
    registeScpi(const std::string& scpi,SetFunc setFunc,GetFunc getFunc,Obj* obj)
    {
        std::vector<std::string> cmdVec = Awg::splitScpiPattern(scpi);
        FunctionWrapper* setWrapper = new FunctionWrapper(setFunc,obj);
        FunctionWrapper* getWrapper = new FunctionWrapper(getFunc,obj);

        root->addScpiNode(cmdVec,setWrapper,getWrapper);
    }
/**
 * 初始化过程中的任何问题都抛出异常,确保初始化的结果是没有歧义的**/
    template <typename SetFunc,typename GetFunc>
    typename std::enable_if<FuncOrNull<SetFunc>::value && FuncOrNull<GetFunc>::value>::type
    registeScpi(const std::string& scpi,SetFunc setFunc,GetFunc getFunc)
    {
        std::vector<std::string> cmdVec = Awg::splitScpiPattern(scpi);
        FunctionWrapper* setWrapper = new FunctionWrapper(setFunc);
        FunctionWrapper* getWrapper = new FunctionWrapper(getFunc);

        root->addScpiNode(cmdVec,setWrapper,getWrapper);
    }
/**将StringConvertor中的数组转换结果改成(@1,2,3)这样的形式,就可以适配SCPI中的数组了*/
    std::string excute(const std::string& scpi)
    {

        //将字符串拆解为指令和参数两部分
        const std::vector<std::string>& extraArgs = root->getExtraArgs();
        std::vector<std::string> list = Awg::split(scpi," ");

        std::vector<std::string> inputArgs{};
        if(list.size() > 1)
            inputArgs = Awg::splitArgs(list.at(1));

        std::vector<std::string> cmds = Awg::split(list.at(0),":");
        FunctionWrapper* func = root->parse(cmds);
        if(func)
        {
            //将指令解析过程中的参数添加到输入参数的前面
            inputArgs.insert(inputArgs.begin(), extraArgs.begin(), extraArgs.end());
            //执行目标函数
            if(func->execString(inputArgs))
                return func->getResultString();
            else
                return "target func exec fail";
        }
        return "no match function has been found";
    }

private:
    ScpiParseNode* root = nullptr;
};

class Object
{
public:
    void func(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
};

static void func1(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
static void func2(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
static void func3(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
static void func4(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
static void func5(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
static void func6(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
static void test()
{
    Object obj;
    ScpiParseTree tree;

    // 注册复杂路径命令
    tree.registeScpi("A[:B][:C]:D", func1, nullptr);          // 两个连续可选
    tree.registeScpi("X[:Y]:Z[:W]:T", func2, nullptr);        // 中间有可选
    tree.registeScpi("P[:Q][:R]:S[:T]:U", func3, nullptr);    // 多级嵌套
    tree.registeScpi("M[:N]:O[:P]:Q[:R]", func4, nullptr);    // 交替可选
    tree.registeScpi("G[:H]:I[:J][:K][:L]", &Object::func, nullptr,&obj);    // 复杂组合

    // 对应的测试输入
    tree.excute("A:D");             // 全部省略
    tree.excute("A:B:D");           // 包含B
    tree.excute("A:C:D");           // 包含C
    tree.excute("A:B:C:D ");         // 全部包含

    tree.excute("X:Z:T");           // 省略Y和W
    tree.excute("X:Y:Z:W:T ");       // 全部包含

    tree.excute("P:S:U ");           // 全省略
    tree.excute("P:Q:S:T:U ");       // 包含Q和T

    tree.excute("M:O:Q 9");           // 全省略
    tree.excute("M:N:O:P:Q:R ");    // 全包含

    tree.excute("G:I ");            // 全省略
    tree.excute("G:H:I:J ");        // 最后两级省略
}
#endif // SCPIPARSETREE_H
