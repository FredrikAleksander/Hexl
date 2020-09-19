#pragma once
#ifndef __HEXL_UI__SCHEMECONSOLEENVIRONMENT_HPP
#define __HEXL_UI__SCHEMECONSOLEENVIRONMENT_HPP 1

#include <Hexl/UI/Console.hpp>
#include <Hexl/Scheme/SchemeEnvironment.hpp>

namespace Hexl {
namespace UI {

class SchemeConsoleEnvironment final: public ConsoleEnvironment {
	private:
		Scheme::SchemeEnvironment& m_schemeEnvironment;
		ImVec4 m_textColor;
		ImVec4 m_warningColor;
	protected:
		virtual void AddLines(Hexl::LogLevel defaultLogLevel, Hexl::String&& lines);

		virtual bool IsCommandComplete(const char* command) override;
		virtual void EvalCommand(const char* command) override;
	public:
		SchemeConsoleEnvironment(Scheme::SchemeEnvironment& schemeEnvironment);
		virtual ~SchemeConsoleEnvironment();
};

}
}

#endif
