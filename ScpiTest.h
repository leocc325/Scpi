#ifndef SCPITEST_H
#define SCPITEST_H

#include "ScpiParseTree.h"

class Object
{
public:
    void func(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
};

static void func1(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
static void func2(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
static void func3(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
static void func4(){std::cerr<<__PRETTY_FUNCTION__<<std::endl;}
static void func5(int a,int b){std::cerr<<__PRETTY_FUNCTION__<<a<<b<<std::endl;}
static int func6(int a){std::cerr<<__PRETTY_FUNCTION__<<a<<std::endl;return a;}

static void test()
{
    Object obj;
    ScpiParseTree tree;

    tree.registeScpi("A:B[:C]", func1, nullptr);
    tree.registeScpi("A:B[:C]:D", func1, nullptr);

    // 注册复杂路径命令
    tree.registeScpi("A[:B][:C]:D", func1, nullptr);          // 两个连续可选
    tree.registeScpi("X[:Y]:Z[:W]:T", func2, nullptr);        // 中间有可选
    tree.registeScpi("P[:Q][:R]:S[:T]:U", func3, nullptr);    // 多级嵌套
    tree.registeScpi("M[:N]:O[:P]:Q[:R]", func4, nullptr);    // 交替可选
    tree.registeScpi("G[:H]:I[:J][:K][:L]", &Object::func, nullptr,&obj);    // 复杂组合
    tree.registeScpi("U:V[:W]:X[<n>]:Y", func5, func6);    // 节点存在可省略默认参数的设置和读取测试

    // 对应的测试输入
    tree.excute("A:D");             // 全部省略
    tree.excute("A:B:D");           // 包含B
    tree.excute("A:C:D");           // 包含C
    tree.excute("A:B:C:D");         // 全部包含

    tree.excute("X:Z:T");           // 省略Y和W
    tree.excute("X:Y:Z:W:T ");       // 全部包含

    tree.excute("P:S:U ");           // 全省略
    tree.excute("P:Q:S:T:U");       // 包含Q和T

    tree.excute("M:O:Q");           // 全省略
    tree.excute("M:N:O:P:Q:R");    // 全包含

    tree.excute("G:I");            // 全省略
    tree.excute("G:H:I:J");        // 最后两级省略

    tree.excute("U:V:X10:Y 20");
    std::string s = tree.excute("U:V:X30:Y?");
}


// 4.9.1 电流幅值设置/查询
static void SOURce_CURRent_AMPLitude_Set(int channel, double value) {
    std::cerr << "SOURce_CURRent_AMPLitude_Set" << channel  << value << "A" << std::endl;
}

static double SOURce_CURRent_AMPLitude_Query(int channel) {
    std::cerr << "SOURce_CURRent_AMPLitude_Query" << channel  << std::endl;
    return 1.0; // 默认返回值
}

// 4.9.2 电流步进设置
static void SOURce_CURRent_STEP_Set(int channel, double step) {
    std::cerr << "SOURce_CURRent_STEP_Set" << channel << step << "A" << std::endl;
}

// 4.9.3 清除电流保护
static void SOURce_CURRent_PROTection_CLEar(int channel) {
    std::cerr << "SOURce_CURRent_PROTection_CLEar" << channel << std::endl;
}

// 4.9.4 电流保护电平设置/查询
static void SOURce_CURRent_PROTection_LEVel_Set(int channel, double level) {
    std::cerr << "SOURce_CURRent_PROTection_LEVel_Set" << channel << level << "A" << std::endl;
}

static double SOURce_CURRent_PROTection_LEVel_Query(int channel) {
    std::cerr << "SOURce_CURRent_PROTection_LEVel_Query" << channel << std::endl;
    return 2.0; // 默认返回值
}

// 4.9.5 电流保护状态设置/查询
static void SOURce_CURRent_PROTection_STATE_Set(int channel, bool state) {
    std::cerr << "SOURce_CURRent_PROTection_STATE_Set" << channel<< (state ? "ON" : "OFF") << std::endl;
}

static bool SOURce_CURRent_PROTection_STATE_Query(int channel) {
    std::cerr << "SOURce_CURRent_PROTection_STATE_Query" << channel << std::endl;
    return true; // 默认返回值
}

// 4.9.6 查询电流保护是否触发
static bool SOURce_CURRent_PROTection_TRIPped_Query(int channel) {
    std::cerr << "SOURce_CURRent_PROTection_TRIPped_Query" << channel << std::endl;
    return false; // 默认返回值
}

// 4.9.7 电压幅值设置/查询
static void SOURce_VOLTage_AMPLitude_Set(int channel, double value) {
    std::cerr << "SOURce_VOLTage_AMPLitude_Set" << channel << "voltage: " << value << "V" << std::endl;
}

static double SOURce_VOLTage_AMPLitude_Query(int channel) {
    std::cerr << "SOURce_VOLTage_AMPLitude_Query" << channel << std::endl;
    return 5.0; // 默认返回值
}

// 4.9.8 电压步进设置
static void SOURce_VOLTage_STEP_Set(int channel, double step) {
    std::cerr << "SOURce_VOLTage_STEP_Set" << channel <<  step << "V" << std::endl;
}

// 4.9.9 清除电压保护
static void SOURce_VOLTage_PROTection_CLEar(int channel) {
    std::cerr << "SOURce_VOLTage_PROTection_CLEar" << channel << std::endl;
}

// 4.9.10 电压保护电平设置/查询
static void SOURce_VOLTage_PROTection_LEVel_Set(int channel, double level) {
    std::cerr << "SOURce_VOLTage_PROTection_LEVel_Set" << channel<< level << "V" << std::endl;
}

static double SOURce_VOLTage_PROTection_LEVel_Query(int channel) {
    std::cerr << "SOURce_VOLTage_PROTection_LEVel_Query" << channel << std::endl;
    return 10.0; // 默认返回值
}

// 4.9.11 电压保护状态设置/查询
static void SOURce_VOLTage_PROTection_STATE_Set(int channel, bool state) {
    std::cerr << "SOURce_VOLTage_PROTection_STATE_Set" << channel << (state ? "ON" : "OFF") << std::endl;
}

static bool SOURce_VOLTage_PROTection_STATE_Query(int channel) {
    std::cerr << "SOURce_VOLTage_PROTection_STATE_Query" << channel<< std::endl;
    return true; // 默认返回值
}

// 4.9.12 查询电压保护是否触发
static bool SOURce_VOLTage_PROTection_TRIPped_Query(int channel) {
    std::cerr << "SOURce_VOLTage_PROTection_TRIPped_Query" << channel << "protect" << std::endl;
    return false; // 默认返回值
}

// ============================================================================
// :STATus 命令子系统 (4.10)
// ============================================================================

// 4.10.1 查询操作状态条件寄存器
static uint16_t STATus_OPERation_CONDition_Query() {
    std::cerr << "STATus_OPERation_CONDition_Query" << std::endl;
    return 0x0000; // 默认返回值
}

// 4.10.2 设置操作状态使能寄存器
static void STATus_OPERation_ENABLE_Set(uint16_t value) {
    std::cerr << "STATus_OPERation_ENABLE_Set: 0x" << std::hex << value << std::dec << std::endl;
}

// 4.10.3 查询操作状态事件寄存器
static uint16_t STATus_OPERation_EVENT_Query() {
    std::cerr << "STATus_OPERation_EVENT_Query" << std::endl;
    return 0x0000; // 默认返回值
}

// 4.10.4 状态寄存器预置
static void STATus_PRESet() {
    std::cerr << "STATus_PRESet" << std::endl;
}

// 4.10.5 设置可疑状态使能寄存器
static void STATus_QUESTionable_ENABLE_Set(uint16_t value) {
    std::cerr << "STATus_QUESTionable_ENABLE_Set: 0x" << std::hex << value << std::dec << std::endl;
}

// 4.10.6 查询可疑状态事件寄存器
static uint16_t STATus_QUESTionable_EVENT_Query() {
    std::cerr << "STATus_QUESTionable_EVENT_Query" << std::endl;
    return 0x0000; // 默认返回值
}

// 4.10.7 设置仪器可疑状态使能寄存器
static void STATus_QUESTionable_INstrument_ENABLE_Set(uint16_t value) {
    std::cerr << "STATus_QUESTionable_INstrument_ENABLE_Set: 0x" << std::hex << value << std::dec << std::endl;
}

// 4.10.8 查询仪器可疑状态事件寄存器
static uint16_t STATus_QUESTionable_INstrument_EVENT_Query() {
    std::cerr << "STATus_QUESTionable_INstrument_EVENT_Query" << std::endl;
    return 0x0000; // 默认返回值
}

// 4.10.9 查询仪器可疑状态摘要条件寄存器
static uint8_t STATus_QUESTionable_INstrument_SUMMARY_CONDition_Query(int n) {
    std::cerr << "STATus_QUESTionable_INstrument_SUMMARY_CONDition_Query" << n << "reg" << std::endl;
    return 0x00; // 默认返回值
}

// 4.10.10 设置仪器可疑状态摘要使能寄存器
static void STATus_QUESTionable_INstrument_SUMMARY_ENABLE_Set(int n, uint8_t value) {
    std::cerr << "STATus_QUESTionable_INstrument_SUMMARY_ENABLE_Set" << n << "reg: 0x" << std::hex << (int)value << std::dec << std::endl;
}

// 4.10.11 查询仪器可疑状态摘要事件寄存器
static uint8_t STATus_QUESTionable_INstrument_SUMMARY_EVENT_Query(int n) {
    std::cerr << "STATus_QUESTionable_INstrument_SUMMARY_EVENT_Query" << n << "event" << std::endl;
    return 0x00; // 默认返回值
}

// ============================================================================
// :SYSTem 命令子系统 (4.11)
// ============================================================================

// 4.11.1 立即蜂鸣器
static void SYSTem_BEEPer_IMMediate() {
    std::cerr << "SYSTem_BEEPer_IMMediate" << std::endl;
}

// 4.11.2 设置蜂鸣器状态
static void SYSTem_BEEPer_STATE_Set(bool state) {
    std::cerr << "SYSTem_BEEPer_STATE_Set: " << (state ? "ON" : "OFF") << std::endl;
}

// 4.11.3 设置亮度
static void SYSTem_BRIGHTness_Set(int level) {
    std::cerr << "SYSTem_BRIGHTness_Set: " << level << std::endl;
}

// 4.11.4.1 应用LAN设置
static void SYSTem_COMMunicate_LAN_APPLy() {
    std::cerr << "SYSTem_COMMunicate_LAN_APPLy" << std::endl;
}

// 4.11.4.2 设置自动IP状态
static void SYSTem_COMMunicate_LAN_AUTOip_STATE_Set(bool state) {
    std::cerr << "SYSTem_COMMunicate_LAN_AUTOip_STATE_Set: " << (state ? "ON" : "OFF") << std::endl;
}

// 4.11.4.3 设置DHCP状态
static void SYSTem_COMMunicate_LAN_DHCP_STATE_Set(bool state) {
    std::cerr << "SYSTem_COMMunicate_LAN_DHCP_STATE_Set: " << (state ? "ON" : "OFF") << std::endl;
}

// 4.11.4.4 设置DNS服务器
static void SYSTem_COMMunicate_LAN_DNS_Set(const std::string& dns) {
    std::cerr << "SYSTem_COMMunicate_LAN_DNS_Set: " << dns << std::endl;
}

// 4.11.4.5 设置IP地址
static void SYSTem_COMMunicate_LAN_IPADdress_Set(const std::string& ip) {
    std::cerr << "SYSTem_COMMunicate_LAN_IPADdress_Set: " << ip << std::endl;
}

// 4.11.4.6 设置网关
static void SYSTem_COMMunicate_LAN_GATEway_Set(const std::string& gateway) {
    std::cerr << "设置网关: " << gateway << std::endl;
}

// 4.11.4.7 查询MAC地址
static std::string SYSTem_COMMunicate_LAN_MAC_Query() {
    std::cerr << "SYSTem_COMMunicate_LAN_MAC_Query" << std::endl;
    return "00:1A:2B:3C:4D:5E"; // 默认返回值
}

// 4.11.4.8 设置手动IP状态
static void SYSTem_COMMunicate_LAN_MANualip_STATE_Set(bool state) {
    std::cerr << "SYSTem_COMMunicate_LAN_MANualip_STATE_Set: " << (state ? "ON" : "OFF") << std::endl;
}

// 4.11.4.9 设置子网掩码
static void SYSTem_COMMunicate_LAN_SMASK_Set(const std::string& mask) {
    std::cerr << "SYSTem_COMMunicate_LAN_SMASK_Set: " << mask << std::endl;
}

// 4.11.5 设置远程/本地状态
static void SYSTem_COMMunicate_RLState_Set(bool remote) {
    std::cerr << "SYSTem_COMMunicate_RLState_Set: " << (remote ? "REMOTE" : "LOCAL") << std::endl;
}

// 4.11.6 查询下一个错误
static std::string SYSTem_ERROR_NEXT_Query() {
    std::cerr << "SYSTem_ERROR_NEXT_Query" << std::endl;
    return "0,\"No error\""; // 默认返回值
}

// 4.11.7 设置键盘锁定状态
static void SYSTem_KLOCK_STATE_Set(bool state) {
    std::cerr << "SYSTem_KLOCK_STATE_Set: " << (state ? "LOCKED" : "UNLOCKED") << std::endl;
}

// 4.11.8 设置语言类型
static void SYSTem_LANGuage_TYPE_Set(const std::string& language) {
    std::cerr << "SYSTem_LANGuage_TYPE_Set: " << language << std::endl;
}

// 4.11.9 切换到本地模式
static void SYSTem_LOCal() {
    std::cerr << "SYSTem_LOCal" << std::endl;
}

// 4.11.10 查询上电状态
static bool SYSTem_POWEron_Query() {
    std::cerr << "SYSTem_POWEron_Query" << std::endl;
    return true; // 默认返回值
}

// 4.11.11 查询打印状态
static std::string SYSTem_PRINT_Query() {
    std::cerr << "SYSTem_PRINT_Query" << std::endl;
    return "Print status"; // 默认返回值
}

// 4.11.12 切换到远程模式
static void SYSTem_REMote() {
    std::cerr << "SYSTem_REMote" << std::endl;
}

// 4.11.13 设置读写锁定
static void SYSTem_RWLock_Set(bool state) {
    std::cerr << "SYSTem_RWLock_Set: " << (state ? "LOCKED" : "UNLOCKED") << std::endl;
}

// 4.11.14 设置屏幕保护
static void SYSTem_SAVer_Set(int mode) {
    std::cerr << "SYSTem_SAVer_Set: " << mode << std::endl;
}

// 4.11.15 设置同步状态
static void SYSTem_SYNC_STATE_Set(bool state) {
    std::cerr << "SYSTem_SYNC_STATE_Set: " << (state ? "ON" : "OFF") << std::endl;
}

// 4.11.16 设置时间模式
static void SYSTem_TMODE_Set(int mode) {
    std::cerr << "SYSTem_TMODE_Set: " << mode << std::endl;
}

// 4.11.17 设置时间锁定
static void SYSTem_TLOCK_Set(bool state) {
    std::cerr << "SYSTem_TLOCK_Set: " << (state ? "LOCKED" : "UNLOCKED") << std::endl;
}

// 4.11.18 查询系统版本
static std::string SYSTem_VERSION_Query() {
    std::cerr << "SYSTem_VERSION_Query" << std::endl;
    return "DP900 V1.0.0"; // 默认返回值
}

void registerSourceCommands(ScpiParseTree& tree) {
    // 4.9.1 电流设置/查询
    tree.registeScpi(":SOURce[<n>]:CURRent[:LEVel][:IMMediate]:AMPLitude",
                     SOURce_CURRent_AMPLitude_Set,
                     SOURce_CURRent_AMPLitude_Query);

    // 4.9.2 电流步进设置
    tree.registeScpi(":SOURce[<n>]:CURRent[:LEVel][:IMMediate]:STEP[:INCRement]",
                     SOURce_CURRent_STEP_Set,
                     nullptr);

    // 4.9.3 清除电流保护
    tree.registeScpi(":SOURce[<n>]:CURRent:PROTection:CLEar",
                     SOURce_CURRent_PROTection_CLEar,
                     nullptr);

    // 4.9.4 电流保护电平设置/查询
    tree.registeScpi(":SOURce[<n>]:CURRent:PROTection[:LEVel]",
                     SOURce_CURRent_PROTection_LEVel_Set,
                     SOURce_CURRent_PROTection_LEVel_Query);

    // 4.9.5 电流保护状态设置
    tree.registeScpi(":SOURce[<n>]:CURRent:PROTection:STATE",
                     SOURce_CURRent_PROTection_STATE_Set,
                     SOURce_CURRent_PROTection_STATE_Query);

    // 4.9.6 查询电流保护是否触发
    tree.registeScpi(":SOURce[<n>]:CURRent:PROTection:TRIPped?",
                     nullptr,
                     SOURce_CURRent_PROTection_TRIPped_Query);

    // 4.9.7 电压设置/查询
    tree.registeScpi(":SOURce[<n>]:VOLTage[:LEVel][:IMMediate]:AMPLitude",
                     SOURce_VOLTage_AMPLitude_Set,
                     SOURce_VOLTage_AMPLitude_Query);

    // 4.9.8 电压步进设置
    tree.registeScpi(":SOURce[<n>]:VOLTage[:LEVel][:IMMediate]:STEP[:INCRement]",
                     SOURce_VOLTage_STEP_Set,
                     nullptr);

    // 4.9.9 清除电压保护
    tree.registeScpi(":SOURce[<n>]:VOLTage:PROTection:CLEar",
                     SOURce_VOLTage_PROTection_CLEar,
                     nullptr);

    // 4.9.10 电压保护电平设置/查询
    tree.registeScpi(":SOURce[<n>]:VOLTage:PROTection[:LEVel]",
                     SOURce_VOLTage_PROTection_LEVel_Set,
                     SOURce_VOLTage_PROTection_LEVel_Query);

    // 4.9.11 电压保护状态设置
    tree.registeScpi(":SOURce[<n>]:VOLTage:PROTection:STATE",
                     SOURce_VOLTage_PROTection_STATE_Set,
                     SOURce_VOLTage_PROTection_STATE_Query);

    // 4.9.12 查询电压保护是否触发
    tree.registeScpi(":SOURce[<n>]:VOLTage:PROTection:TRIPped?",
                     nullptr,
                     SOURce_VOLTage_PROTection_TRIPped_Query);
}

void registerStatusCommands(ScpiParseTree& tree) {
    // 4.10.1 查询操作状态条件寄存器
    tree.registeScpi(":STATus:OPERation:CONDition?",
                     nullptr,
                     STATus_OPERation_CONDition_Query);

    // 4.10.2 设置操作状态使能寄存器
    tree.registeScpi(":STATus:OPERation:ENABLE",
                     STATus_OPERation_ENABLE_Set,  // 需要参数
                     nullptr);

    // 4.10.3 查询操作状态事件寄存器
    tree.registeScpi(":STATus:OPERation[:EVENt]?",
                     nullptr,
                     STATus_OPERation_EVENT_Query);

    // 4.10.4 状态寄存器预置
    tree.registeScpi(":STATus:PRESet",
                     STATus_PRESet,
                     nullptr);

    // 4.10.5 设置可疑状态使能寄存器
    tree.registeScpi(":STATus:QUESTionable:ENABLE",
                     STATus_QUESTionable_ENABLE_Set,  // 需要参数
                     nullptr);

    // 4.10.6 查询可疑状态事件寄存器
    tree.registeScpi(":STATus:QUESTionable[:EVENt]?",
                     nullptr,
                     STATus_QUESTionable_EVENT_Query);

    // 4.10.7 设置仪器可疑状态使能寄存器
    tree.registeScpi(":STATus:QUESTionable:INstrument:ENABLE",
                     STATus_QUESTionable_INstrument_ENABLE_Set,  // 需要参数
                     nullptr);

    // 4.10.8 查询仪器可疑状态事件寄存器
    tree.registeScpi(":STATus:QUESTionable:INstrument[:EVENt]?",
                     nullptr,
                     STATus_QUESTionable_INstrument_EVENT_Query);

    // 4.10.9 查询仪器可疑状态摘要条件寄存器
    tree.registeScpi(":STATus:QUESTionable:INstrument:SUMmary[<n>]:CONDition",
                     nullptr,
                     STATus_QUESTionable_INstrument_SUMMARY_CONDition_Query); // 需要参数

    // 4.10.10 设置仪器可疑状态摘要使能寄存器
    tree.registeScpi(":STATus:QUESTionable:INstrument:SUMmary[<n>]:ENABLE",
                     STATus_QUESTionable_INstrument_SUMMARY_ENABLE_Set,  // 需要参数
                     nullptr);

    // 4.10.11 查询仪器可疑状态摘要事件寄存器
    tree.registeScpi(":STATus:QUESTionable:INstrument:SUMmary[<n>][:EVENt]",
                     nullptr,
                     STATus_QUESTionable_INstrument_SUMMARY_EVENT_Query); // 需要参数
}

void registerSystemCommands(ScpiParseTree& tree) {
    // 4.11.1 立即蜂鸣器
    tree.registeScpi(":SYSTem:BEEPer:IMMediate",
                     SYSTem_BEEPer_IMMediate,
                     nullptr);

    // 4.11.2 设置蜂鸣器状态
    tree.registeScpi(":SYSTem:BEEPer[:STATE]",
                     SYSTem_BEEPer_STATE_Set,
                     nullptr);

    // 4.11.3 设置亮度
    tree.registeScpi(":SYSTem:BRIGHTness",
                     SYSTem_BRIGHTness_Set,  // 需要参数
                     nullptr);

    // 4.11.4.1 应用LAN设置
    tree.registeScpi(":SYSTem:COMMunicate:LAN:APPLy",
                     SYSTem_COMMunicate_LAN_APPLy,
                     nullptr);

    // 4.11.4.2 设置自动IP状态
    tree.registeScpi(":SYSTem:COMMunicate:LAN:AUTOip[:STATE]",
                     SYSTem_COMMunicate_LAN_AUTOip_STATE_Set,
                     nullptr);

    // 4.11.4.3 设置DHCP状态
    tree.registeScpi(":SYSTem:COMMunicate:LAN:DHCP[:STATE]",
                     SYSTem_COMMunicate_LAN_DHCP_STATE_Set,
                     nullptr);

    // 4.11.4.4 设置DNS服务器
    tree.registeScpi(":SYSTem:COMMunicate:LAN:DNS",
                     SYSTem_COMMunicate_LAN_DNS_Set,  // 需要参数
                     nullptr);

    // 4.11.4.5 设置IP地址
    tree.registeScpi(":SYSTem:COMMunicate:LAN:IPADdress",
                     SYSTem_COMMunicate_LAN_IPADdress_Set,  // 需要参数
                     nullptr);

    // 4.11.4.6 设置网关
    tree.registeScpi(":SYSTem:COMMunicate:LAN:GATEway",
                     SYSTem_COMMunicate_LAN_GATEway_Set,  // 需要参数
                     nullptr);

    // 4.11.4.7 查询MAC地址
    tree.registeScpi(":SYSTem:COMMunicate:LAN:MAC?",
                     nullptr,
                     SYSTem_COMMunicate_LAN_MAC_Query);

    // 4.11.4.8 设置手动IP状态
    tree.registeScpi(":SYSTem:COMMunicate:LAN:MANualip[:STATE]",
                     SYSTem_COMMunicate_LAN_MANualip_STATE_Set,
                     nullptr);

    // 4.11.4.9 设置子网掩码
    tree.registeScpi(":SYSTem:COMMunicate:LAN:SMASK",
                     SYSTem_COMMunicate_LAN_SMASK_Set,  // 需要参数
                     nullptr);

    // 4.11.5 设置远程/本地状态
    tree.registeScpi(":SYSTem:COMMunicate:RLState",
                     SYSTem_COMMunicate_RLState_Set,
                     nullptr);

    // 4.11.6 查询下一个错误
    tree.registeScpi(":SYSTem:ERROR[:NEXT]?",
                     nullptr,
                     SYSTem_ERROR_NEXT_Query);

    // 4.11.7 设置键盘锁定状态
    tree.registeScpi(":SYSTem:KLOCK:STATE",
                     SYSTem_KLOCK_STATE_Set,
                     nullptr);

    // 4.11.8 设置语言类型
    tree.registeScpi(":SYSTem:LANGuage:TYPE",
                     SYSTem_LANGuage_TYPE_Set,  // 需要参数
                     nullptr);

    // 4.11.9 切换到本地模式
    tree.registeScpi(":SYSTem:LOCal",
                     SYSTem_LOCal,
                     nullptr);

    // 4.11.10 查询上电状态
    tree.registeScpi(":SYSTem:POWEron?",
                     nullptr,
                     SYSTem_POWEron_Query);

    // 4.11.11 查询打印状态
    tree.registeScpi(":SYSTem:PRINT?",
                     nullptr,
                     SYSTem_PRINT_Query);

    // 4.11.12 切换到远程模式
    tree.registeScpi(":SYSTem:REMote",
                     SYSTem_REMote,
                     nullptr);

    // 4.11.13 设置读写锁定
    tree.registeScpi(":SYSTem:RWLock",
                     SYSTem_RWLock_Set,
                     nullptr);

    // 4.11.14 设置屏幕保护
    tree.registeScpi(":SYSTem:SAVer",
                     SYSTem_SAVer_Set,  // 需要参数
                     nullptr);

    // 4.11.15 设置同步状态
    tree.registeScpi(":SYSTem:SYNC[:STATE]",
                     SYSTem_SYNC_STATE_Set,
                     nullptr);

    // 4.11.16 设置时间模式
    tree.registeScpi(":SYSTem:TMODE",
                     SYSTem_TMODE_Set,  // 需要参数
                     nullptr);

    // 4.11.17 设置时间锁定
    tree.registeScpi(":SYSTem:TLOCK",
                     SYSTem_TLOCK_Set,
                     nullptr);

    // 4.11.18 查询系统版本
    tree.registeScpi(":SYSTem:VERSION?",
                     nullptr,
                     SYSTem_VERSION_Query);
}

#include <QElapsedTimer>
#include <QDebug>
void testV2()
{
    ScpiParseTree tree;

    // 注册所有命令
    registerSourceCommands(tree);
    registerStatusCommands(tree);
    registerSystemCommands(tree);
    QElapsedTimer timer;
    timer.start();
    // 测试一些命令
    tree.excute(":SOURce1:CURRent:PROTection:CLEar");
    std::string result1 = tree.excute(":SOURce2:CURRent:PROTection:TRIPped?");

    tree.excute(":STATus:PRESet");
    std::string result2 = tree.excute(":STATus:OPERation:CONDition?");

    tree.excute(":SYSTem:BEEPer:IMMediate");
    std::string result3 = tree.excute(":SYSTem:VERSION?");
    std::string result4 = tree.excute(":SYSTem:COMMunicate:LAN:MAC?");
     qDebug()<<timer.elapsed();
}
#endif // SCPITEST_H
