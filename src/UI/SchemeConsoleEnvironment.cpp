#include "imgui.h"
#include <Hexl/UI/SchemeConsoleEnvironment.hpp>
#include <sstream>

using namespace Hexl;
using namespace Hexl::UI;
using namespace Hexl::Scheme;

SchemeConsoleEnvironment::SchemeConsoleEnvironment(SchemeEnvironment& schemeEnvironment)
	: m_schemeEnvironment(schemeEnvironment)
{
}

SchemeConsoleEnvironment::~SchemeConsoleEnvironment() {
}

void SchemeConsoleEnvironment::AddLines(Hexl::LogLevel defaultLogLevel, Hexl::String&& lines) {
	static const Hexl::String delim = "\n";

	if(lines.empty())
		return;

	std::istringstream iss(std::move(lines));
	
	for (std::string line; std::getline(iss, line); )
	{
		Hexl::LogLevel level = defaultLogLevel;
		if(line.find("WARNING") == 0) {
			level = LogLevel::LOG_WARN;
		}
		else if(line.find("ERROR") == 0) {
			level = LogLevel::LOG_ERROR;
		}
		AddLog(LogCategory::SCHEME, level, "%s", line.c_str());

	}

}

bool SchemeConsoleEnvironment::IsCommandComplete(const char* command) {
	int unused;
	return m_schemeEnvironment.IsFormComplete(command, unused);
}

void SchemeConsoleEnvironment::EvalCommand(const char *command) {
	AddLines(Hexl::LogLevel::LOG_INFO, m_schemeEnvironment.Repl(command));
}
