#pragma once
#include <ADV/MetaString.h>
#include <spdlog/spdlog.h>
#include <boost/filesystem.hpp>


#ifndef SPDLOG_NO_SOURCE_LOC
#	define LOGGER_CALL(logger, level, ...) \
		(logger)->log(spdlog::source_loc{ OBFUSCATED(__FILE__), __LINE__, SPDLOG_FUNCTION }, level, __VA_ARGS__)
#else
#	define LOGGER_CALL(logger, level, ...) \
		(logger)->log(spdlog::source_loc{}, level, __VA_ARGS__)
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
#	define LOGT(...) LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::trace, __VA_ARGS__)
#else
#	define LOGT(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
#	define LOGD(...) LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::debug, __VA_ARGS__)
#else
#	define LOGD(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
#	define LOGI(...) LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::info, __VA_ARGS__)
#else
#	define LOGI(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
#	define LOGW(...) LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::warn, __VA_ARGS__)
#else
#	define LOGW(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
#	define LOGE(...) LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::error, __VA_ARGS__)
#else
#	define LOGE(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
#	define LOGC(...) LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::critical, __VA_ARGS__)
#else
#	define LOGC(...) (void)0
#endif

class Log {
public:
	using level = spdlog::level::level_enum;
	using path	= boost::filesystem::path;

	struct Config {
		level		logLevel	  = level::debug;
		bool		enableConsole = true;
		bool		enableFile	  = true;
		bool		enableRotate  = false;
		std::size_t maxSize		  = 10 * 1024;
		std::size_t maxFiles	  = 3;
		bool 		enctyptEnable = false;
		std::string encrptKey 	  = "";
		path		logPath		  = OBFUSCATED("logs/default.log");
		std::string pattern		  = OBFUSCATED("[%Y-%m-%d %H:%M:%S.%e][%L][%s:%#] %v");
	};

	static void Init(const Config& logConfig);
};
