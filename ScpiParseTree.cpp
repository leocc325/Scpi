#include "ScpiParseTree.h"

ScpiParseTree::ScpiParseTree()
{
    root = new ScpiParseNode("Root",nullptr,nullptr);
}

ScpiParseTree::~ScpiParseTree()
{

}

std::string ScpiParseTree::excute(const std::string &scpi)
{
    if(scpi.empty())
        return "empty scpi command";

    const std::vector<std::string>& extraArgs = Awg::getExtraArgs();
    std::vector<std::string> list = Awg::split(scpi,' ');

    std::vector<std::string> inputArgs{};
    if(list.size() > 1)
        inputArgs = Awg::splitArgs(list.at(1));

    std::vector<std::string> cmds = Awg::split(list.at(0),':');
    FunctionWrapper* func = root->parse(cmds);
    if(func)
    {
        //将指令解析过程中的额外参数添加到输入参数的前面
        inputArgs.insert(inputArgs.begin(), extraArgs.begin(), extraArgs.end());
        //执行目标函数
        if(func->execString(inputArgs))
            return func->getResultString();
        else
        {
            std::cerr<<"target func exec fail"<<std::endl;
            return "target func exec fail";
        }

    }
    std::cerr<<"no match function has been found"<<std::endl;
    return "no match function has been found";
}
