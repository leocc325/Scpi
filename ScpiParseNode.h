#ifndef SCPIPARSENODE_H
#define SCPIPARSENODE_H

#include "ScpiStringHelpr.h"
#include "FunctionWrapper/FunctionWrapper.hpp"

class ScpiParseNode
{
public:
    ScpiParseNode();

    ScpiParseNode(const std::string& regCmd,FunctionWrapper* setter,FunctionWrapper* getter);

    void addScpiNode(std::vector<std::string>& regCmdVec,FunctionWrapper* setter,FunctionWrapper* getter);

    ///查找是否有这条子命令对应的直接子节点
    ScpiParseNode* findDirectChildNode(const std::string& regCmd);

    ///从子节点中去查找匹配cmds的最后一级节点,从当前节点的直接子节点中去查找对应cmds中的第一条指令
    FunctionWrapper* parse(const std::vector<std::string>& cmds);

private:
    bool hasChildren();

    bool omissible();

    ///使用正则表达式输入指令匹配的节点,是否可以增加一个额外接口用于传入一个自定义节点解析器?
    ScpiParseNode* match(const std::string& inputCmd);

    ///通过输入的scpi指令递归地从所有节点中去匹配对应地节点
    ScpiParseNode* parseNodes(std::vector<std::string>::const_iterator cmdIt,std::vector<std::string>::const_iterator cmdEnd);

    ///通过输入指令查找固定节点
    ScpiParseNode* parseFromAllDirectNodes(std::vector<std::string>::const_iterator& cmdIt,std::vector<std::string>::const_iterator cmdEnd);

    ///通过输入指令查找可省略节点
    ScpiParseNode* parseFromOmissibleNodes(std::vector<std::string>::const_iterator& cmdIt,std::vector<std::string>::const_iterator cmdEnd);

    ///判断当前节点是否存在一条可以省略的N级分支
    ScpiParseNode* parseOmissibleBranch();

private:
    std::string nodeCommand;//当前节点对应的指令
    std::list<ScpiParseNode*> allDirectNodes;//全部直接子节点
    std::list<ScpiParseNode*> omissibleNodes;//全部直接子节点中可省略节点的集合
    FunctionWrapper* setFunc = nullptr;
    FunctionWrapper* getFunc = nullptr;
    bool existOmissibleBranch = false;//当前节点是否存在完全可省略的子分支
};

#endif // SCPIPARSENODE_H
