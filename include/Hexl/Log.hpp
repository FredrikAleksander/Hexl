#pragma once
#ifndef __HEXL__LOG_HPP
#define __HEXL__LOG_HPP 1

#include <stdio.h>
#include <stdarg.h>

namespace Hexl {
	namespace _internal {
		void flushLog();
		void initLog();
		FILE* readLog();
	}
	enum class LogCategory {
		RUNTIME,
		RENDERER,
		UI,
		SCHEME
	};
	enum class LogLevel {
		LOG_VERBOSE,
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARN,
		LOG_ERROR,
		LOG_FATAL
	};
	void LogV(LogCategory category, LogLevel level, const char* fmt, va_list ap);
	void Log(LogCategory category, LogLevel level, const char* fmt, ...);
}

#define LOG_VERBOSE(cat, fmt, ...) do { ::Hexl::Log(cat, ::Hexl::LogLevel::LOG_VERBOSE, fmt, __VA_ARGS__); } while(0)
#define LOG_DEBUG(cat, fmt, ...)   do { ::Hexl::Log(cat, ::Hexl::LogLevel::LOG_DEBUG, fmt, __VA_ARGS__); } while(0)
#define LOG_INFO(cat, fmt, ...)    do { ::Hexl::Log(cat, ::Hexl::LogLevel::LOG_INFO, fmt, __VA_ARGS__); } while(0)
#define LOG_WARN(cat, fmt, ...)    do { ::Hexl::Log(cat, ::Hexl::LogLevel::LOG_WARN, fmt, __VA_ARGS__); } while(0)
#define LOG_ERROR(cat, fmt, ...)   do { ::Hexl::Log(cat, ::Hexl::LogLevel::LOG_ERROR, fmt, __VA_ARGS__); } while(0)
#define LOG_FATAL(cat, fmt, ...)   do { ::Hexl::Log(cat, ::Hexl::LogLevel::LOG_FATAL, fmt, __VA_ARGS__); } while(0)


#endif
