#pragma once
#ifndef __HEXL_UI__LOGWINDOW_HPP
#define __HEXL_UI__LOGWINDOW_HPP 1

#include <Hexl/UI/Window.hpp>
#include <Hexl/Log.hpp>
#include <stdio.h>
#include <imgui.h>

namespace Hexl {
namespace UI {

class LogWindow final: public Window {
	private:
		bool m_showWindow;
		FILE* m_logFile;
		char m_buffer[2048];

		ImGuiTextBuffer     m_buf;
		ImGuiTextFilter     m_filter;
		ImVector<int>       m_lineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		bool                m_autoScroll;  // Keep scrolling if already at the bottom.
		ImVec4              m_textColor;
		ImVec4              m_warningColor;
		ImVec4              m_errorColor;
	private:
		void Clear();
		void AddLine(const char* line);
		void Text(LogLevel level, const char* start, const char* end);
		void Text(const char* start, const char* end);

		virtual void RegisterThemeProperties(UI::Theme &theme) override;
	public:
		LogWindow();
		virtual ~LogWindow();

		virtual void Draw(UIRenderer& renderer) override;
};

}
}

#endif
