#ifndef SCPIPARSETREE_H
#define SCPIPARSETREE_H

#include "ScpiParseNode.h"

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

    template <typename SetFunc,typename GetFunc>
    typename std::enable_if<FuncOrNull<SetFunc>::value && FuncOrNull<GetFunc>::value>::type
    registeScpi(const std::string& scpi,SetFunc setFunc,GetFunc getFunc)
    {
        std::vector<std::string> cmdVec = Awg::splitScpiPattern(scpi);
        FunctionWrapper* setWrapper = new FunctionWrapper(setFunc);
        FunctionWrapper* getWrapper = new FunctionWrapper(getFunc);

        root->addScpiNode(cmdVec,setWrapper,getWrapper);
    }

    std::string excute(const std::string& scpi);

private:
    ScpiParseNode* root = nullptr;
};

#endif // SCPIPARSETREE_H
