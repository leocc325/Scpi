#ifndef SCPISTRINGHELPR_H
#define SCPISTRINGHELPR_H

#include <string>
#include <vector>
#include <set>

namespace Awg {
    ///返回scpi指令的额外参数
    const std::vector<std::string>& getExtraArgs();

    ///返回可省略分支指令
    const std::set<std::string>& getOmissibleArgs();

    ///清除上一次解析得到的额外参数
    void clearExtraArgs();

    ///指令是查询指令还是设置指令
    bool isQueryScpi(const std::string& input);

    ///指令是否可省略
    bool isOmissible(const std::string& input);

    ///将原始SCPI指令拆分为若干个子命令
    std::vector<std::string> splitScpiPattern(const std::string& input);

    ///按指定字符拆分字符串
    std::vector<std::string> split(const std::string& input,const char spliter);

    ///将字符串拆分为SCPI指令的参数包
    std::vector<std::string> splitArgs(const std::string& input);

    ///判断输入指令是否和当前节点SCPI匹配
    int scpiMatch(const std::string& nodeCmd ,const std::string& inputCmd);
}

#endif // SCPISTRINGHELPR_H
