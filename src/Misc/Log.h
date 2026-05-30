#pragma once
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#define LOGD SPDLOG_DEBUG
#define LOGI SPDLOG_INFO
#define LOGW SPDLOG_WARN
#define LOGE SPDLOG_ERROR

class Log {
public:
    using level = spdlog::level::level_enum;
    using path  = boost::filesystem::path;

    struct Config {
        level       logLevel      = level::debug;
        bool        enableConsole = true;
        bool        enableFile    = true;
        bool        enableRotate  = false;
        std::size_t maxSize       = 10 * 1024;
        std::size_t maxFiles      = 3;
        path        logPath       = "logs/default.log";
        std::string pattern       = "[%Y-%m-%d %H:%M:%S.%e][%L][%s:%#] %v";
    };

    static void Init(const Config& logConfig);
};  // namespace log
