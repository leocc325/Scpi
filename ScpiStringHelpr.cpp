#include "ScpiStringHelpr.h"

using StringIt = std::string::const_iterator;

bool Awg::isQueryScpi(const std::string &input)
{
    return input.back() == '?';
}

std::string splitScpiHelper_Node(StringIt& beg,StringIt& mid,const StringIt& end)
{
    mid = beg + 1;
    while (mid < end)
    {
        if( *mid == ':' || *mid == '[')
        {
            break;
        }
        ++mid;
    }
    return std::string(beg,mid);
}

std::string splitScpiHelper_omissible(StringIt& beg,StringIt& mid,const StringIt& end)
{
    //每查找到一个'['都让层数计数器+1,直到匹配出全部方括号
    int leftBrackets = 1;
    mid = beg + 1;
    while (mid < end)
    {
        if(*mid == '[')
            ++leftBrackets;
        else if(*mid == ']')
            --leftBrackets;

        ++mid;//break要放到这一步后面,因为迭代器是左闭右开区间

        if(leftBrackets == 0)
            break;
    }
    return std::string(beg,mid);
}

///将SCPI字符串切割为若干个子命令,由于存在可省略指令,所以方括号的匹配必须匹配到最外层
/*std::vector<std::string> Awg::splitScpiPattern(const std::string &input)
{
    std::vector<std::string> tokens;
    StringIt beg = input.cbegin();
    StringIt mid = input.cbegin();
    StringIt end = input.cend();
    while (beg < end)
    {
        if( *beg == ':' || std::isalpha(*beg))//如果第一个匹配上的是:或者是字母,就按:的模式查找下一个分隔符(冒号或者左方括号)
        {
            mid = beg + 1;
            while (mid < end)
            {
                if( *mid == ':' || *mid == '[')
                {
                    break;
                }
                ++mid;
            }
            tokens.push_back(std::string(beg,mid));
            beg = mid;
        }
        else if( (*beg)=='[' )//如果第一个匹配上的是 '[' 就按 ']' 的模式查完整的方括号
        {
            //每查找到一个'['都让层数计数器+1,直到匹配出全部方括号
            int leftBrackets = 1;
            mid = beg + 1;
            while (mid < end)
            {
                if(*mid == '[')
                    ++leftBrackets;
                else if(*mid == ']')
                    --leftBrackets;

                ++mid;//break要放到这一步后面,因为迭代器是左闭右开区间

                if(leftBrackets == 0)
                    break;
            }
            tokens.push_back(std::string(beg,mid));
            beg = mid;
        }
        else
        {
            ++beg;
        }
    }
    return tokens;
}*/

std::vector<std::string> Awg::splitScpiPattern(const std::string &input)
{
    std::vector<std::string> tokens;
    StringIt beg = input.cbegin();
    StringIt mid = input.cbegin();
    StringIt end = input.cend();
    while (beg < end)
    {
        if( *beg == ':' || std::isalpha(*beg))//如果第一个匹配上的是:或者是字母,就按:的模式查找下一个分隔符(冒号或者左方括号)
        {
            std::string s = splitScpiHelper_Node(beg,mid,end);
            tokens.push_back(s);

            beg = mid;
        }
        else if( (*beg)=='[' )//如果第一个匹配上的是 '[' 就按 ']' 的模式查完整的方括号
        {
            std::string s = splitScpiHelper_omissible(beg,mid,end);
            tokens.push_back(s);

            beg = mid;
        }
        else
        {
            ++beg;
        }
    }
    return tokens;
}

std::vector<std::string> Awg::split(const std::string& input,const std::string& spliter)
{
    std::vector<std::string> tokens;
    size_t startPos = 0;
    size_t endPos = input.find(spliter,startPos);

    while (endPos != std::string::npos)
    {
        tokens.push_back(input.substr(startPos, endPos - startPos));
        startPos = endPos + spliter.size();
        endPos = input.find(spliter, startPos);
    }

    // 添加最后一个分隔符后的子串（或最后一个子串）
    tokens.push_back(input.substr(startPos, endPos));

    //如果最后一个字符串为空,则删除最后一个
    if(tokens.back().empty())
        tokens.pop_back();

    return tokens;
}

std::string splitArgsHelper_Array(StringIt& beg,StringIt& mid,const StringIt& end)
{
    while (mid < end)
    {
        if( *mid == ')' )
        {
            ++mid;//匹配到数组终止字符之后还需要将指针位置向后移动一位才能提取完整的数组字符串
            break;
        }
        ++mid;
    }
    return std::string(beg,mid);
}

std::string splitArgsHelper_Element(StringIt& beg,StringIt& mid,const StringIt& end)
{
    while (mid < end)
    {
        if( *mid == ',' )
            break;
        ++mid;
    }
    return std::string(beg,mid);
}

std::vector<std::string> Awg::splitArgs(const std::string &input)
{
    std::vector<std::string> tokens;
    StringIt beg = input.cbegin();
    StringIt mid = input.cbegin();
    StringIt end = input.cend();

    //从字符串开头开始查找','分隔符,但是对于数组需要单独处理
    while(beg < end)
    {
        if(*beg == ',')//如果是分隔符就往后移动一位
        {
            beg++;
        }
        else
        {
            mid = beg;
            if(*beg == '(')//如果以括号开头则表示可能遇到了数组
            {
                std::string array = splitArgsHelper_Array(beg,mid,end);
                tokens.push_back(array);
            }
            else//否则是一个单独的字符串元素
            {
                std::string element = splitArgsHelper_Element(beg,mid,end);
                tokens.push_back(element);
            }
            beg = mid;
        }
    }
    return tokens;
}

int Awg::scpiMatch(const std::string &nodeCmd, const std::string &inputCmd)
{
    if(nodeCmd.empty() || inputCmd.empty())
        return false;

    StringIt inputBeg = inputCmd.cbegin();
    StringIt inputEnd = inputCmd.cend();
    StringIt nodeBeg = nodeCmd.cbegin();
    StringIt nodeEnd = nodeCmd.cend();

    //首先将节点指令的字符指针移动到第一个英文字符位置
    while (nodeBeg < nodeEnd)
    {
        if(std::isalpha(*nodeBeg))
            break;
        ++nodeBeg;
    }

    //同样的,将输入指令的字符指针移动到一个英文字符位置,跳过指令开头的冒号
    while (inputBeg < inputEnd)
    {
        if(std::isalpha(*inputBeg))
            break;
        ++inputBeg;
    }

    StringIt nodeMid = nodeBeg;//指令开头
    StringIt nodeMidu = nodeBeg;//大写结尾
    StringIt nodeMidl = nodeBeg;//小写结尾
    //取出节点的指令部分字符串
    while (nodeMidl < nodeEnd)
    {
        if(!std::isalpha(*nodeMidl))
            break;

        if(std::isupper(*nodeMidl))
            ++nodeMidu;
        ++nodeMidl;
    }
    //现在[nodeBeg,nodeMidu)表示的就是原始指令的大写部分字符串,[nodeBeg,nodeMidl)表示的就是原始指令的大写+小写

    //判断当前指令是否存在自定义部分,如果存在可省略部分,则将输入字符串的终止指针往前移动
    //跳过数字部分(目前<n>只能被数字替换,暂时还没发现可以用其他字符替换的SCPI指令)
    //所以跳过结尾的数值之后就认为现在的输入指令是完整的scpi
    StringIt inputMid = inputEnd;//不包括<n>对应字符的输入指令的结尾
    if( nodeCmd.find("<n>") != std::string::npos )
    {
        while (inputMid > inputBeg)
        {
            if(std::isdigit(*(inputMid-1)))
                --inputMid;
            else
                break;
        }
    }

    //自定义比较函数
    auto cmp = [](StringIt itbeg1,StringIt itend1,StringIt input)->bool
    {
        while (itbeg1 < itend1)
        {
            if(std::tolower(*itbeg1) != std::tolower(*input))
                return false;
            ++itbeg1;
            ++input;
        }
        return true;
    };

    //开始匹配,输入指令要么全等于大小,要么全等于小写,不能作为子字符串判等
    if(std::distance(inputBeg,inputMid) == std::distance(nodeMid,nodeMidl))
        return cmp(nodeMid,nodeMidl,inputBeg);
    else if(std::distance(inputBeg,inputMid) == std::distance(nodeMid,nodeMidu))
        return cmp(nodeMid,nodeMidu,inputBeg);
    else
        return false;
}
