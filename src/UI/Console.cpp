#include "imgui.h"
#include <Hexl/UI/Console.hpp>

using namespace Hexl;
using namespace UI;

ConsoleEnvironment::ConsoleEnvironment()
	: m_items()
	, m_history()
	, m_historyPos(0)
{
}

ConsoleEnvironment::~ConsoleEnvironment() {
	ClearLog();
	for(int i = 0; i < m_history.size(); i++) {
		free(m_history[i]);
	}
}

void ConsoleEnvironment::ClearLog() {
        for (int i = 0; i < m_items.Size; i++)
            free(m_items[i].item);
        m_items.clear();
}
static char* Strdup(const char* s)                           { size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
 
static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

void ConsoleEnvironment::AddLog(LogCategory category, LogLevel level, const char* fmt, ...) {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
	m_items.push_back({ .level=level, .item=Strdup(buf) });
}

void ConsoleEnvironment::Eval(const char *command) {
	EvalCommand(command);
}

Console::Console()
	: Window("console", "Console")
	, m_showWindow(true)
	, m_autoScroll(true)
	, m_scrollToBottom(false)
	, m_textColor(1.0f, 1.0f, 1.0f, 1.0f)
	, m_warningColor(1.0f, 1.0f, 0.0f, 1.0f)
	, m_errorColor(1.0f, 0.0f, 0.0f, 1.0f)
{
	SetIcon("\uf120");
}

Console::~Console()
{
}

void Console::RegisterThemeProperties(Theme& theme) {
	// TODO: Set default colors when not already bound
	theme.BindColor("text", [this](auto& name, float r, float g, float b, float a) { 
		m_textColor.x = r; 
		m_textColor.y = g; 
		m_textColor.z = b; 
		m_textColor.w = a; 
	}, true);
	theme.BindColor("text-warning", [this](auto& name, float r, float g, float b, float a) {
		m_warningColor.x = r;
		m_warningColor.y = g;
		m_warningColor.z = b;
		m_warningColor.w = a;
	}, true);
	theme.BindColor("text-error", [this](auto& name, float r, float g, float b, float a) {
		m_errorColor.x = r;
		m_errorColor.y = g;
		m_errorColor.z = b;
		m_errorColor.w = a;
	}, true);
}

static int TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
	Console* console = (Console*)data->UserData;
	return console->TextEditCallback(data);
}

void Console::Draw(UIRenderer &renderer) {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if(!ImGui::Begin(GetLabel().c_str(), &m_showWindow))
	{
		ImGui::End();
		return;
	}

	// Clean up unreferenced environments
	bool done = false;
	while(!done) {
		done = true;
		for(auto i = m_environments.begin(); i != m_environments.end(); ++i) {
			if(i->expired()) {
				m_environments.erase(i);
				done = false;
				break;
			}
		}
	}

	auto ptrEnv = GetCurrentEnvironment();
        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close Console"))
                m_showWindow = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped(
            "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
            "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        ImGui::TextWrapped("Enter 'HELP' for help.");

        // TODO: display items starting from the bottom

        if (ImGui::SmallButton("Clear"))           { if(ptrEnv) ptrEnv->ClearLog(); }
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton("Copy");
        //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

        ImGui::Separator();

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &m_autoScroll);
            ImGui::EndPopup();
        }

        // Options, Filter
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        m_filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) { if(ptrEnv) ptrEnv->ClearLog(); }
            ImGui::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets.
        // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        // to only process visible items. The clipper will automatically measure the height of your first item and then
        // "seek" to display only items in the visible area.
        // To use the clipper we can replace your standard loop:
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      ImGuiListClipper clipper(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - That your items are evenly spaced (same height)
        // - That you have cheap random access to your elements (you can access them given their index,
        //   without processing all the ones before)
        // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        // We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        // to improve this example code!
        // If your items are of variable height:
        // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            ImGui::LogToClipboard();
	ImGui::PushFont(renderer.GetMonospacedFont());
	if(ptrEnv) {
	        for (int i = 0; i < ptrEnv->m_items.Size; i++)
	        {
	            const char* item = ptrEnv->m_items[i].item;
	            if (!m_filter.PassFilter(item))
	                continue;
	
	            bool has_color = false;
		    switch(ptrEnv->m_items[i].level) {

			    case LogLevel::LOG_FATAL:
			    case LogLevel::LOG_ERROR:
	                	    ImGui::PushStyleColor(ImGuiCol_Text, m_errorColor);
				    has_color = true;
				    break;
			    case LogLevel::LOG_WARN:
	                	    ImGui::PushStyleColor(ImGuiCol_Text, m_warningColor);
				    has_color = true;
				    break;
			    default:
				    break;
		    }
	            ImGui::TextUnformatted(item);
	            if (has_color)
	                ImGui::PopStyleColor();
	        }
	}
	ImGui::PopFont();
        if (copy_to_clipboard)
            ImGui::LogFinish();

        if (m_scrollToBottom || (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        m_scrollToBottom = false;

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
	ImGui::PushFont(renderer.GetMonospacedFont());
        if (ImGui::InputText("Input", m_inputBuf, IM_ARRAYSIZE(m_inputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
        {
            char* s = m_inputBuf;
            Strtrim(s);
            if (s[0] && ptrEnv) {
		ptrEnv->Eval(s);
	    }
            strcpy(s, "");
            reclaim_focus = true;
        }
        // Auto-focus on window apparition
	ImGui::PopFont();
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        ImGui::End();
}

std::shared_ptr<ConsoleEnvironment> Console::GetCurrentEnvironment()
{
	std::shared_ptr<ConsoleEnvironment> ptrEnv = m_currentEnvironment.lock();
	if(!ptrEnv) {
		for(auto& i : m_environments) {
			auto p = i.lock();
			if(p) {
				ptrEnv = p;
				break;
			}
		}
	}
	if(ptrEnv)
		m_currentEnvironment = ptrEnv;
	return ptrEnv;
}

void Console::AddEnvironment(const std::shared_ptr<ConsoleEnvironment> &env) {
	m_environments.push_back(env);
}

int Console::TextEditCallback(ImGuiInputTextCallbackData *data) {
	auto ptrEnv = GetCurrentEnvironment();
	switch(data->EventFlag) {
		case ImGuiInputTextFlags_CallbackCompletion:
			if(!ptrEnv) {
				return 0;
			}
			break;
		case ImGuiInputTextFlags_CallbackHistory:
			if(!ptrEnv)
				return 0;
			// Example of HISTORY
			const int prev_history_pos = ptrEnv->m_historyPos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
			    if (ptrEnv->m_historyPos == -1)
			        ptrEnv->m_historyPos = ptrEnv->m_history.Size - 1;
			    else if (ptrEnv->m_historyPos > 0)
			        ptrEnv->m_historyPos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
			    if (ptrEnv->m_historyPos != -1)
			        if (++ptrEnv->m_historyPos >= ptrEnv->m_history.Size)
			            ptrEnv->m_historyPos = -1;
			}
			
			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != ptrEnv->m_historyPos)
			{
			    const char* history_str = (ptrEnv->m_historyPos >= 0) ? ptrEnv->m_history[ptrEnv->m_historyPos] : "";
			    data->DeleteChars(0, data->BufTextLen);
			    data->InsertChars(0, history_str);
			}

			break;
	}
	return 0;
}
