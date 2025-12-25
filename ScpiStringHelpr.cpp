#include "ScpiStringHelpr.h"

///节点中可省略参数或者指定参数的集合
static std::vector<std::string> extraArgs(10);
///注册SCPI检测到可完全的省略子分支时将这条分支的可省略节点对应的指令保存到这个数组中
///关于omissibleCmds的若干操作(添加元素和查找)效率都比较低,但是这些操作都处于程序初始化阶段而且频率不会很高,所以暂时就这样
static std::set<std::string> omissibleCmds;

using StringIt = std::string::const_iterator;

const std::vector<std::string> &Awg::getExtraArgs()
{
    return extraArgs;
}

const std::set<std::string> &Awg::getOmissibleArgs()
{
    return omissibleCmds;
}

void Awg::clearExtraArgs()
{
    extraArgs.clear();
}

bool Awg::isQueryScpi(const std::string &input)
{
    return input.back() == '?';
}

bool Awg::isOmissible(const std::string &input)
{
    if(input.empty())
        return false;
    else
        return (*input.cbegin() == '[') && (*input.crbegin() == ']');
}

std::string splitScpiHelper_Node(StringIt& beg,StringIt& mid,const StringIt& end)
{
    mid = beg + 1;
    while (mid < end)
    {
        //如果是不可省略节点,就一定要查找到下一个:分割符才算是一个完整的节点,然后再判断:分割符前面是不是左方括号,
        //如果是左方括号则说明进入了下一个可省略节点的内部,此时需要将指针位置往后移动一次，此时两个指针之间的字符串才是正确的scpi节点
        if( *mid == ':' )
        {
            if(*(mid-1) == '[')
                --mid;
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
    //开始解析前先清除上一次保存的可连续省略的分支节点信息
    omissibleCmds.clear();

    std::vector<std::string> tokens;
    StringIt beg = input.cbegin();
    StringIt mid = input.cbegin();
    StringIt end = input.cend();
    while (beg < end)
    {
        if( *beg == ':' || std::isalpha(*beg))//如果第一个匹配上的是:或者是字母,就按:的模式查找下一个分隔符:
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

    //逆序查找是否存在可连续省略的节点
    auto omissBeg = tokens.crbegin();
    auto omissEnd = tokens.crend();
    while (omissBeg < omissEnd)
    {
        if(Awg::isOmissible(*omissBeg))
        {
            omissibleCmds.insert(*omissBeg);
            ++omissBeg;
        }
        else
            break;
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

    //判断输入指令是否是查询指令,如果是查询指令则将输入指令的结束指针移动到问号位置
    std::size_t quesPos = inputCmd.find('?');
    if(quesPos != std::string::npos)
        inputEnd = inputBeg + quesPos;

    //判断当前指令是否存在自定义部分,如果存在可省略部分,则将输入字符串的终止指针往前移动
    //跳过数字部分(目前<...>只能被数字替换,暂时还没发现可以用其他字符替换的SCPI指令)
    //所以跳过结尾的数值之后就认为现在的输入指令是完整的scpi
    StringIt inputMid = inputEnd;//不包括<...>对应字符的输入指令的结尾
    if( nodeCmd.find("<") != std::string::npos && nodeCmd.find(">") != std::string::npos)
    {
        while (inputMid > inputBeg)
        {
            if(std::isdigit(*(inputMid-1)))
                --inputMid;
            else
                break;
        }

        //节点参数可省略[:SOURce[<n>]]  节点参数不可省略[:SOURce<n>]
        std::string s(inputMid,inputEnd);
        if(s.empty())
        {
            //如果存在节点参数,但是实际输入的指令中被省略了,那么无论这个节点的参数在原始指令中是否可以被省略,这里都用-999代替,最后在函数接口中处理这个-999
            //原因同上,目前节点参数暂时只有数字而且只能存在一个节点参数,所以这里直接使用-999代替暂时是没有问题的
            extraArgs.push_back("-999");
        }
        else
        {
            //否则直接将输入指令的节点参数添加到额外参数列表中
            extraArgs.push_back(s);
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
