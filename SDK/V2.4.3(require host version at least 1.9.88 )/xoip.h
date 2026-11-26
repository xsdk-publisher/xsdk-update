#ifndef XOIP_H
#define XOIP_H

#include <string>
#include "export.h"

namespace HCXSDKLib {

class DLL_EXPORT VersionManager {
public:
    /// 获取SDK版本
    static std::string getLibVersion();


    /// 获取下位机版本
    static std::string getHostVersion();


    /// 获取驱动版本
    static std::string getDriveVersion();


    /// 更新主机程序
    /// path: 主机程序路径(绝对地址)
    static bool updateHost(const std::string path);


    /// 更新驱动程序
    /// path: 驱动程序路径(绝对地址)
    static bool updateDrive(const std::string path);
};

}; // namespace HCXSDKLib

#endif
