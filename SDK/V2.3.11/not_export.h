#ifndef HC_NOT_EXPORT_H
#define HC_NOT_EXPORT_H

#include "export.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace HCXSDKLib {

class DLL_EXPORT NotExportManager {
public:
    /// Function: registerTopicCallback
    /// 注册topic回调函数
    static void
    registerTopicCallback(const std::string &,
                          std::function<void(const std::string &topic, const std::string &content)> callback);
};

}; // namespace HCXSDKLib

#endif
