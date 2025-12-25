#include "ScpiParseNode.h"

std::vector<std::string> ScpiParseNode::extraArgs;

ScpiParseNode::ScpiParseNode()
{

}

ScpiParseNode::ScpiParseNode(const std::string& regCmd, FunctionWrapper *setter, FunctionWrapper *getter)
{
    nodeCommand = regCmd;
    setFunc = setter;
    getFunc = getter;
}

void ScpiParseNode::addScpiNode(std::vector<std::string> &regCmdVec, FunctionWrapper *setter, FunctionWrapper *getter)
{
    if(regCmdVec.empty())
        return;

    //取出当前指令并移除
    std::string cmd = regCmdVec.front();
    regCmdVec.erase(regCmdVec.begin());

    //先查找是否存在这条子命令对应的节点,如果存在则直接使用,否则添加
    ScpiParseNode* node = findDirectChildNode(cmd);
    //如果节点不存在则新增一个
    if(node == nullptr)
    {
        //仅仅在这是最后一集指令的时候才给函数指针赋值,不是最后一级指令的节点不保留函数调用指针
        if(regCmdVec.empty())
            node = new ScpiParseNode(cmd,setter,getter);
        else
            node = new ScpiParseNode(cmd,nullptr,nullptr);

        allDirectNodes.push_back(node);
        if(node->omissible())
            omissibleNodes.push_back(node);
    }

    //递归调用这个函数,直到所有子命令都生成对应的节点
    node->addScpiNode(regCmdVec,setter,getter);
}

ScpiParseNode *ScpiParseNode::findNode(std::vector<std::string> &regCmd)
{

}

ScpiParseNode *ScpiParseNode::findDirectChildNode(const std::string &regCmd)
{
    for(ScpiParseNode* node : allDirectNodes)
    {
        if(node->nodeCommand == regCmd)
            return node;
    }
    return nullptr;
}

FunctionWrapper* ScpiParseNode::parse(const std::vector<std::string> &cmds)
{
    extraArgs.clear();//每一次解析完整指令前前清除上一次解析得到的额外参数

    std::vector<std::string>::const_iterator cmdBeg = cmds.cbegin();
    std::vector<std::string>::const_iterator cmdEnd = cmds.cend();

    ScpiParseNode* node = parseNodes(cmdBeg,cmdEnd);

    if(node == nullptr)
        return nullptr;
    else
        return Awg::isQueryScpi(*cmds.crbegin()) ? node->getFunc : node->setFunc;
}

const std::vector<std::string> &ScpiParseNode::getExtraArgs()
{
    return extraArgs;
}

bool ScpiParseNode::hasChildren()
{
    return  !allDirectNodes.empty();
}

bool ScpiParseNode::omissible()
{
    if(nodeCommand.empty())
        return false;
    else
        return (nodeCommand[0] == '[');
}

ScpiParseNode *ScpiParseNode::match(const std::string &inputCmd)
{
    if(Awg::scpiMatch(nodeCommand,inputCmd))
        return this;
    else
        return nullptr;
}

ScpiParseNode *ScpiParseNode::parseNodes(std::vector<std::string>::const_iterator cmdIt, std::vector<std::string>::const_iterator cmdEnd)
{
    /**
     * parseFromAllDirectNodes和parseFromOmissibleNodes传递的都是指令迭代器的引用,而且scpi指令迭代器仅仅在查找到匹配的节点之后才会让迭代器前进1
     * 所以这两个函数返回的节点和scpi指令迭代器是完全匹配的,无需回溯迭代器位置
     */

    ScpiParseNode* node = parseFromAllDirectNodes(cmdIt,cmdEnd);
    if(node->hasChildren())//如果查找到的节点还有子节点,则说明查找到的不是最终节点,需要从当前节点的可省略子节点中继续查找
        node = node->parseFromOmissibleNodes(cmdIt,cmdEnd);
    return node;
}

ScpiParseNode *ScpiParseNode::parseFromAllDirectNodes(std::vector<std::string>::const_iterator& cmdIt,std::vector<std::string>::const_iterator cmdEnd)
{
    /**
     * @brief 只查找当前节点的直接子节点,如果直接子节点中不存在匹配的节点,可能需要查找[可省略节点]的子节点(当前节点的孙节点)。
     * 这个函数查找失败的时候返回当前节点,方便下一阶段的可省略节点查找,可以避免从头重新遍历降低查找效率
     */
    std::list<ScpiParseNode*>::iterator nodeBeg = allDirectNodes.begin();
    std::list<ScpiParseNode*>::iterator nodeEnd = allDirectNodes.end();
    while (nodeBeg != nodeEnd && cmdIt != cmdEnd)
    {
        ScpiParseNode* node = (*nodeBeg)->match(*cmdIt);
        if(node)
        {
            //指令长度可以超过解析树分支长度,只要当前节点是最后一个节点即返回调用函数
            //因为只有最后一个节点保存了调用函数的指针,所以即使提前返回了也返回的是空指针
            if(node->hasChildren())
                return node->parseFromAllDirectNodes(++cmdIt,cmdEnd);//存在子节点意味着需要继续递归查找
            else
                return node;//没有子节点意味着这里就是目标节点
        }
        ++nodeBeg;
    }

    if(cmdIt == cmdEnd)//这里需要处理最后一级或者最后多级指令可省略的情况
        return parseOmissibleBranch();

    return this;
}

ScpiParseNode *ScpiParseNode::parseFromOmissibleNodes(std::vector<std::string>::const_iterator& cmdIt, std::vector<std::string>::const_iterator cmdEnd)
{
    /**
     * @brief 这里查找的是当前节点的孙节点,即当前节点的可省略子节点的子节点,这个函数查找失败返回空指针,表明当前节点内确实不存在匹配的子节点
     * 这个函数在某种角度上和parseFromAllDirectNodes是一样的,都是遍历节点内部的全部子节点,区别在于:
     * parseFromAllDirectNodes只有在当前节点的全部子节点有一个匹配成功的情况选才会开始下一次递归匹配
     * parseFromOmissibleNodes遍历全部子节点匹配失败之后,还会继续遍历[可省略子节点]下面的全部子节点,直到返回匹配节点或者空指针,多了N层递归
     */
    ScpiParseNode* node = nullptr;

    std::list<ScpiParseNode*>::iterator nodeBeg = omissibleNodes.begin();
    std::list<ScpiParseNode*>::iterator nodeEnd = omissibleNodes.end();
    while(nodeBeg != nodeEnd && cmdIt != cmdEnd)
    {
        node = (*nodeBeg)->parseNodes(cmdIt,cmdEnd);
        if(node)
            return node;

        ++nodeBeg;
    }

    return nullptr;
}

ScpiParseNode *ScpiParseNode::parseOmissibleBranch()
{
    if(this->omissible() && !this->hasChildren())//如果当前节点是最后一个节点而且是可省略的,就直接返回这个节点
        return this;//注:在指令注册的时候就判断了同一个节点下不会有两个可省略的最终节点,所以这里返回的节点一定是正确的
    else
    {
        if(this->omissibleNodes.size() == 1)//否则判断当前节点是否存在唯一可省略子节点
            return this->omissibleNodes.front()->parseOmissibleBranch();//对这个唯一子节点递归查找
        else
            return this;
    }
}
