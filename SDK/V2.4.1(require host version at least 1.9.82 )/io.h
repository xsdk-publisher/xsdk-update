#ifndef HC_IO_H
#define HC_IO_H

#include "export.h"
#include <functional>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

namespace HCXSDKLib {

enum ModbusCommandType {
    CMD_READ_COILS = 0x01,
    CMD_READ_DISCRETE_INPUTS = 0x02,
    CMD_READ_HOLD_REGISTER = 0x03,
    CMD_READ_INPUT_REGISTER = 0x04,
    CMD_WRITE_SINGLE_COIL = 0x05,
    CMD_WRITE_MULTIPLE_COILS = 0x0F,
    CMD_WRITE_MULTIPLE_REGISTERS = 0x10,
};

// 作为主站，read/write从站的命令
struct ModbusRtuData {

    ModbusRtuData() : name(""), slaveid(0), addr(0), amount(0), write_value(0) {}

    // 数据通道名："COM1"/"COM2"（RTU）或 "192.168.1.10"（TCP）
    std::string name;

    /* 从站ID */
    int slaveid;

    /* 首个地址（寄存器地址或线圈地址） */
    int addr;

    /* 数据长度（寄存器个数/线圈个数，不是字节数） */
    int amount;

    /* 写单个线圈(0x05) 或写单个寄存器(0x06) 使用 */
    uint16_t write_value;

    // 写多个寄存器使用，或写多个线圈时传0/1序列
    std::vector<uint16_t> writeValues;
};

class DLL_EXPORT IOManager {
public:
    using ModbusUpdate = std::function<void(bool success, const ModbusRtuData &data, std::vector<uint16_t> &values)>;
    using ModbusWrite = std::function<void(bool success, const ModbusRtuData &data)>;

    // 注册modbus更新值回调,使用modbusControl前先注册,只需要注册一次
    static bool registerModbusReadCb(ModbusUpdate cb);
    static bool registerModbusWriteCb(ModbusWrite cb);

    struct ModbusOptParam {
        bool needCallback;                 /// needCallback - 是否触发注册的回调
        bool needBlock;                    /// needBlock - 是否阻塞直到本次操作完成
        uint32_t timeout_ms;               /// timeout_ms - 阻塞超时时间(毫秒)，0表示不超时
        std::vector<uint16_t> *readValues; /// readValues - 仅在读功能码且阻塞模式下使用, 返回读取到的值

        ModbusOptParam() : needCallback(true), needBlock(false), timeout_ms(0), readValues(nullptr) {}
    };

    /// Function: modbusControl
    /// modbus控制函数
    ////功能码:0x05写单个线圈 cmd传入CMD_WRITE_SINGLE_COIL
    ////modbusData name "COM1"、slaveid 1、 addr 0、 write_value 0/1(内部转0x0000/0xFF00)
    ////功能码:0x0F写多个线圈 cmd传入CMD_WRITE_MULTIPLE_COILS
    ////modbusData name "COM1"、slaveid 1、 addr 0、 amount 16、writeValues推入0/1值
    ////功能码:0x10写多个寄存器 cmd传入CMD_WRITE_MULTIPLE_REGISTERS
    ////modbusData name "COM1"、slaveid 1、 addr 500、 amount 寄存器数量、writeValues推入相应数值
    ////功能码:0x01/0x02/0x03/0x04 读线圈/离散输入/保持寄存器/输入寄存器
    ////modbusData name "COM1"、slaveid 1、 addr 500、 amount 1
    static bool modbusControl(ModbusCommandType cmd, const ModbusRtuData &modbusData, const ModbusOptParam &opt = ModbusOptParam());

    struct ModbusRtuMasterConfig {
        std::string serialport = "COM1"; // 串口名，例如 "COM1" / "COM2"
        int baud = 115200;               // 波特率，例如 9600/19200/115200
        int parity = 0;                  // 奇偶校验：0-无 1-奇 2-偶
        int stopbit = 1;                 // 停止位：1 或 2
        int overtime = 1;                // 超时时间，单位秒
        int datamode = 0;                // 数据格式：0-大端 1-小端
    };

    struct ModbusTcpMasterConfig {
        std::string tcptype = "Client"; // TCP类型："Client" 或 "Server"
        std::string ip = "0.0.0.0";     // 远端IP（Client模式有效，Server模式固定为"0.0.0.0"）
        int port = 502;                 // TCP端口，例如 502
        int overtime = 1;               // 超时时间，单位秒
        int datamode = 0;               // 数据格式：0-大端 1-小端
    };

    /// Function: addModbusRtuMaster
    /// 新增modbus RTU主站
    static bool addModbusRtuMaster(const ModbusRtuMasterConfig &config);

    /// Function: deleteModbusRtuMaster
    /// 删除指定modbus RTU主站(按串口)
    static bool deleteModbusRtuMaster(const std::string &serialport);

    /// Function: getModbusRtuMaster
    /// 查询指定modbus RTU主站(按串口)
    static bool getModbusRtuMaster(const std::string &serialport, ModbusRtuMasterConfig &config);

    /// Function: getModbusRtuMasterList
    /// 查询modbus RTU主站列表
    static bool getModbusRtuMasterList(std::vector<ModbusRtuMasterConfig> &configs);

    /// Function: addModbusTcpMaster
    /// 新增modbus TCP主站
    static bool addModbusTcpMaster(const ModbusTcpMasterConfig &config);

    /// Function: deleteModbusTcpMaster
    /// 删除指定modbus TCP主站(按类型+IP+端口)
    static bool deleteModbusTcpMaster(const ModbusTcpMasterConfig &config);

    /// Function: getModbusTcpMaster
    /// 查询指定modbus TCP主站(按类型+IP+端口)
    static bool getModbusTcpMaster(const ModbusTcpMasterConfig &query, ModbusTcpMasterConfig &config);

    /// Function: getModbusTcpMasterList
    /// 查询modbus TCP主站列表
    static bool getModbusTcpMasterList(std::vector<ModbusTcpMasterConfig> &configs);

    /// Function: setEmergencyStopIO
    /// 设置急停IO
    // inputIO: 10-17、20-27、30-37  根据实际IO选择
    // high: true表示高电平有效，false表示低电平有效
    static bool setEmergencyStopIO(int inputIO, bool high = true);

    /// Function: getInputState
    /// 获取输入IO
    /// input_io inputIO: 10-17、20-27、30-37  根据实际IO选择
    /// return: true表示高电平，false表示低电平
    static bool getInputState(int input_io);

    /// Function: setOutputState
    /// 获取输出IO
    /// output_io outputIO: 10-17、20-27、30-37  根据实际IO选择
    /// return: true表示高电平，false表示低电平
    static bool setOutputState(int output_io, bool value);

public:
    /// Function: getEthercatIOState
    /// 获取EthercatIO
    /// prefix 前缀 "eio_out"(ethercat io output) 或者 "eio_in"(ethercat io input)
    /// master_index 主站序号, 0-3，一般为0
    /// pos 从站
    /// vendor_id 厂商号
    /// index 索引
    /// subindex 子索引
    static int32_t getEthercatIOState(const char *prefix, int master_index, int pos, int vendor_id, int index,
                                      int subindex);

    /// Function: setEthercatIOState
    /// 设置EthercatIO
    /// prefix 前缀 "eio_out"(ethercat io output, TXPDO) 或者 "eio_in"(ethercat io input)
    /// master_index 主站序号, 0-3，一般为0
    /// pos 从站
    /// vendor_id 厂商号
    /// index 索引
    /// subindex 子索引
    /// value 要设置的值
    /// return: true表示成功，false表示失败
    static bool setEthercatIOState(const char *prefix, int master_index, int pos, int vendor_id, int index,
                                   int subindex, int value);

    struct EcatOperation {
        std::string prefix;
        int master_index = 0;
        int pos = 0;
        int vendor_id = 0;
        int index = 0;
        int subindex = 0;
        int value = 0;
        int type = 0;
    };
    /// Function: getEthercatPDO
    /// 获取已经配置的ecat pdo
    ///     同getEthercatIOState
    ///     主要用于非402标准PDO读写，6040等读写稍晚开放
    static int32_t getEthercatPDO(const EcatOperation&);

    /// Function: setEthercatPDO
    /// 设置已经配置的ecat pdo
    ///     同setEthercatIOState
    ///     主要用于非402标准PDO读写，6040等读写稍晚开放
    static bool setEthercatPDO(const EcatOperation&);

    /// Function: getEthercatPDOs
    /// 批量获取已经配置的ecat pdo
    ///     任一条变量不存在则返回 false
    static bool getEthercatPDOs(std::vector<EcatOperation> &ops);

    /// Function: setEthercatPDOs
    /// 批量设置 ecat pdo
    ///     任一条变量不存在或 RPC 失败则返回 false
    static bool setEthercatPDOs(const std::vector<EcatOperation> &ops);

    /// Function: modbusControlOverEcat
    /// 部分设备通过CAN或者485方式连接到ethercat设备
    /// 通过ethercat pdo来发送modbus指令
    /// 默认方式：
    ///     01      03       00 01 00 003 54 0B
    /// subindex subindex+1
    static int32_t modbusControlOverEcat(int funccode, const ModbusRtuData& op, const EcatOperation& ecat_op);

public:
    /// Function: writeCOM
    /// 读串口
    /// com - COM1 or COM2
    /// return: true表示成功，false表示失败
    static bool openCom(const char *com, uint32_t baudrate = 115200, uint32_t databits = 8, int parity = 0,
                        uint32_t stopbits = 1);
    /// Function: writeCOM
    /// 写串口
    /// com - COM1 or COM2
    /// buffer - 写缓冲区
    /// len - 写缓冲区长度
    /// return: true表示成功，false表示失败
    static bool writeCOM(const char *com, const char *buffer, uint32_t len);

    /// Function: readCOM
    /// 读串口
    /// com - COM1 or COM2
    /// buffer - 读缓冲区,请确保缓冲区长度大于256
    /// buffer_len - 缓冲区长度
    /// return: 读到的字符长度
    static uint32_t readCOM(const char *com, char *buffer, uint32_t buffer_len);

public:
    struct CanFrame {
        int port;
        uint32_t id;
        std::vector<uint8_t> data;
    };
    using CanReadCallback = std::function<void(const CanFrame &frame)>;

    /// Function: registerCanReadCb
    /// 注册CAN读回调,当有CAN消息读到时会触发回调,只需要注册一次
    /// cb - 回调函数，参数为读到的CAN消息
    /// return: true表示注册成功，false表示已经注册过了
    static bool registerCanReadCb(CanReadCallback cb);

    /// Function: openCAN
    /// 打开CAN接口
    /// port - CAN端口 can0对应0，can1对应1
    /// bitrate - 波特率，常用的有500000、1000000等
    /// type - CAN类型，0表示经典CAN，1表示CAN FD
    /// return: true表示成功，false表示失败
    static bool openCAN(int port, uint32_t bitrate, int type = 0);

    /// Function: writeCAN
    /// 写CAN消息
    /// port - CAN端口 can0对应0，can1对应1
    /// can_id - CAN ID
    /// buffer - 写缓冲区
    /// len - 写缓冲区长度，经典CAN最大8字节，CAN FD最大64字节
    /// is_extended - 是否扩展帧，经典CAN最大11位ID，扩展帧最大29位ID
    /// is_fd - 是否CAN FD帧，经典CAN最大8字节数据，CAN FD最大64字节数据
    /// return: true表示成功，false表示失败
    static bool writeCAN(int port, uint32_t can_id, const char *buffer, uint32_t len, bool is_extended = false,
                         bool is_fd = false);

    /// Function: readCAN
    /// 读CAN消息    /// port - CAN端口 can0对应0，can1对应1
    /// return: 读到的消息数量，0表示没有读到消息，-1表示读取失败
    /// 从注册的回调函数中获取读到的消息或者&outFrames
    static uint32_t readCAN(int port, std::vector<CanFrame> &outFrames);
};

}; // namespace HCXSDKLib

#endif
