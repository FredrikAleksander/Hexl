#pragma once
#include "Hexl/Log.hpp"
#include <memory>
#ifndef __HEXL_UI__CONSOLE_HPP
#define __HEXL_UI__CONSOLE_HPP 1

#include <Hexl/UI/Window.hpp>
#include <list>

namespace Hexl {
namespace UI {

class Console;
class ConsoleEnvironment {
	private:
		struct Item {
			LogLevel level;
			char*    item;
		};
		ImVector<Item> m_items;
		ImVector<char*> m_history;
		int             m_historyPos;
		friend class Console;
	protected:
		void ClearLog();
		void AddLog(LogCategory category, LogLevel level, const char* fmt, ...) IM_FMTARGS(4);

		virtual bool IsCommandComplete(const char* command) = 0;
		virtual void EvalCommand(const char* command) = 0;
	public:
		ConsoleEnvironment();
		virtual ~ConsoleEnvironment();

		void Eval(const char* command);
};

class Console final: public Window {
	private:
		char            m_inputBuf[512];
		bool            m_showWindow;
		ImGuiTextFilter m_filter;
		bool            m_autoScroll;
		bool            m_scrollToBottom;
		std::weak_ptr<ConsoleEnvironment> m_currentEnvironment;
		std::list< std::weak_ptr<ConsoleEnvironment> > m_environments;
		ImVec4          m_textColor;
		ImVec4          m_warningColor;
		ImVec4          m_errorColor;
	protected:
		virtual void RegisterThemeProperties(UI::Theme& theme) override;
	public:
		Console();
		virtual ~Console();

		int TextEditCallback(ImGuiInputTextCallbackData* data);
		std::shared_ptr<ConsoleEnvironment> GetCurrentEnvironment();
		void AddEnvironment(const std::shared_ptr<ConsoleEnvironment>& env);

		virtual void Draw(UIRenderer& renderer) override;
};

}
}

#endif
