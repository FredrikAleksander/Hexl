#include "Hexl/Log.hpp"
#include "Hexl/Scheme/SchemeEnvironment.hpp"
#include "Hexl/UI/Theme.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "zep/buffer.h"
#include "zep/glyph_iterator.h"
#include "zep/mcommon/file/cpptoml.h"
#include "zep/mcommon/string/stringutils.h"
#include "zep/mode.h"
#include "zep/splits.h"
#include "zep/tab_window.h"
#include <Hexl/UI/TextEditor.hpp>
#include <SDL.h>
#include <SDL_clipboard.h>
#include <memory>
#include <utility>
#include <zep/editor.h>
#include <zep/theme.h>
//#include <zep/mode_repl.h>
#include <zep/imgui/editor_imgui.h>
#include "IconsFontAwesome5.h"

Zep::NVec2f GetDisplayScale()
{
	float ddpi = 0.0f;
	float hdpi = 0.0f;
	float vdpi = 0.0f;
	auto res = SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi);
	if (res == 0 && hdpi != 0)
	{
		return Zep::NVec2f(hdpi, vdpi) / 96.0f;
	}
	return Zep::NVec2f(1.0f);
}



namespace Hexl {
namespace UI {
	struct TextEditorContainer : public virtual Zep::IZepComponent {
		std::unique_ptr<Zep::ZepEditor_ImGui> spEditor;
		Scheme::SchemeEnvironment* schemeEnvironment;

		TextEditorContainer(const std::string& startupFilePath, const std::string& configPath, Scheme::SchemeEnvironment* schemeEnvironment)
			: spEditor(std::make_unique<Zep::ZepEditor_ImGui>(configPath))
			, schemeEnvironment(schemeEnvironment)
		{


			spEditor->RegisterCallback(this);
			spEditor->SetPixelScale(GetDisplayScale());
//
//			Zep::ZepReplExCommand::Register(*spEditor, this);
//			Zep::ZepReplEvaluateOuterCommand::Register(*spEditor, this);
//			Zep::ZepReplEvaluateInnerCommand::Register(*spEditor, this);
//			Zep::ZepReplEvaluateCommand::Register(*spEditor, this);
//
			if(!startupFilePath.empty()) {
				spEditor->InitWithFileOrDir(startupFilePath);
			}
			else {
				spEditor->InitWithText("Untitled", "");
			}
		}
		virtual ~TextEditorContainer() {
			spEditor->UnRegisterCallback(this);
		}

		virtual void Notify(std::shared_ptr<Zep::ZepMessage> message) override
		{
			std::string  command;
			std::string  res;
			int          arg;
			switch(message->messageId) {
				case Zep::Msg::Buffer:
					break;
				case Zep::Msg::ComponentChanged:
					break;
				case Zep::Msg::ConfigChanged:
					break;
				case Zep::Msg::GetClipBoard:
					if(SDL_HasClipboardText()) {
						message->str = SDL_GetClipboardText();
						message->handled = true;
					}
					break;
				case Zep::Msg::HandleCommand:
					command = message->str.substr(1);
					Zep::LTrim(command);
					// Assume that commands that start with ( is an S-Expression to be evaluated by the Scheme interpreter
					if(command.find("(") == 0) {
						if(schemeEnvironment->IsFormComplete(command, arg)) {
							res = schemeEnvironment->Repl(command);
							spEditor->SetCommandText(res);
							message->handled = true;
						}
					}
					else if(command.find("!") == 0) {
						// TODO: Run shell command
					}
					break;
				case Zep::Msg::MouseDown:
				case Zep::Msg::MouseMove:
					break;
				case Zep::Msg::MouseUp:
					break;
				case Zep::Msg::RequestQuit:
					break;
				case Zep::Msg::SetClipBoard:
					SDL_SetClipboardText(message->str.c_str());
					message->handled = true;
					break;
				case Zep::Msg::Tick:
					break;
				case Zep::Msg::ToolTip:
					break;
			}
		}
		
		virtual Zep::ZepEditor& GetEditor() const override
		{
			return *spEditor;
		}
	};
}
}

Hexl::UI::TextEditor::TextEditor(Hexl::Scheme::SchemeEnvironment* schemeEnvironment) :
	Window("_textEdit", "Text Editor"),
	m_showWindow(true),
	m_container(new Hexl::UI::TextEditorContainer("", SDL_GetBasePath(), schemeEnvironment))
{
	SetIcon(u8"\uF044");
}

Hexl::UI::TextEditor::~TextEditor()
{
	if(m_container) {
		delete m_container;
		m_container = nullptr;
	}
}

void Hexl::UI::TextEditor::OpenFile(const Hexl::String &path, int line) {
	auto pFileBuffer = m_container->spEditor->GetFileBuffer(path.c_str());

	auto windowsForBuffer = m_container->spEditor->FindBufferWindows(pFileBuffer);
	Zep::ZepWindow* window = nullptr;

	Zep::ZepTabWindow* tabWindow = nullptr;

	if(m_container->spEditor->GetTabWindows().empty())
	{
		tabWindow = m_container->spEditor->AddTabWindow();
	}
	else if (m_container->spEditor->GetActiveTabWindow())
	{
		tabWindow = m_container->spEditor->GetActiveTabWindow();
	}
	else {
		tabWindow = m_container->spEditor->GetTabWindows().front();
	}

	std::function<bool(Zep::ZepTabWindow*, Zep::ZepWindow*&)> matcher = [&windowsForBuffer](auto tabWin, auto& win) {
		for(auto windowInTab : tabWin->GetWindows()) {
			auto i = std::find(windowsForBuffer.begin(), windowsForBuffer.end(), windowInTab);
			if(i != windowsForBuffer.end()) {
				win = *i;
				return true;
			}
		}

		return false;
	};

	if(!matcher(tabWindow, window)) {
		for(auto t : m_container->spEditor->GetTabWindows()) {
			if(matcher(t, window)) {
				tabWindow = t;
				break;
			}
		}
	}

	m_showWindow = true;

	if(window == nullptr) {
	// TODO: Support a preference that decides what to do if the file is not
	//       already open. Either open a new tab, a new window, or replace the buffer
	//       of a existing window
		tabWindow = m_container->spEditor->AddTabWindow();
		window = tabWindow->AddWindow(pFileBuffer, nullptr, Zep::RegionLayoutType::HBox);
		window->SetBuffer(pFileBuffer);
	}

	m_container->spEditor->SetCurrentTabWindow(tabWindow);
	tabWindow->SetActiveWindow(window);

	if(line >= 0) {
		Zep::ByteRange lineRange = window->GetBuffer().GetLineOffsets(line, lineRange);
		window->SetBufferCursor(Zep::GlyphIterator(&window->GetBuffer(), lineRange.first));
	}
}

static void RegisterZepColor(Hexl::UI::Theme& theme, Zep::ZepEditor* editor, Zep::ThemeColor zepColor, const Hexl::String& name, Hexl::String&& description) {
	auto& zepTheme = editor->GetTheme();
	auto& zepColorData = zepTheme.GetColor(zepColor);
	if(!theme.IsColorDescribed(name)) {
		theme.DescribeColor(name, std::move(description));
	}
	Hexl::UI::Theme::ColorChangeHandler colorChangeHandler = [editor, zepColor](auto& name, auto r, auto g, auto b, auto a) {
		editor->GetTheme().SetColor(zepColor, Zep::NVec4f(r, g, b, a));	
	};
	if(!theme.IsColorDefined(name)) {
		theme.SetColor(name, zepColorData.x, zepColorData.y, zepColorData.z, zepColorData.w);
		theme.BindColor(name, std::move(colorChangeHandler), false);
	}
	else {
		theme.BindColor(name, std::move(colorChangeHandler), true);
	}
}

static void RegisterZepScalar(Hexl::UI::Theme& theme, Zep::ZepEditor* editor, const Hexl::String& name, decltype(&Zep::EditorConfig::underlineHeight) member, Hexl::String&& description) {
	auto config = &editor->GetConfig();
	if(!theme.IsScalarDescribed(name)) {
		theme.DescribeScalar(name, std::move(description));
	}
	Hexl::UI::Theme::ScalarChangeHandler scalarChangeHandler = [config, editor, member](auto& name, auto value) {

		((*config).*member) = value;
		auto msg = std::make_shared<Zep::ZepMessage>(Zep::Msg::ConfigChanged);
		editor->Broadcast(msg);
	};
	if(!theme.IsScalarDefined(name)) {
		theme.SetScalar(name, ((*config).*member));
		theme.BindScalar(name, std::move(scalarChangeHandler), false);
	}
	else {
		theme.BindScalar(name, std::move(scalarChangeHandler), true);
	}
}

static void RegisterZepFlag(Hexl::UI::Theme& theme, Zep::ZepEditor* editor, const Hexl::String& name, decltype(&Zep::EditorConfig::showLineNumbers) member, Hexl::String&& description)
{
	auto config = &editor->GetConfig();
	if(!theme.IsFlagDescribed(name)) {
		theme.DescribeFlag(name, std::move(description));
	}
	Hexl::UI::Theme::FlagChangeHandler flagChangeHandler = [config, editor, member](auto& name, auto value) {
		((*config).*member) = value;
		auto msg = std::make_shared<Zep::ZepMessage>(Zep::Msg::ConfigChanged);
		editor->Broadcast(msg);

	};
	if(!theme.IsFlagDefined(name)) {
		bool b = ((*config).*member);
		theme.SetFlag(name, b);
		theme.BindFlag(name, std::move(flagChangeHandler), false);
	}
	else {
		theme.BindFlag(name, std::move(flagChangeHandler), true);
	}

}

static void RegisterZepVec2(Hexl::UI::Theme& theme, Zep::ZepEditor* editor, const Hexl::String& name, decltype(&Zep::EditorConfig::lineMargins) member, Hexl::String&& description) {
	auto config = &editor->GetConfig();
	if(!theme.IsVec2Described(name)) {
		theme.DescribeVec2(name, std::move(description));
	}
	Hexl::UI::Theme::Vec2ChangeHandler vecChangeHandler = [config, editor, member](auto& name, auto x, auto y) {
		auto msg = std::make_shared<Zep::ZepMessage>(Zep::Msg::ConfigChanged);
		editor->Broadcast(msg);

	};
	if(!theme.IsVec2Defined(name)) {
		theme.SetVec2(name, ((*config).*member).x, ((*config).*member).y);
		theme.BindVec2(name, std::move(vecChangeHandler));
	}
	else {
		theme.BindVec2(name, std::move(vecChangeHandler));
	}
}

void Hexl::UI::TextEditor::RegisterThemeProperties(Hexl::UI::Theme& theme) {
	auto zepTheme = m_container->spEditor->GetTheme();
	auto editor = m_container->spEditor.get();

	auto root = cpptoml::make_table();
	root->insert("editor", cpptoml::make_table());
	editor->LoadConfig(root);
	auto config = editor->GetConfig();
	RegisterZepColor(theme, editor, Zep::ThemeColor::TabBorder, "tab-border", "Tab border color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::HiddenText, "text-hidden", "Hidden text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Text, "text", "Text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::TextDim, "text-disabled", "Text disabled color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Background, "window-background", "Background color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::TabInactive, "tab", "Tab color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::TabActive, "tab-active", "Tab active color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::LineNumberBackground, "line-number-background", "Line numer background color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::LineNumber, "line-number", "Line number color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::LineNumberActive, "line-number-active", "Line number active background color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::CursorNormal, "text-cursor-normal", "Text cursor normal color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::CursorInsert, "text-cursor-insert", "Text cursor insert color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Light, "light", "Light color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Dark, "dark", "Dark color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::VisualSelectBackground, "visual-select-background", "Visual select background");
	RegisterZepColor(theme, editor, Zep::ThemeColor::CursorLineBackground, "text-cursor-line-background", "Text cursor line background");
	RegisterZepColor(theme, editor, Zep::ThemeColor::AirlineBackground, "zepline-background", "Text editor status line background color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Mode, "zepline-mode", "Text editor mode status line color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Normal, "text-normal", "Normal text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Keyword, "text-keyword", "Keyword text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Identifier, "text-identifier", "Identifier text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Number, "text-number", "Number text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::String, "text-string", "String literal text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Comment, "text-comment", "Comment text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Whitespace, "text-whitespace", "Whitespace text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::HiddenChar, "text-hidden", "Hidden text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Parenthesis, "text-parenthesis", "Parenthesis text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Error, "text-error", "Error text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Warning, "text-warning", "Warning text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::Info, "text-info", "Info text color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::WidgetBorder, "border", "Border color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::WidgetBackground, "frame-background", "Frame background color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::WidgetActive, "frame-active", "Frame active color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::WidgetInactive, "frame", "Frame color");
	RegisterZepColor(theme, editor, Zep::ThemeColor::FlashColor, "flash-color", "Flash color");


	RegisterZepScalar(theme, editor, "text-editor/underline-height", &Zep::EditorConfig::underlineHeight, "Text Editor underline height");
	RegisterZepScalar(theme, editor, "text-editor/background-fade-time", &Zep::EditorConfig::backgroundFadeTime, "Text Editor background fade time");
	RegisterZepScalar(theme, editor, "text-editor/background-fade-wait", &Zep::EditorConfig::backgroundFadeWait, "Text Editor background wait time");

	RegisterZepFlag(theme, editor, "text-editor/show-line-numers", &Zep::EditorConfig::showLineNumbers, "Show Text Editor line numbers");
	RegisterZepFlag(theme, editor, "text-editor/short-tab-names", &Zep::EditorConfig::shortTabNames, "Text Editor short tab names");
	RegisterZepFlag(theme, editor, "text-editor/show-indicator-region", &Zep::EditorConfig::showIndicatorRegion, "Show text editor indicator region");
	RegisterZepFlag(theme, editor, "text-editor/auto-hide-command-region", &Zep::EditorConfig::autoHideCommandRegion, "Auto hide text editor command region");
	RegisterZepFlag(theme, editor, "text-editor/cursor-line-solid", &Zep::EditorConfig::cursorLineSolid, "Solid cursor line in text editor");
	RegisterZepFlag(theme, editor, "text-editor/show-normal-mode-keystrokes", &Zep::EditorConfig::showNormalModeKeyStrokes, "Show normal mode keystrokes in text editor");

	RegisterZepVec2(theme, editor, "text-editor/line-margins", &Zep::EditorConfig::lineMargins, "Line margins in text editor");
	RegisterZepVec2(theme, editor, "text-editor/widget-margins", &Zep::EditorConfig::widgetMargins, "Widget margins in text editor");
	RegisterZepVec2(theme, editor, "text-editor/inline-widget-margins", &Zep::EditorConfig::inlineWidgetMargins, "Inline widget margins in text editor");

}

void Hexl::UI::TextEditor::Draw(UIRenderer &renderer) {
	if(!m_showWindow)
		return;
	auto m_backgroundColor = m_container->spEditor->GetTheme().GetColor(Zep::ThemeColor::Background);
	ImGui::PushFont(renderer.GetMonospacedFont());
	ImGui::PushStyleColor(ImGuiCol_WindowBg,  ImVec4(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, m_backgroundColor.w));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

	auto focusReceived = ReceiveFocus();
	
	if(focusReceived)
		ImGui::SetNextWindowFocus();

	if(!ImGui::Begin(GetLabel().c_str(), &m_showWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
		ImGui::End();
		ImGui::PopStyleVar(4);
        	ImGui::PopStyleColor(1);
		ImGui::PopFont();
		return;
	}

	auto& io = ImGui::GetIO();
	auto min = ImGui::GetCursorScreenPos();
	auto max = ImGui::GetContentRegionAvail();
	max.x = std::max(1.0f, max.x);
	max.y = std::max(1.0f, max.y);

	max.x = min.x + max.x;
	max.y = min.y + max.y;

	ImGui::InvisibleButton("zepEditor", max);
	ImGui::SetKeyboardFocusHere(-1);

	m_container->spEditor->SetDisplayRegion(Zep::NVec2f(min.x, min.y), Zep::NVec2f(max.x, max.y));
	m_container->spEditor->Display();

	if (ImGui::IsWindowFocused())
	{
		io.WantCaptureKeyboard = true;
		io.WantCaptureMouse = true;
		m_container->spEditor->HandleInput();
	}

	ImGui::End();
	ImGui::PopStyleVar(4);
        ImGui::PopStyleColor(1);
	ImGui::PopFont();
}
