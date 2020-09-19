#include "imgui.h"
#include <Hexl/UI/LogWindow.hpp>
#include <Hexl/Log.hpp>

#include <SDL.h>
#include <SDL_filesystem.h>
#include <zep/editor.h>
#include <zep/imgui/editor_imgui.h>

Hexl::UI::LogWindow::LogWindow() :
	Window("_logWin", "Log"),
	m_showWindow(true),
	m_logFile(_internal::readLog()),
	m_autoScroll(true)
{
	Clear();
	SetIcon("\uf15c");
}

void Hexl::UI::LogWindow::Clear() {
	m_buf.clear();
	m_lineOffsets.clear();
	m_lineOffsets.push_back(0);
}

Hexl::UI::LogWindow::~LogWindow()
{
}

void Hexl::UI::LogWindow::AddLine(const char* line) {
	int old_size = m_buf.size();
	m_buf.append(line);
	for (int new_size = m_buf.size(); old_size < new_size; old_size++)
		if (m_buf[old_size] == '\n')
			m_lineOffsets.push_back(old_size + 1);

}

void Hexl::UI::LogWindow::Text(LogLevel level, const char* start, const char* end) {
	bool pop = false;
	switch(level) {
		case LogLevel::LOG_FATAL:
		case LogLevel::LOG_ERROR:
			pop = true;
			ImGui::PushStyleColor(ImGuiCol_Text, m_errorColor);
			break;
		case LogLevel::LOG_WARN:
			pop = true;
			ImGui::PushStyleColor(ImGuiCol_Text, m_warningColor);
			break;
		default:
			break;
	}
	ImGui::TextUnformatted(start, end);

	if(pop) {
		ImGui::PopStyleColor();
	}
}

void Hexl::UI::LogWindow::Text(const char* start, const char* end) {
	bool pop = false;
	ImVec4 col;
	LogLevel level = LogLevel::LOG_VERBOSE;
	if(end-start>sizeof("VERBOSE:")) {
		if(strncmp("FATAL:", start, sizeof("FATAL:")-1) == 0) {
			level = LogLevel::LOG_FATAL;
		}
		else if(strncmp("ERROR:", start, sizeof("ERROR:")-1) == 0) {
			level = LogLevel::LOG_ERROR;
		}
		else if(strncmp("WARN:", start, sizeof("WARN:")-1) == 0) {
			level = LogLevel::LOG_WARN;
		}
		else if(strncmp("INFO:", start, sizeof("INFO:")-1) == 0) {
			level = LogLevel::LOG_INFO;
		}
		else if(strncmp("DEBUG:", start, sizeof("DEBUG:")-1) == 0) {
			level = LogLevel::LOG_DEBUG;
		}
	}
	Text(level, start, end);
}

void Hexl::UI::LogWindow::RegisterThemeProperties(UI::Theme& theme) {
	theme.BindColor("text", [this](auto&, auto r, auto g, auto b, auto a) {
		this->m_textColor.x = r;
		this->m_textColor.y = g;
		this->m_textColor.z = b;
		this->m_textColor.w = a;
	});
	theme.BindColor("text-warning", [this](auto&, auto r, auto g, auto b, auto a) {
		this->m_warningColor.x = r;
		this->m_warningColor.y = g;
		this->m_warningColor.z = b;
		this->m_warningColor.w = a;
	});
	theme.BindColor("text", [this](auto&, auto r, auto g, auto b, auto a) {
		this->m_errorColor.x = r;
		this->m_errorColor.y = g;
		this->m_errorColor.z = b;
		this->m_errorColor.w = a;
	});
}

void Hexl::UI::LogWindow::Draw(UIRenderer& renderer) {

	if (!ImGui::Begin(GetLabel().c_str(), &m_showWindow))
	{
		ImGui::End();
		return;
	}
	while(feof(m_logFile) == 0) {
		if(fgets(m_buffer, 2048, m_logFile) == nullptr)
			break;
		AddLine(m_buffer);
	}

	clearerr(m_logFile);

	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &m_autoScroll);
		ImGui::EndPopup();
	}

	if (ImGui::Button("\uf013 Options"))
		ImGui::OpenPopup("Options");
	ImGui::SameLine();
	bool clear = ImGui::Button("\uf1f8 Clear");
	ImGui::SameLine();
	bool copy = ImGui::Button("\uf0c5 Copy");
	ImGui::SameLine();
	m_filter.Draw("\uf0b0 Filter", -100.0f);

	ImGui::Separator();
	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	if (clear)
		Clear();
	if (copy)
		ImGui::LogToClipboard();
	ImGui::PushFont(renderer.GetMonospacedFont());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	const char* buf = m_buf.begin();
	const char* buf_end = m_buf.end();

	if (m_filter.IsActive())
	{
		for (int line_no = 0; line_no < m_lineOffsets.Size; line_no++)
		{
			const char* line_start = buf + m_lineOffsets[line_no];
			const char* line_end = (line_no + 1 < m_lineOffsets.Size) ? (buf + m_lineOffsets[line_no + 1] - 1) : buf_end;
			if (m_filter.PassFilter(line_start, line_end))
				Text(line_start, line_end);
		}
	}
	else
	{
		ImGuiListClipper clipper;
		clipper.Begin(m_lineOffsets.Size);
		while (clipper.Step())
		{
			for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
			{
				const char* line_start = buf + m_lineOffsets[line_no];
				const char* line_end = (line_no + 1 < m_lineOffsets.Size) ? (buf + m_lineOffsets[line_no + 1] - 1) : buf_end;
				Text(line_start, line_end);
			}
		}
		clipper.End();
	}

	ImGui::PopStyleVar();
	ImGui::PopFont();
	if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
	ImGui::EndChild();

	ImGui::End();
}
