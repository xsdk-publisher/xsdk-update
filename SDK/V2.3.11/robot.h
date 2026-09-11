#ifndef ROBOT_H
#define ROBOT_H

#include "export.h"
#include <any>
#include <functional>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>
namespace HCXSDKLib {

class DLL_EXPORT RobotManager {
public:
    enum TeachCoordinateType {
        TeachCoordinateJoint = 0,     // 关节坐标
        TeachCoordinateWorld = 30,    // 世界坐标
        TeachCoordinateTool = 31,     // 工具坐标
        TeachCoordinateWorkSpace = 32 // 工作台坐标
    };

    enum MoveDirection {
        MoveNegative = -1,
        MovePositive = 1,
    };

    enum CollabDirectionMask {
        CollabDirectionMaskXReverse = 1 << 0,
        CollabDirectionMaskYReverse = 1 << 1,
        CollabDirectionMaskZReverse = 1 << 2,
        CollabDirectionMaskUReverse = 1 << 3,
        CollabDirectionMaskVReverse = 1 << 4,
        CollabDirectionMaskWReverse = 1 << 5,
    };

    /// *************************************************基础函数***************************************************

    /// Function: init_data
    ///     需要首先调用，用户库初始化数据
    /// Parameters:
    ///     localIp  传入本地IP地址接受文件
    ///     remoteIp 下位机IP
    ///     port     端口号
    /// 提示登录成功后可以使用
    static bool init_data(std::string localIp, std::string remoteIp, uint32_t port);

    /// Function: stop_data
    /// 主动停止SDK内部HTTP上传服务与gRPC订阅连接
    /// 结束进程前调用，或在连接异常时调用以释放资源 否则可能会内存泄露崩溃！
    static bool stop_data();

    /// Function: getLinkStatus
    /// 获取当前与下位机连接状态
    /// return:  true - 连接正常   false - 连接异常
    /// 注意：连接异常时，调用其他接口会无效
    static bool getLinkStatus();

    /// Function: setHostCommunicationMode
    /// 设置主机与示教器通信模式
    /// mode: 0-串口 1-以太网
    static bool setHostCommunicationMode(int mode);

    /// Function: getHostCommunicationMode
    /// 获取主机与示教器通信模式
    /// return: 0-串口 1-以太网
    static bool getHostCommunicationMode(int &outMode);

    /// Function: detachHMI
    /// 脱离示教器
    /// 注意:调用函数后需要拔掉示教器
    static bool detachHMI();

    /// Function: getDetachHMIStatus
    /// 获取脱离示教器状态
    /// return: true - 脱离  false - 未脱离
    static bool getDetachHMIStatus();

    /// Function: servoIsOpOrNot
    /// mater_index 0或1  opFinish作为判断是否OP
    /// 仅ethercat伺服下调用该函数
    /// return 函数调用成功或者失败
    static bool servoIsOpOrNot(int master_index, bool &opFinish);

    /// Function: isAlarming
    /// 获取报警信息
    /// return:  true - 报警   false - 无报警
    /// alarmInfo 报警时返回报警信息
    static bool isAlarming(std::vector<std::string> &alarmInfo);

    /// Function: clearAlarm
    /// 清除报警
    static bool clearAlarm();

    /// Function: robotEnableOrNot
    /// 上下使能
    /// enable： true - 使能   false - 未使能
    static bool robotEnableOrNot(bool enable);

    /// Function: getRobotEnable
    /// 获取使能状态
    /// return:  true - 使能   false - 未使能
    static bool getRobotEnableStatus();

    /// Function: getRobotSoftEmergencyStopStatus
    /// 获取软急停状态
    /// return: true - 正常   false - 急停
    static bool getRobotSoftEmergencyStopStatus(bool &outStatus);

    /// Function: robotSoftEmergencyStop
    /// 软急停
    /// stop： true - 正常   false - 急停
    static bool robotSoftEmergencyStop(bool notStop);

    /// Function: setSingleRobotEnable
    /// 单个机器人上下使能
    /// robotId - 机器人ID  enable： true - 使能   false - 未使能
    /// 注意：使用该功能请不要切换示教器的三挡开关
    static bool setSingleRobotEnable(int robotId, bool enable);

    /// Function: getSingleRobotEnableStatus
    /// 获取单个机器人使能状态
    /// robotId-机器人ID  return: true - 使能   false - 未使能
    static bool getSingleRobotEnableStatus(int robotId);

    /// Function: setGlobalSpeed
    /// 设置全局速度
    /// speed 0-100
    static bool setGlobalSpeed(double speed);

    /// Function: getGlobalSpeed
    /// 获取全局速度
    /// return 当前全局速度
    static double getGlobalSpeed();

    /// Function: setRobotAlarmNotServoOff
    /// 报警时伺服是否下使能
    /// needNotOff ： true - 不下使能   false - 下使能
    static bool setRobotAlarmNotServoOff(bool needNotOff);

    /// Function: alarmingServoOffOrNot
    /// 获取报警时伺服是否下使能
    /// off ： true - 不下使能   false - 下使能
    static bool getRobotAlarmNotServoOff();

    /// Function: setRobotProtectStatus
    /// 设置机器人保护状态
    /// protect: true - 保护状态   false - 非保护状态   robotID传入-1操作所有机器人
    /// 注意：非保护状态下，配合PluseToServo函数使用，完全交给用户控制机器人不做任何保护
    /// 注意：切换保护状态需要等3s以上再发指令
    static bool setRobotProtectStatus(int robotId, bool protect);

    /// Function: getRobotProtectStatus
    /// 获取机器人保护状态
    /// 返回值: true - 保护状态   false - 非保护状态
    static bool getRobotProtectStatus(int robotId);

    /// Function: setAlarmNotPrompt
    /// 设置告警不提示
    /// flag: bit0 正负极限不报警  bit1 路径规划失败不报警
    static bool setAlarmNotPrompt(uint32_t flag);

    /// Function: getAlarmNotPrompt
    /// 获取告警不提示
    /// 返回值: bit0 正负极限不报警  bit1 路径规划失败不报警
    static uint32_t getAlarmNotPrompt();

    struct RobotToolInfo {
        uint16_t id;      // 工具ID toolId
        std::string name; // 工具名称
        int method;       // 2-两点法  3-三点法 6-六点法 23-二十三点法
    };

    struct RobotToolCoordParam {
        std::string name;                        // 工具坐标系名称
        int method = 2;                          // 仅支持 2(两点法) 或 6(六点法)
        std::vector<std::vector<double>> points; // 标定点位，每个点为6维 XYZUVW
    };

    /// Function: setRobotCurrentTool
    /// 设置当前使用的工具
    /// robotId - 机器人ID  toolId - 工具ID
    static bool setRobotCurrentTool(int robotId, uint16_t toolId);

    /// Function: switchRobotToolByName
    /// 按名称切换当前工具坐标系
    /// robotId - 机器人ID  toolName - 工具名称（传空字符串表示取消选择工具）
    static bool switchRobotToolByName(int robotId, const std::string &toolName);
    /// Function: getRobotCurrentTool
    /// 获取当前使用的工具
    /// robotId - 机器人ID  tools - 工具信息
    static bool getRobotToolInfoList(int robotId, std::vector<RobotToolInfo> &tools);

    /// Function: getRobotCurrentToolCoord
    /// 获取当前机器人正在使用的工具坐标系
    /// robotId - 机器人ID
    /// outToolId - 当前工具ID（0表示未选择工具）
    /// outParam - 当前工具坐标系参数（名称/标定法/点位）
    /// 返回值: true - 获取成功（包含未选择工具） false - 获取失败
    static bool getRobotCurrentToolCoord(int robotId, uint16_t &outToolId, RobotToolCoordParam &outParam);

    /// Function: createRobotTool
    /// 创建工具坐标系并提交校验
    /// robotId - 机器人ID
    /// param - 工具参数（仅支持两点法/六点法）
    /// outToolId - 返回新建工具ID
    /// 返回值: true - 成功（校验通过） false - 失败（失败时自动删除该工具）
    static bool createRobotTool(int robotId, const RobotToolCoordParam &param, uint16_t &outToolId);

    /// Function: clearRobotCurrentTool
    /// 取消当前工具选择（回到未选择工具状态）
    /// robotId - 机器人ID
    static bool clearRobotCurrentTool(int robotId);

    /// Function: removeRobotToolByName
    /// 按名称删除指定工具坐标系
    /// robotId - 机器人ID  toolName - 工具名称
    static bool removeRobotToolByName(int robotId, const std::string &toolName);

    /// Function: updateRobotToolByName
    /// 按名称修改指定工具坐标系（可修改名称/标定法/点位）
    /// robotId - 机器人ID
    /// oldToolName - 原工具名称
    /// param - 新工具参数（仅支持两点法/六点法）
    /// 返回值: true - 成功 false - 失败（失败时自动回滚）
    static bool updateRobotToolByName(int robotId, const std::string &oldToolName, const RobotToolCoordParam &param);

    /// Function: getRobotToolNameList
    /// 获取机器人全部工具坐标系名称
    static bool getRobotToolNameList(int robotId, std::vector<std::string> &toolNames);

    /// Function: getRobotToolByName
    /// 按名称获取指定工具坐标系
    static bool getRobotToolByName(int robotId, const std::string &toolName, uint16_t &outToolId,
                                   RobotToolCoordParam &outParam);

    /// Function: removeRobotAllTools
    /// 删除机器人绑定的所有工具坐标系（用于删除机器人时清理工具）
    static bool removeRobotAllTools(int robotId);

    struct RobotWorkSpaceInfo {
        uint16_t id;      // 工作坐标系ID
        std::string name; // 工作坐标系名称
    };

    struct RobotWorkSpaceParam {
        std::string name;                        // 工作坐标系名称
        std::vector<std::vector<double>> points; // 三点法点位，固定3个点，每个点XYZ三维
    };

    /// Function: setRobotCurrentWorkSpace
    /// 设置当前工作坐标系
    /// robotId - 机器人ID  workSpaceId - 工作坐标系ID, 0表示无工作坐标系
    static bool setRobotCurrentWorkSpace(int robotId, uint16_t workSpaceId);

    /// Function: switchRobotWorkSpaceByName
    /// 按名称切换当前工作坐标系
    /// robotId - 机器人ID  workSpaceName - 工作坐标系名称（传空字符串表示无工作坐标系）
    static bool switchRobotWorkSpaceByName(int robotId, const std::string &workSpaceName);

    /// Function: createRobotWorkSpace
    /// 三点法创建工作坐标系并提交校验
    /// robotId - 机器人ID
    /// param - 工作坐标系参数（3个XYZ点）
    /// outWorkSpaceId - 返回新建工作坐标系ID
    /// 返回值: true - 校验成功并保存 false - 校验失败并删除
    static bool createRobotWorkSpace(int robotId, const RobotWorkSpaceParam &param, uint16_t &outWorkSpaceId);

    /// Function: clearRobotCurrentWorkSpace
    /// 取消当前工作坐标系（回到无工作坐标系）
    static bool clearRobotCurrentWorkSpace(int robotId);

    /// Function: removeRobotWorkSpaceByName
    /// 按名称删除指定工作坐标系
    static bool removeRobotWorkSpaceByName(int robotId, const std::string &workSpaceName);

    /// Function: updateRobotWorkSpaceByName
    /// 按名称修改指定工作坐标系（名称/点位）
    /// 返回值: true - 成功 false - 失败（失败自动回滚）
    static bool updateRobotWorkSpaceByName(int robotId, const std::string &oldWorkSpaceName,
                                           const RobotWorkSpaceParam &param);

    /// Function: getRobotWorkSpaceNameList
    /// 获取机器人全部工作坐标系名称
    static bool getRobotWorkSpaceNameList(int robotId, std::vector<std::string> &workSpaceNames);

    /// Function: getRobotCurrentWorkSpace
    /// 获取当前机器人正在使用的工作坐标系
    /// outWorkSpaceId - 当前工作坐标系ID（0表示无工作坐标系）
    /// 返回值: true - 获取成功（包含无工作坐标系） false - 获取失败
    static bool getRobotCurrentWorkSpace(int robotId, uint16_t &outWorkSpaceId, RobotWorkSpaceParam &outParam);

    /// Function: getRobotWorkSpaceByName
    /// 按名称获取指定工作坐标系
    static bool getRobotWorkSpaceByName(int robotId, const std::string &workSpaceName, uint16_t &outWorkSpaceId,
                                        RobotWorkSpaceParam &outParam);

    /// Function: removeRobotAllWorkSpaces
    /// 删除机器人绑定的所有工作坐标系（用于删除机器人时清理工作坐标系）
    static bool removeRobotAllWorkSpaces(int robotId);

    struct RobotGlobalWaypointParam {
        std::string name;           // 路点名称
        std::vector<double> joints; // 关节坐标(按当前机器人轴数) 必填
        std::vector<double> worlds; // 世界坐标(固定6维XYZUVW)  必填
        std::string toolName;       // 依赖工具名称
        std::string workSpaceName;  // 依赖工作台名称
    };

    /// Function: createRobotGlobalWaypoint
    /// 新增指定机器人的全局路点
    static bool createRobotGlobalWaypoint(int robotId, const RobotGlobalWaypointParam &param);

    /// Function: updateRobotGlobalWaypoint
    /// 按路点名称修改指定机器人的全局路点
    static bool updateRobotGlobalWaypoint(int robotId, const std::string &waypointName,
                                          const RobotGlobalWaypointParam &param);

    /// Function: removeRobotGlobalWaypoint
    /// 按路点名称删除指定机器人的全局路点
    static bool removeRobotGlobalWaypoint(int robotId, const std::string &waypointName);

    /// Function: getRobotGlobalWaypointByName
    /// 按名称获取指定机器人的全局路点
    static bool getRobotGlobalWaypointByName(int robotId, const std::string &waypointName,
                                             RobotGlobalWaypointParam &outParam);

    /// Function: getRobotGlobalWaypointList
    /// 获取指定机器人的全部全局路点
    static bool getRobotGlobalWaypointList(int robotId, std::vector<RobotGlobalWaypointParam> &outList);

    using moveJointsEndCb = std::function<void(int robot_id, bool end)>;
    using moveWorldsEndCb = std::function<void(int robot_id, bool end)>;

    /// Function: registerMoveJointsEndCb
    /// 注册关节运动结束回调
    /// 只能注册一次
    static bool registerMoveJointsEndCb(moveJointsEndCb cb);

    /// Funciotn: registerMoveWorldsEndCb
    /// 注册世界运动结束回调
    /// 只能注册一次
    static bool registerMoveWorldsEndCb(moveWorldsEndCb cb);

    /// Function: moveWorlds
    /// 移动到指定世界坐标位置, 直线运动
    /// Parameters:
    ///     worlds - XYZUVW
    ///     interpolation_en - 系统将打断之前的轨迹运行新轨迹
    ///     needCallback - 为真时系统将调用registerMoveWorldsEndCb指定的回调
    ///     delay_in_ms - 滞后执行，此时latch_en/slide_len将无效
    /// accTime - 临时加速度时间，0.1~1.0，0默认0.5s
    /// decTime - 临时减速度时间，0.1~1.0，0默认0.5s
    /// maxLineSpeed - 临时最大线速度，单位m/s，0默认1.0 (最大不要超过2.0  非常快!!!!)
    /// specifyGlobalSpeed - 指定临时全局速度比值，0默认为当前全局速度  0.1即全局速度为10%
    /// 摇操高频调用时 accTime = 0.3  decTime = 0.3 maxLineSpeed = 1.5 specifyGlobalSpeed = 1

    /// decription:该接口后续逐渐废弃，推荐使用moveWorlds2接口，moveWorlds2接口参数更丰富，功能更强大
    static bool moveWorlds(int robotId, const std::vector<float> &worlds, bool interpolation_en = false,
                           bool needCallback = false, uint32_t delay_in_ms = 0, float accTime = 0, float decTime = 0,
                           float maxLineSpeed = 0, float specifyGlobalSpeed = 0);

    /// Function: moveWorldsCurve
    /// 移动到指定世界坐标位置, 曲线运动
    /// Parameters:
    ///     midWorld - XYZUVW 中间点
    ///     endWorld - XYZUVW 结束点
    static bool moveWorldsCurve(int robotId, const std::vector<float> &midWorld, const std::vector<float> &endWorld,
                                bool needCallback = false, uint32_t delay_in_ms = 0);

    /// Function: moveWorldsRound
    /// 移动到指定世界坐标位置, 整圆运动
    /// Parameters:
    ///     midWorld - XYZUVW 中间点
    ///     endWorld - XYZUVW 结束点
    static bool moveWorldsRound(int robotId, const std::vector<float> &midWorld, const std::vector<float> &endWorld,
                                bool needCallback = false, uint32_t delay_in_ms = 0);

    /// Function: moveJoints
    /// 移动到指定关节坐标位置
    /// joints 单位：度
    ///     needCallback - 为真时系统将调用registerMoveWorldsEndCb指定的回调
    ///     delay_in_ms - 滞后执行，此时latch_en/slide_len将无效
    /// accTime - 临时加速度时间，0.1~1.0，0默认0.5s
    /// decTime - 临时减速度时间，0.1~1.0，0默认0.5s
    /// maxLineSpeed - 临时最大线速度，单位m/s，0默认1.0 (最大不要超过2.0  非常快!!!!)
    /// specifyGlobalSpeed - 指定临时全局速度比值，0默认为当前全局速度  0.1即全局速度为10%
    /// 摇操高频调用时 accTime = 0.2  decTime = 0.2 maxLineSpeed = 2.0 specifyGlobalSpeed = 1

    /// decription:该接口后续逐渐废弃，推荐使用moveJoints2接口，moveJoints2接口参数更丰富，功能更强大
    static bool moveJoints(int robotId, const std::vector<float> &joints, bool needCallback = false,
                           uint32_t delay_in_ms = 0, float accTime = 0, float decTime = 0, float maxLineSpeed = 0,
                           float specifyGlobalSpeed = 0);

    struct RobotActionParam {
        std::vector<float> midWorld;      // moveWorldsCurve2和moveWorldsRound2使用
        std::vector<float> endWorld;      // moveWorlds2使用
        std::vector<float> joints;        // moveJoints2使用
        bool interpolation_en = false;    // 立即打断之前的轨迹执行新目标点
        bool needCallback = false;        // 是否需要回调
        bool needBlock = false;           // 是否需要阻塞
        uint32_t delay_in_ms = 0;         // 延时
        float accTime = 0;                // 加速度时间，0默认0.5s
        float decTime = 0;                // 减速度时间，0默认0.5s
        float maxLineSpeed = 0;           // 最大线速度，单位m/s，0默认1.0 (最大不要超过2.0  非常快!!!!)
        float specifyGlobalSpeed = 0;     // 指定全局速度比值，0默认为当前全局速度  0.1即全局速度为10%
        int collabRobotID = 0;            // 双臂协同机器人ID，0为不需要协同，当需要协同时，指定另一个机器人的ID
        uint32_t collabDirectionMask = 0; // 双臂协同方向掩码，bit0-bit5 对应 XYZUVW，置1 表示该方向反向
        int smooth = 1;                   // 平滑等级，平滑等级1-9，0表示不平滑会打断之前路径
        int endJointIndex = 0;            // 以第几轴作为末端运动(索引从0开始)，0为默认整链末端；
                                          // 指定后该轴之后的关节固定为当前关节角，运动仅解算该轴及之前的关节。
                                          // 例：7轴机器人以第五轴为末端 → endJointIndex=4，轴5、6(索引5、6)固定
        bool directIK =
            false; // 直接逆解：控制器直接调用P2J逆解出关节角后走自由路径（仅moveWorldsMulti2批量世界坐标运动使用）
    };

    struct MultiRobotActionParam {
        int robotId = 0;
        RobotActionParam param;
    };

    using moveJointsEndCb2 = std::function<void(int robot_id, bool end, uint64_t sequence)>;
    using moveWorldsEndCb2 = std::function<void(int robot_id, bool end, uint64_t sequence)>;

    static bool registerMoveJointsEndCb2(moveJointsEndCb2 cb);
    static bool registerMoveWorldsEndCb2(moveWorldsEndCb2 cb);

    /// Function: moveWorlds2
    /// 移动到指定世界坐标位置
    /// Parameters:
    ///     robotId - 机器人ID
    ///     param - 机器人动作参数
    /// See Also: RobotActionParam
    ///     如果需要回调 会返回一个唯一的sequence 通过回调函数通知相应的sequence完成 sequence从10开始递增
    ///     返回值0代表完成 1代表失败
    ///     双臂协同使用条件，两个机器人坐标系方向统一，且调用双臂协同时，两个机器人都处于静止状态，调用过程中不允许向协同臂再发送新的运动指令
    static uint64_t moveWorlds2(int robotId, const RobotActionParam &param);

    /// Function: moveJoints2
    /// 移动到指定关节坐标位置
    /// Parameters:
    ///     robotId - 机器人ID
    ///     param - 机器人动作参数
    /// See Also: RobotActionParam
    ///     如果需要回调 会返回一个唯一的sequence 通过回调函数通知相应的sequence完成
    ///     返回值0代表完成 1代表失败
    static uint64_t moveJoints2(int robotId, const RobotActionParam &param);

    /// Function: moveWorldsMulti2
    /// 同时下发多个机器人的世界坐标运动指令
    /// Parameters:
    ///     params - 多机器人动作参数列表，每项包含 robotId 与 RobotActionParam
    /// See Also: MultiRobotActionParam
    ///     批量接口仅负责同时下发，不支持 needCallback / needBlock / sequence / collabRobotID / collabDirectionMask
    ///     返回值: true - 下发成功 false - 下发失败
    static bool moveWorldsMulti2(const std::vector<MultiRobotActionParam> &params);

    /// Function: moveJointsMulti2
    /// 同时下发多个机器人的关节坐标运动指令
    /// Parameters:
    ///     params - 多机器人动作参数列表，每项包含 robotId 与 RobotActionParam
    /// See Also: MultiRobotActionParam
    ///     批量接口仅负责同时下发，不支持 needCallback / needBlock / sequence / collabRobotID / collabDirectionMask
    ///     返回值: true - 下发成功 false - 下发失败
    static bool moveJointsMulti2(const std::vector<MultiRobotActionParam> &params);

    /// Function: PluseToServo
    /// 角度转脉冲直接送伺服（不带有华成运动控制算法）
    /// 关节摇操透传时使用 配合setRobotProtectStatus函数使用
    /// joints 单位：度
    static bool PluseToServo(int robotId, const std::vector<float> &joints);

    /// Function: WorldsToServo
    /// 末端坐标直接送伺服
    /// 末端摇操透传时使用 配合setRobotProtectStatus函数使用
    /// worlds -XYZUVW
    static bool WorldsToServo(int robotId, const std::vector<float> &worlds);

    /// Function: getJoints
    /// 获取当前关节坐标位置
    static bool getJoints(int robotId, std::vector<float> &joints, bool fb = true);

    /// Function: getWorlds
    /// 获取当前世界坐标位置
    static bool getWorlds(int robotId, std::vector<float> &worlds, bool fb = true);

    /// Function: getJointWorldPose
    /// 获取机器人指定关节的世界坐标(含用户坐标系和浮基变换，不含工具变换、非末端无法加工具，该接口不能高频使用)
    /// Parameters:
    ///     robotId - 机器人ID
    ///     jointIndex - 关节索引(从0开始)，-1或超出范围时默认计算到末端，例如获取第五轴传4
    ///     worldPose - 输出 6DOF 世界坐标
    ///     feedback - true使用反馈关节角，false使用指令关节角
    /// Return: 成功返回true
    static bool getJointWorldPose(int robotId, int jointIndex, std::vector<double> &worldPose, bool feedback = true);

    /// Function: setPosReFreshMS
    /// 设置位置刷新周期
    /// robotId - 机器人ID
    /// jointMs - 关节位置刷新周期，单位毫秒 默认40ms更新一次  worldMs - 世界位置刷新周期，单位毫秒 默认50ms更新一次
    /// 注意：由于示教器是串口通讯 位置刷新周期过短会影响示教器的功能，建议根据实际需要设置合理的刷新周期
    static bool setPosReFreshMS(int robotId, uint16_t jointMs, uint16_t worldMs);

    /// Function: getPosReFreshMS
    /// 获取位置刷新周期
    static bool getPosReFreshMS(int robotId, uint16_t &jointMs, uint16_t &worldMs);

    /// Function: getVelocity
    /// 获取当前规划速度
    static double getVelocity(int robotId);

    /// Function: getAcceleration
    /// 获取当前的加速度信息
    static double getAcceleration(int robotId);

    /// Function: switchRunMode
    /// 切换运行模式
    /// axis 从 0 开始  传入-1为所有轴
    /// mode - 0 位置  1 速度  2 力矩
    static bool switchRunMode(int robotId, int mode, int axis = -1);

    /// Function: getCurrentRunMode
    /// 获取轴运行模式
    /// axis 从 0 开始
    /// mode - 0 位置  1 速度  2 力矩
    static int getCurrentRunMode(int robotId, int axis);

    /// Function: getPulseFeedback
    /// 获取反馈脉冲值
    /// robotId - 机器人ID
    /// axisId - 轴ID
    /// return: 反馈脉冲值
    static int32_t getPulseFeedback(int robotId, int axisId);

    /// Function: getTorqueFeedback
    /// 获取力控扭矩
    /// return : true - 成功  false - 失败
    static bool getTorqueFeedback(int robotId, std::vector<int32_t> &torqueFeedback);

    /// Function: getSpeedFeedback
    /// 获取速度反馈
    /// return : true - 成功  false - 失败
    static bool getSpeedFeedback(int robotId, std::vector<int32_t> &speedFeedback);

    /// Function: torqueToServo
    /// 力矩模式下控制使用
    /// torques 单位根据电机自身定义
    static bool torqueToServo(int robotId, const std::vector<int32_t> &torques);

    /// Function: speedToServo
    /// 速度模式下控制使用
    /// speeds 单位根据电机自身定义
    static bool speedToServo(int robotId, const std::vector<int32_t> &speeds);

    /// Function: pauseRobot
    ///     暂停(减速停)或恢复机器人
    /// Parameters: robotId - 机器人ID , pause - true - 暂停  false - 恢复
    /// See Also: getRobotPaused / getRobotMoveState
    ///     getRobotPaused 返回真，则获取机器人已经停稳，处于暂停状态；返回假，则机器人处于运动状态或正在减速停过程中
    ///     getRobotMoveState 暂停后，机器人运动状态依然为真
    static bool pauseRobot(int robotId, bool pause);

    /// Function: clearRobotRoute
    /// 清除机器人路径
    /// robotId - 机器人ID  -1 - 清除所有机器人路径
    ///     emergency_stop - 默认真 / 立即停
    /// See Also: getRobotMoveState
    ///     getRobotMoveState 用于获取机器人运动状态
    /// 注意：清除路径后，可以polling，等待机器人运动状态变为假
    static bool clearRobotRoute(int robotId, bool emergency_stop = true);

    /// Function: getRobotPaused
    /// 用于判断机器人处于暂停
    ///     pauseRobot(robotid, true)，暂停后用本函数判断机器是否已经停稳
    /// Parameters:
    ///     robotId - 机器人ID
    static bool getRobotPaused(int robotId);

    /// Function: setEnableTolerance
    /// 设置到位容差
    /// robot_id 机器人id , enable true:开启到位容差 false:关闭 , tolerance 到位容差(角度，最小0.1) , tolerance_time
    /// 到位容差时间(秒，最小0.5) return false:设置错误 true:设置成功
    static bool setEnableTolerance(int robot_id, bool enable, double tolerance = 0.1, double tolerance_time = 0.5);

    /// Function: getRobotMoveState
    /// 获取机器人运动状态
    /// robot_id 机器人id
    /// return -1:获取错误 0:停止 1:运动
    static int getRobotMoveState(int robot_id);

    /// Function: getRobotAxisMoveState
    /// 获取轴运动状态
    /// robot_id 机器人id , axis_id 轴id
    /// return -1:获取错误 0:停止 1:运动
    static int getRobotAxisMoveState(int robot_id, int axis_id);

    /// Function: setRobotCurrentPosAsOrigin
    /// 设置指定机器人“以当前位置为原点”（对该机器人所有轴生效）
    static bool setRobotCurrentPosAsOrigin(int robotId);

    /// Function: setRobotAxisCurrentPosAsOrigin
    /// 设置指定机器人指定关节“以当前位置为原点”
    static bool setRobotAxisCurrentPosAsOrigin(int robotId, int axisId);

    /// Function: getTeachCoordinateType
    /// 获取当前示教运动坐标系
    static TeachCoordinateType getTeachCoordinateType();

    /// Function: setTeachCoordinateType
    /// 切换示教运动坐标系
    static bool setTeachCoordinateType(TeachCoordinateType type);

    /// Function: teachJoint
    /// 关节示教（与上位机一致：按下持续点动，调用endTeach停止）
    /// robotId - 机器人ID, axisId - 关节轴(从0开始), direction - 方向, speed - 速度百分比(0~100)
    /// collaRobotId - 双臂协同从臂ID，默认0表示不启用协同； 设置为自身ID会返回false
    /// 调用时会直接设置全局速度（kMotionGlobalSpeed）
    static bool teachJoint(int robotId, int axisId, MoveDirection direction, double speed, int collaRobotId = 0);

    /// Function: teachCartesian
    /// 笛卡尔示教（与上位机一致：按下持续点动，调用endTeach停止）
    /// robotId - 机器人ID, axisId - 轴(0:X 1:Y 2:Z 3:U 4:V 5:W), direction - 方向, speed - 速度百分比(0~100)
    /// collaRobotId - 双臂协同从臂ID，默认0表示不启用协同；
    /// 调用时会直接设置全局速度（kMotionGlobalSpeed）
    /// 坐标系由setTeachCoordinateType决定（世界/工具/工作台）
    static bool teachCartesian(int robotId, int axisId, MoveDirection direction, double speed, int collaRobotId = 0);

    /// Function: endTeach
    /// 结束示教运动
    static bool endTeach(int robotId);

    /// Function: stepJoint
    /// 关节步进（机器人运动中禁止调用）
    /// needBlock - true时阻塞等待运动完成，false时异步返回
    static bool stepJoint(int robotId, int axisId, MoveDirection direction, double angleDeg, double speed = 20.0,
                          bool needBlock = false);

    /// Function: stepPosition
    /// 位置步进：axisId 取值0~2(X/Y/Z)（机器人运动中禁止调用）
    /// needBlock - true时阻塞等待运动完成，false时异步返回
    static bool stepPosition(int robotId, int axisId, MoveDirection direction, double lengthMm, double speed = 20.0,
                             bool needBlock = false);

    /// Function: stepAttitude
    /// 姿态步进：axisId 取值0~2(Rx/Ry/Rz 对应 U/V/W)（机器人运动中禁止调用）
    /// needBlock - true时阻塞等待运动完成，false时异步返回
    static bool stepAttitude(int robotId, int axisId, MoveDirection direction, double angleDeg, double speed = 20.0,
                             bool needBlock = false);

    struct RobotFloatingBaseParam {
        bool enable = false;     // 浮动基座开关
        std::vector<double> pos; // 浮动基座位姿，固定6维：XYZRXRYRZ
    };

    /// Function: getRobotFloatingBase
    /// 获取指定机器人的浮动基座参数（开关+位姿）
    static bool getRobotFloatingBase(int robotId, RobotFloatingBaseParam &param);

    /// Function: setRobotFloatingBase
    /// 设置指定机器人的浮动基座参数（开关+位姿）
    /// 位姿必须为6维：XYZRXRYRZ
    static bool setRobotFloatingBase(int robotId, const RobotFloatingBaseParam &param);

    /// Function: setRobotFloatingBaseEnable
    /// 设置指定机器人的浮动基座开关
    static bool setRobotFloatingBaseEnable(int robotId, bool enable);

    /// Function: getRobotFloatingBaseEnableStatus
    /// 获取指定机器人的浮动基座开关状态
    static bool getRobotFloatingBaseEnableStatus(int robotId, bool &enable);

public:
    enum AxisParameterIndex {
        KPositiveLimitation = 1,
        KNegtiveLimitation,
    };

    struct RobotDHParam {
        double theta = 0.0; // 关节偏置角
        double d = 0.0;     // 连杆偏距
        double a = 0.0;     // 连杆长度
        double alpha = 0.0; // 连杆扭角
    };

    /// Function: getAxisParameter
    /// 获取轴参数
    /// robot_id 机器人id , axis_id 轴id value 轴参数
    /// return : true - 成功  false - 失败
    static bool getAxisParameter(int robot_id, int axis_id, AxisParameterIndex index, double &value);

    /// Function: setAxisParameter
    /// 设置轴参数
    /// robot_id 机器人id , axis_id 轴id
    static bool setAxisParameter(int robot_id, int axis_id, AxisParameterIndex index, double limitation);

    struct RobotAxisMotorParam {
        uint32_t pulsePerRevolution = 0;   // 每转脉冲数(kAxisDataPulse)
        double reductionRatio = 0.0;       // 减速比(kAxisDataRatio)
        uint32_t maxRpm = 0;               // 最大转速(kAxisDataRpm)
        double ratedTorque = 0.0;          // 额定扭矩(kAxisDataNormalTorque)
        double torqueConversionFactor = 0; // 力矩转换系数(kAxisDataTorqueConversionFactor)
        double accTime = 0.0;              // 加速时间(kAxisDataAcc)
        double decTime = 0.0;              // 减速时间(kAxisDataDec)
        double positiveLimit = 0.0;        // 正极限(kAxisDataLPPos)
        double negativeLimit = 0.0;        // 负极限(kAxisDataLNPos)
    };

    /// Function: getRobotAxisMotorParam
    /// 获取指定机器人指定轴电机参数
    static bool getRobotAxisMotorParam(int robotId, int axisId, RobotAxisMotorParam &param);

    /// Function: setRobotAxisMotorParam
    /// 设置指定机器人指定轴电机参数
    static bool setRobotAxisMotorParam(int robotId, int axisId, const RobotAxisMotorParam &param);

    /// Function: getRobotDHParams
    /// 获取机器人DH参数（每轴theta/d/a/alpha）
    static bool getRobotDHParams(int robotId, std::vector<RobotDHParam> &dhParams);

    /// Function: setRobotDHParams
    /// 设置并保存机器人DH参数（每轴theta/d/a/alpha）
    static bool setRobotDHParams(int robotId, const std::vector<RobotDHParam> &dhParams);

    /// Function: quaternionToEuler
    /// 四元数转换为欧拉角，采用 ZYX（roll-pitch-yaw）旋转约定
    /// quaternion - 四元数，固定4维，元素顺序为 [w, x, y, z]，函数内部会归一化
    /// rpy - 输出欧拉角，固定3维，元素顺序为 [roll, pitch, yaw]，单位为弧度
    /// return false - 输入无效  true - 转换成功
    static bool quaternionToEuler(const std::vector<double> &quaternion, std::vector<double> &rpy);

    /// Function: eulerToQuaternion
    /// 欧拉角转换为四元数，采用 ZYX（roll-pitch-yaw）旋转约定
    /// rpy - 欧拉角，固定3维，元素顺序为 [roll, pitch, yaw]，单位为弧度
    /// quaternion - 输出单位四元数，固定4维，元素顺序为 [w, x, y, z]
    /// return false - 输入无效  true - 转换成功
    static bool eulerToQuaternion(const std::vector<double> &rpy, std::vector<double> &quaternion);

    /// Function: tryWorlds
    /// 将末端坐标转为关节坐标，并返回是否超过极限
    /// fixedJointIndices 固定关节索引数组(从0开始)；fixedJointValues 固定关节角度值数组[°]（两者长度一致）
    /// return : true - 成功  false - 失败
    /// 该函数即将废弃 明确语义逆解函数 inverseKinematics
    static bool tryWorlds(int robotId, const std::vector<double> &worlds, std::vector<double> &joint,
                          const std::vector<int> &fixedJointIndices = {},
                          const std::vector<double> &fixedJointValues = {}, int *pLimitMask = nullptr,
                          int *nLimitMask = nullptr, double eps = 1e-2);

    /// Function: inverseKinematics
    /// 普通逆运动学：将指定工作/用户坐标系下的TCP笛卡尔位姿转换为关节角
    /// toolId、workSpaceId 为0时分别表示不使用工具、使用机器人基坐标系
    /// return : true - 成功  false - 失败
    static bool inverseKinematics(int robotId, const std::vector<double> &cartesianPose,
                                  std::vector<double> &joints, int toolId = 0, int workSpaceId = 0);

    /// Function: inverseKinematicsWithFixedJoints
    /// 固定轴逆运动学；fixedJointIndices与fixedJointValues必须非空、等长，索引从0开始，角度单位为度
    static bool inverseKinematicsWithFixedJoints(int robotId, const std::vector<double> &cartesianPose,
                                                 const std::vector<int> &fixedJointIndices,
                                                 const std::vector<double> &fixedJointValues,
                                                 std::vector<double> &joints, int toolId = 0,
                                                 int workSpaceId = 0);

    /// Function: forwardKinematics
    /// 正运动学：将关节角转换为指定工作/用户坐标系下的TCP笛卡尔位姿
    /// toolId、workSpaceId 为0时输出基坐标系下的法兰位姿
    static bool forwardKinematics(int robotId, const std::vector<double> &joints,
                                  std::vector<double> &cartesianPose, int toolId = 0, int workSpaceId = 0);

    /// Function: convertBasePoseToUserToolPose
    /// 将基坐标系下的法兰位姿转换为指定工作/用户坐标系下的TCP位姿
    /// toolId、workSpaceId 必须大于等于0 否则返回失败
    static bool convertBasePoseToUserToolPose(int robotId, const std::vector<double> &basePose, int toolId,
                                              int workSpaceId, std::vector<double> &userToolPose);

    /// Function: convertUserToolPoseToBasePose
    /// 将指定工作/用户坐标系下的TCP位姿转换为基坐标系下的法兰位姿
    /// toolId、workSpaceId 必须大于等于0 否则返回失败
    static bool convertUserToolPoseToBasePose(int robotId, const std::vector<double> &userToolPose, int toolId,
                                              int workSpaceId, std::vector<double> &basePose);

    /// Function: jointOutLimit
    /// 设置轴参数
    /// robot_id 机器人id , axis_id 轴id
    /// overPositive - 超过正极限
    /// overNegtive - 超过负极限
    static bool jointOutLimit(int robotId, int axis_id, double joint, bool &overPositive, bool &overNegtive,
                              double eps = 1e-2);

    enum SdoDataType { SDO_UINT8 = 0, SDO_UINT16, SDO_UINT32, SDO_INT8, SDO_INT16, SDO_INT32 };

    /// Function: SdoRead
    /// SDO读取回调函数类型定义
    using SdoRead = std::function<void(int robotId, int axisId, int index, int subindex, int value)>;

    /// Function: registerSdoReadCb
    /// 注册SDO读取回调
    /// 注意: 使用readSdo前先注册
    static bool registerSdoReadCb(SdoRead cb);

    /// Function: readSdo
    /// 读取SDO
    /// robotId - 机器人ID, axisId - 轴ID, index - SDO索引, subindex - SDO子索引, dataType - 数据类型
    static bool readSdo(int robotId, int axisId, int index, int subindex, int dataType = SDO_UINT32);

    /// Function: writeSdo
    /// 写入SDO
    /// robotId - 机器人ID, axisId - 轴ID, index - SDO索引, subindex - SDO子索引, value - 写入值, dataType - 数据类型
    static bool writeSdo(int robotId, int axisId, int index, int subindex, int value, int dataType = SDO_UINT32);

    /// Function: runSubProgram
    /// 运行指定模号下的子程序
    /// moldName - 模号名         subProgramName - 子程序名
    /// startLine - startMode 为 2 时生效的行号
    /// startMode - 启动行策略：1-首行  2-指定行  4-继续运行(默认)
    /// speed - 全局速度 0~100，<0 表示不修改
    /// firstMoldSpeed - 首模速度 0~100，<0 表示不修改
    /// firstMoldSpeedEnable - 首模速度使能：-1 不修改  0 关闭  1 开启
    /// recycle - 子程序是否循环运行：true 循环  false 单次
    static bool runSubProgram(const std::string &moldName, const std::string &subProgramName, int startLine = 0,
                              int startMode = 4, double speed = -1, double firstMoldSpeed = -1,
                              int firstMoldSpeedEnable = -1, bool recycle = true);

    /// Function: stopSubProgram
    /// 停止子程序运行
    static bool stopSubProgram();

    struct CoordinatedMotionConfig {
        int waist_robot_id = -1;
        std::vector<int> arm_robot_ids;
        std::vector<std::vector<double>> mount_transforms; // 手臂浮动基座(腰末端→臂i基座) [x,y,z,rx,ry,rz]
        std::vector<double> waist_base_transform;          // 腰浮动基座
    };

    struct CoordinatedMotionTarget {
        std::vector<std::vector<double>> object_waypoints; // 物体world系轨迹 N×6DOF
        std::vector<std::vector<double>> grasp_offsets;    // 物体抓取偏置(物体→臂i抓取点) [x,y,z,rx,ry,rz]
        std::vector<int> arm_indices;                      // 参与搬运的臂索引，空则默认全部
        float speed = 100;
    };

    /// Function: setCoordinatedMotionConfig
    /// 设置腰手协同配置
    /// return true成功 false失败
    static bool setCoordinatedMotionConfig(const CoordinatedMotionConfig &config);

    /// Function: coordinatedMoveTo
    /// 启动腰手协同运动
    /// target - 物体世界系轨迹 + 抓取偏置 + 参与臂索引 + 速度
    /// return true成功 false失败
    static bool coordinatedMoveTo(const CoordinatedMotionTarget &target);

    /// Function: coordinatedMoveStop
    /// 停止腰手协同运动
    /// return true成功 false失败
    static bool coordinatedMoveStop();

    /// Function: coordinatedMoveIsEnd
    /// 查询协同运动是否结束
    /// return true  - 已结束（未在协同中，或参与协同的机器人均已到位）
    /// return false - 未结束（仍有机器人在运动）；查询失败时也返回 false
    static bool coordinatedMoveIsEnd();

    /// *************************************************基础函数***************************************************

    /// *************************************************力控函数***************************************************

public:
    /// @brief 机器人（负载）辨识
    /// @param robot_id 当前机器人id
    /// @param mode 0-本体辨识  1-负载辨识
    /// @param trial 低速试行
    /// @return
    static bool startRobotIdentify(int robot_id, int mode, bool trial = true);

    /// @brief 获取机器人辨识状态
    /// @param robot_id 当前机器人id
    /// @return 1-辨识完成
    static int getRobotIdentifyStatus(int robot_id);

    /// @brief 停止辨识
    /// @param robot_id 当前机器人id
    /// @return
    static bool stopRobotIdentify(int robot_id);

    /// @brief 获取本体辨识结果
    /// @param robot_id 机器人id
    /// @return
    static std::vector<double> getRobotIdentifyResult(int robot_id);

    /// @brief 获取负载列表
    /// @param robot_id 机器人id
    /// @return 负载id列表
    static std::vector<int> getRobotLoadList(int robot_id);

    /// @brief 获取当前负载id
    /// @param robot_id 机器人id
    /// @return 负载id
    static int getRobotCurrentLoad(int robot_id);

    /// @brief 获取指定负载参数
    /// @param load_id 负载id
    /// @return 负载参数
    static std::vector<double> getRobotLoadInfo(int load_id);

public:
    /// Function: activateDragTeach
    /// 开启或关闭拖动示教
    /// robot_id 机器人id , sw true:开启拖动示教 false:关闭 , friction 每个轴的摩擦比例(可以使用默认值) , compensate
    /// 每个轴的补偿系数(可以使用默认值) return: 成功或失败
    static bool activateDragTeach(int robot_id, bool sw, const std::vector<double> &friction = {},
                                  const std::vector<double> &compensate = {});

    /// Function: activateCollision
    /// 开启或关闭碰撞检测
    /// robot_id 机器人id , sw true:开启碰撞检测 false:关闭 , level 碰撞等级 1-10(越小越灵敏)
    /// return: 成功或失败
    static bool activateCollision(int robot_id, bool sw, int level);

    /// @brief 获取拖动示教状态
    /// @param robot_id
    /// @return 开启或关闭
    static bool getDragStatus(int robot_id);

    /// @brief 获取碰撞检测状态
    /// @param robot_id
    /// @return 开启或关闭
    static bool getCollisionStatus(int robot_id);

    /// @brief 力位混合控制
    /// @param robot_id
    /// @param sw 开关
    /// @param mode 模式：0-定点 1-运动
    /// @return 成功或失败
    static bool activateHybridFPControl(int robot_id, bool sw, int mode);

    /// @brief 获取力位混合状态
    /// @param robot_id
    /// @return 开启或关闭
    static bool getHybridFPControlStatus(int robot_id);

public:
    struct LoadParameter {
        double quality;
        double center_mass_x;
        double center_mass_y;
        double center_mass_z;
    };

    /// @brief setLoad 设置当前负载
    /// @param robot_id 机器人id
    /// @param param 负载参数
    /// @return 1-成功
    static int setLoad(const int robot_id, const LoadParameter &param);

    /// @brief unsetLoad 卸载负载
    /// @param robot_id 机器人id
    /// @return 1-成功
    static int unsetLoad(const int robot_id);

    /// Function: forceKeepSwitch
    /// 恒力模式切换
    /// robotId - 机器人ID,
    /// type  0 - 恒力跟踪  1 - 装配
    /// state 0 - 结束     1 - 开始
    /// fm  合力
    /// startPos - 起始末端位姿, adm_param - 力控参数 M B K P I D
    /// 注意: state为1时, type fm startPos adm_param不能为空,state为0时,其他参数可为空
    static void forceKeepSwitch(int robotId, int state, int type = -1, double fm = -1,
                                std::vector<double> startPos = std::vector<double>(),
                                std::vector<double> adm_param = std::vector<double>());

    /// Function: getForceKeepState
    /// 获取力控状态
    /// robotId - 机器人ID
    /// return: 1 - 寻面中 , 2 - 寻面轨迹运行完成，寻面动作未完成 , 3 - 寻面结束开始恒力跟踪 , 4 - 暂停 , 5 - 停止
    static int getForceKeepState(int robotId);

    /// Function: getTorqueFeedback
    /// 获取力控扭矩
    /// robotId - 机器人ID
    /// axisId - 轴ID
    /// return: 轴扭矩
    static int32_t getTorqueFeedback(int robotId, int axisId);

    /// Function: AdmittanceControl
    /// 导纳控制初始化
    /// robotId - 机器人ID
    /// adm_param_6dof - 六维 M B K P I D
    static void AdmittanceControlInit(int robotId, std::map<int, std::vector<double>> adm_param_6dof);

    /// Function: AdmittanceControl
    /// 导纳控制
    /// robotId - 机器人ID
    /// forces - 目标六维力
    static void AdmittanceControl(int robotId, std::vector<double> forces);

    /// @brief 获取传感器数据
    /// @param robot_id 机器人id
    /// @param baseCoord 基准 0-无 1-世界 2-传感器
    /// @return 传感器数据
    static std::vector<double> getSensorData(const int robot_id, const int baseCoord = 0);

    /// @brief sensorCalibration 传感器标定（当前机器人）
    /// @param robot_id 当前机器人id
    /// @param install_angle 安装角度
    /// @return 1-成功
    static int sensorCalibration(const int robot_id, const double install_angle = 0);

    /// @brief stopCalibration 停止标定（当前机器人）
    /// @return 1-成功
    static int stopCalibration();

    /// @brief getCalibrationState 获取标定状态（当前机器人）
    /// @return 1-标定结束
    static int getCalibrationState();

    /// @brief setSensorZerodrift 设置传感器零漂值
    /// @param robot_id 机器人id
    /// @param drift 零漂值
    /// @return 1-成功
    static int setSensorZerodrift(int robot_id, std::vector<double> drift);

    /// @brief activateSensorDrag 六维力传感器拖动
    /// @param robot_id 机器人id
    /// @param sw 开关
    /// @param position_sensitivity 位置灵敏度 0-100
    /// @param posture_sensitivity 姿态灵敏度 0-100
    /// @param enable xyzuvw是否可拖动 0-否 1-是
    /// @return 1-成功
    static int activateSensorDrag(int robot_id, bool sw, int position_sensitivity = 20, int posture_sensitivity = 20,
                                  std::vector<int> enable = {1, 1, 1, 1, 1, 1});

    /// @brief dragTrajRecord 拖动轨迹记录
    /// @param robot_id 机器人id
    /// @param action_type 0-关节 1-笛卡尔
    /// @param record_interval 记录间隔 action_type=0时单位是°，=1时单位是mm
    /// @return 1-成功
    static int dragTrajRecord(int robot_id, int action_type = 0, double record_interval = 5);

    /// @brief stopAndSaveRecordTraj 停止记录并选择是否保存轨迹
    /// @param robot_id 机器人id
    /// @param name 轨迹名
    /// @param isSave 是否保存
    /// @return 1-成功
    static int stopAndSaveRecordTraj(int robot_id, std::string name, bool isSave = true);

    /// @brief 获取机器人轨迹名列表
    /// @param robot_id
    /// @return 轨迹名列表
    static std::vector<std::string> getTrajList(int robot_id);

    /// @brief runRecordTraj 运行指定轨迹
    /// @param robot_id 机器人id
    /// @param name 轨迹名
    /// @param smooth_level 平滑等级 1-9
    /// @param speed 全局速度百分比 0-100
    /// @return 1-成功
    static int runRecordTraj(int robot_id, std::string name, int smooth_level = 1, int speed = 100);

    /// @brief 删除指定轨迹
    /// @param robot_id
    /// @param name 轨迹名
    /// @return 成功或失败
    static bool delRecordTraj(int robot_id, std::string name);

    /// *************************************************力控函数***************************************************

    /// *************************************************自定义函数***************************************************

    /// Function: setWaistSafePose
    /// 设置机器人腰部安全姿态  Fabs(-J1 + J2 +J3) < angle
    /// Parameters: angle - 安全角度
    static void setWaistSafePose(double angle);

    /// Function: getWaistSafePose
    /// 获取机器人腰部安全姿态
    /// return: 安全角度
    static double getWaistSafePose();

    enum ArmBindMode {
        ArmBindWithNull,
        LeftArmBindWithWaist,
        RightArmBindWithWaist,
        DoubleArmBindWithWaist,
    };

    struct RobotWorldsData {
        int robotId;
        std::vector<float> worldPos;
        bool needCallback = false;
        float accTime = 0;            // 临时-指定空间加速时间(秒)
        float decTime = 0;            // 临时-指定空间减速时间(秒)
        float maxLineSpeed = 0;       // 临时-最大线速度(m/s)
        float specifyGlobalSpeed = 0; // 临时-全局速度指定值，最小值0 最大值1 (比值)
    };

    /// Function: getArmBindMode
    /// 获取当前绑定模式
    /// mode - 绑定模式
    /// return false - 失败  true - 成功
    static bool getArmBindMode(ArmBindMode &mode);

    /// Function: bindModeWorldMove
    /// 绑定模式下末端坐标运动
    /// mode - 绑定模式
    /// robotWorlds  - 机器人末端坐标  key: 机器人ID  value: XYZUVW坐标
    /// return false - 失败  true - 成功
    static bool bindModeWorldMove(ArmBindMode mode, const std::vector<RobotWorldsData> &robotWorlds);

    /// 单绑模式下，此时单个手臂与腰部进行绑定，调用函数给出单个手臂坐标。提前解算是否能到位，手臂到不了，手腰同时运动（同时开始但不是同时结束）。
    /// 双绑模式下，此时双手臂与腰部进行绑定，调用函数给出两个手臂坐标。提前解算是否能到位，手臂到不了，手腰同时运动（同时开始但不是同时结束）。
    /// 无绑模式下, 调用函数传入三个坐标各自运动。
    /// 注意: 切换绑定模式需要上个模式运动结束后再切换，否则返回失败。同种模式下允许连续调用运动函数

    /// Function: setCurrentProtectThreshold
    /// 设置机器人电流保护阈值 threshold不为0 则启用
    /// axis从0开始 传入-1为所有轴
    /// return false - 失败  true - 成功
    static bool setCurrentProtectThreshold(int robotID, int axis, int16_t threshold);

    /// Function: getCurrentProtectThreshold
    /// 获取机器人电流保护阈值
    /// axis从0开始
    /// return false - 失败  true - 成功
    static bool getCurrentProtectThreshold(int robotID, int axis, int16_t &threshold);

    /// *************************************************自定义函数***************************************************
};
} // namespace HCXSDKLib

#endif
