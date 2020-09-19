#include <Hexl/Log.hpp>
#include <Hexl/String.hpp>
#include <SDL.h>
#include <SDL_log.h>
#ifdef WIN32
#include <io.h>
#endif

static bool s_logInitialized = false;
static Hexl::String s_logOutputPath;
static FILE* s_logOutputFile = NULL;

static const char* PrintLogLevel(Hexl::LogLevel level) {
	switch(level) {
		case Hexl::LogLevel::LOG_DEBUG:
			return "DEBUG:   ";
		case Hexl::LogLevel::LOG_INFO:
			return "INFO:    ";
		case Hexl::LogLevel::LOG_WARN:
			return "WARN:    ";
		case Hexl::LogLevel::LOG_ERROR:
			return "ERROR:   ";
		case Hexl::LogLevel::LOG_FATAL:
			return "FATAL:   ";
		default:
			return "VERBOSE: ";
	}
}

static const char* PrintCategory(Hexl::LogCategory category) {
	switch(category) {
		case Hexl::LogCategory::RUNTIME:
			return "RUNTIME     ";
		case Hexl::LogCategory::RENDERER:
			return "RENDERER    ";
		case Hexl::LogCategory::UI:
			return "UI          ";
		case Hexl::LogCategory::SCHEME:
			return "SCHEME      ";
		default:
			return "APPLICATION ";
	}
}

void Hexl::_internal::flushLog() {
	if(s_logOutputFile)
		fflush(s_logOutputFile);
}

FILE* Hexl::_internal::readLog() {
	return fopen(s_logOutputPath.c_str(), "r");
}

void Hexl::_internal::initLog() {
	if(s_logInitialized) {
		return;
	}
	s_logInitialized = true;
	char buf[] = "log_XXXXXX";
#ifndef WIN32
	
#else
	mktemp(buf);
#endif
	s_logOutputPath = buf;
	s_logOutputFile = fopen(buf, "w");
	fprintf(stdout, "Log File: %s\n", buf);
}

void Hexl::LogV(LogCategory category, LogLevel level, const char *fmt, va_list ap) {
	if(s_logOutputFile == nullptr) {
		return;
	}
	fprintf(s_logOutputFile, "%s%s", PrintLogLevel(level), PrintCategory(category));
	vfprintf(s_logOutputFile, fmt, ap);
	fprintf(s_logOutputFile, "\n");
}

void Hexl::Log(LogCategory category, LogLevel level, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	LogV(category, level, fmt, ap);
	va_end(ap);
}
