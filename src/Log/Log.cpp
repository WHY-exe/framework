#include "Log.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/android_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "FileSinkEx.h"
#include "RotationFileSinkEx.h"

void Log::Init(const Config& logConfig) {
	std::vector<spdlog::sink_ptr> sink_list;
	if (logConfig.enableConsole) {
		auto console_sink =
#ifndef __ANDROID__
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#else
            std::make_shared<spdlog::sinks::android_sink_mt>();
#endif
		console_sink->set_pattern(logConfig.pattern);
		console_sink->set_level(logConfig.logLevel);
		sink_list.push_back(std::move(console_sink));
	}
	if (logConfig.enableFile) {
		if (logConfig.enableRotate) {
			auto file_sink = std::make_shared<spdlog::sinks::RotationFileSinkExMt>(logConfig.logPath.string(), logConfig.maxSize, logConfig.maxFiles);
			file_sink->set_pattern(logConfig.pattern);
			file_sink->set_level(logConfig.logLevel);
			sink_list.push_back(std::move(file_sink));
		} else {
			auto file_sink = std::make_shared<spdlog::sinks::FileSinkExMt>(logConfig.logPath.string());
			file_sink->set_pattern(logConfig.pattern);
			file_sink->set_level(logConfig.logLevel);
			sink_list.push_back(std::move(file_sink));
		}
	}
	auto default_logger = std::make_shared<spdlog::logger>("default", sink_list.begin(), sink_list.end());
	default_logger->set_level(logConfig.logLevel);
	spdlog::set_level(logConfig.logLevel);
	spdlog::set_default_logger(std::move(default_logger));
	spdlog::flush_every(std::chrono::seconds(3));
}
