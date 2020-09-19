#include "imgui.h"
#include "Hexl/UI/Theme.hpp"
#include "imgui_impl_opengl3.h"
#include <Hexl/UI/UIRenderer.hpp>
#include <Hexl/Log.hpp>
#include <imgui.h>
#include <vector>

#include "../Sweet16mono_ImGui.inl"
#include "../FontAwesome.inl"
#include "IconsFontAwesome5.h"

static void RegisterImGuiColor(Hexl::UI::Theme& theme, ImVec4* colors, ImGuiCol color, const Hexl::String& name, Hexl::String&& description) {
	auto& clr = colors[color];
	theme.SetColor(name, clr.x, clr.y, clr.z, clr.w);
	theme.DescribeColor(name, std::move(description));
	theme.BindColor(name, [color](const Hexl::String& name, float r, float g, float b, float a) {
		ImGui::GetStyle().Colors[color] = ImVec4(r, g, b, a);
	}, false);
}

static void RegisterImGuiFlag(Hexl::UI::Theme& theme, ImGuiStyle& style, decltype(&ImGuiStyle::AntiAliasedLines) member, const Hexl::String& name, Hexl::String&& description) {
	auto& flag = (style.*member);
	theme.SetFlag(name, flag);
	theme.DescribeFlag(name, std::move(description));
	theme.BindFlag(name, [member](const Hexl::String& name, bool value) {
		ImGui::GetStyle().*member = value;
	}, false);
}

static void RegisterImGuiScalar(Hexl::UI::Theme& theme, ImGuiStyle& style, decltype(&ImGuiStyle::FrameRounding) member, const Hexl::String& name, Hexl::String&& description) {
	auto& scalar = (style.*member);
	theme.SetScalar(name, scalar);
	theme.DescribeScalar(name, std::move(description));
	theme.BindScalar(name, [member](const Hexl::String& name, float value) {
		ImGui::GetStyle().*member = value;
	}, false);
}

static void RegisterImGuiVec2(Hexl::UI::Theme& theme, ImGuiStyle& style, decltype(&ImGuiStyle::WindowPadding) member, const Hexl::String& name, Hexl::String&& description) {
	auto& vec = (style.*member);
	theme.SetVec2(name, vec.x, vec.y);
	theme.DescribeVec2(name, std::move(description));
	theme.BindVec2(name, [member](const Hexl::String& name, float x, float y) {
			auto& style = ImGui::GetStyle();
			auto& m = (ImGui::GetStyle().*member);
			m.x = x;
			m.y = y;
	}, false);
}

static void DeclareColor(Hexl::UI::Theme& theme, const Hexl::String& name, Hexl::String&& description, float r, float g, float b, float a) {
	if(!theme.IsColorDescribed(name)) {
		theme.DescribeColor(name, std::move(description));
	}
	if(!theme.IsColorDefined(name)) {
		theme.SetColor(name, r, g, b, a);
	}
}


Hexl::UI::UIRenderer::UIRenderer() :
	m_defaultFont(nullptr),
	m_monospacedFont(nullptr)
{
}

Hexl::UI::UIRenderer::~UIRenderer()
{
	LOG_VERBOSE(LogCategory::UI, "%s", "UI Renderer shutting down");
}

void Hexl::UI::UIRenderer::LoadStyle() {
	LOG_VERBOSE(LogCategory::UI, "%s", "Loading UI style");
	auto& io = ImGui::GetIO();
	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.16f, 0.17f, 0.20f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border]                 = ImVec4(0.67f, 0.70f, 0.75f, 1.00f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.01f, 0.27f, 0.66f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.01f, 0.27f, 0.66f, 0.54f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.01f, 0.27f, 0.66f, 0.54f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.01f, 0.14f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.01f, 0.27f, 0.66f, 0.54f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.08f, 0.32f, 0.62f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_DockingPreview]         = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	ImGuiStyle& style = ImGui::GetStyle();
	if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	style.FrameRounding = 1.0f;
	style.FrameBorderSize = 1.0f;

	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::Alpha, "alpha", "Global alpha applies to everything in the GUI");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::WindowPadding, "window-padding", "Padding within a window");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::WindowRounding, "window-rounding", "Radius of window corners rounding.");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::WindowBorderSize, "window-border-size", "Thickness of border around windows");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::WindowMinSize, "window-min-size", "Minimum window size");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::WindowTitleAlign, "window-title-align", "Alignment for title bar text");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::FrameRounding, "frame-rounding", "Frame rounding");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::FrameBorderSize, "frame-border-size", "Frame border size");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::ChildRounding, "child-rounding", "Radius of child window corners rounding");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::ChildBorderSize, "child-border-size", "Thickness of border around child windows");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::PopupRounding, "popup-rounding", "Radius of popup window corners rounding");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::PopupBorderSize, "popup-border-size", "Thickness of border around popup/tooltip windows");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::FramePadding, "frame-padding", "Padding within a framed rectangle(used by most widgets)");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::ItemSpacing, "item-spacing", "Horizontal and vertical specing between widgets/lines");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::ItemInnerSpacing, "item-inner-spacing", "Horizontal and vertical spacing between elements within a composed widget");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::TouchExtraPadding, "touch-extra-padding", "Expand reactive bounding box for touch based system where touch position is not accurate enough");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::IndentSpacing, "indent-spacing", "Horizontal indentation when e.g entering a tree node");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::ColumnsMinSpacing, "columns-min-spacing", "Minimum horizontal spacing between two columns");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::ScrollbarSize, "scrollbar-size", "Width of the vertical scrollbar, height of the horizontal scrollbar");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::ScrollbarRounding, "scrollbar-rounding", "Radius of grab corners for scrollbar");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::GrabMinSize, "grab-min-size", "Minimum width/height of a grab box for slider/scrollbar");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::GrabRounding, "grab-rounding", "Radius of grab corners for scrollbar");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::LogSliderDeadzone, "log-slider-deadzone", "The size in pixels of the deadzone around zero on logarithmic sliders that cross zero");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::TabRounding, "tab-rounding", "Radius of upper corners of a tab");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::TabBorderSize, "tab-border-size", "Thickness of border around tabs");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::TabMinWidthForUnselectedCloseButton, "tab-min-width-for-unselected-close-button", "Minimum width for close button to appear on an unselected tab when hovered");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::ButtonTextAlign, "button-text-align", "Alignment of button text when button is larger than text");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::SelectableTextAlign, "selectable-text-align", "Alignment of selectable text");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::DisplayWindowPadding, "display-window-padding", "Window position are clamped to be visible within the display area of monitors by atleast this amount");
	RegisterImGuiVec2(m_theme, style, &ImGuiStyle::DisplaySafeAreaPadding, "display-safe-area-padding", "If you cannot see the edges of your screen(e.g on a TV) increase the safe area padding");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::MouseCursorScale, "mouse-cursor-scale", "Mouse cursor scale");
	RegisterImGuiFlag(m_theme, style, &ImGuiStyle::AntiAliasedLines, "anti-aliased-lines", "Enable anti-aliased lines/borders");
	RegisterImGuiFlag(m_theme, style, &ImGuiStyle::AntiAliasedLinesUseTex, "anti-aliased-lines-using-textures", "Enable anti-aliased lines/borders using textures where possible");
	RegisterImGuiFlag(m_theme, style, &ImGuiStyle::AntiAliasedFill, "anti-aliased-fill", "Enable anti-aliased edges around filled shapes");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::CurveTessellationTol, "curve-tesselation-tolerance", "Tesselation tolerance when using PathBezierCurveTo()");
	RegisterImGuiScalar(m_theme, style, &ImGuiStyle::CircleSegmentMaxError, "circle-segment-max-error", "Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified");

	DeclareColor(m_theme, "text-warning", "Warning text color", 1.0f, 1.0f, 0.0f, 1.0f);
	DeclareColor(m_theme, "text-error", "Error text color", 1.0f, 0.0f, 0.0f, 1.0f);

	RegisterImGuiColor(m_theme, colors, ImGuiCol_Text, "text", "Default text color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_TextDisabled, "text-disabled", "Disabled text color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_WindowBg, "window-background", "Window background color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ChildBg, "child-background", "Child background color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_PopupBg, "popup-background", "Popup background color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_Border, "border", "Border color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_BorderShadow, "border-shadow", "Border shadow color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_FrameBg, "frame-background", "Frame background color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_FrameBgHovered, "frame-background-hovered", "Frame background hovered color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_FrameBgActive, "frame-background-active", "Frame background active color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_TitleBg, "title-background", "Title background color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_TitleBgActive, "title-background-active", "Title active background color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_TitleBgCollapsed, "title-background-collapsed", "Title collapsed background color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_MenuBarBg, "menu-bar-background", "Menu bar background");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ScrollbarBg, "scrollbar-background", "Scrollbar background");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ScrollbarGrab, "scrollbar-grab", "Scrollbar grab color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ScrollbarGrabHovered, "scrollbar-grab-hovered", "Scrollbar grab hovered color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ScrollbarGrabActive, "scrollbar-grab-active", "Scrollbar grab active color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_CheckMark, "check-mark", "Check mark color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_SliderGrab, "slider-grab", "Slider grab color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_SliderGrabActive, "slider-grab-active", "Slider grab active color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_Button, "button", "Button color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ButtonHovered, "button-hovered", "Button hovered color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ButtonActive, "button-active", "Button active color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_Header, "header", "Header color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_HeaderHovered, "header-hovered", "Header hovered color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_HeaderActive, "header-active", "Header active color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_Separator, "separator", "Separator color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_SeparatorHovered, "separator-hovered", "Separator hovered color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_SeparatorActive, "separator-active", "Separator active color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ResizeGrip, "resize-grip", "Resize grip color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ResizeGripHovered, "resize-grip-hovered", "Resize grip hovered color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ResizeGripActive, "resize-grip-active", "Resize grip active");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_Tab, "tab", "Tab color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_TabHovered, "tab-hovered", "Tab hovered color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_TabActive, "tab-active", "Tab active color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_TabUnfocused, "tab-unfocused", "Tab unfocused color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_TabUnfocusedActive, "tab-unfocused-active", "Tab unfocused active color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_DockingPreview, "docking-preview", "Docking preview color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_DockingEmptyBg, "docking-empty-background", "Docking empty background color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_PlotLines, "plot-lines", "Plot lines color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_PlotLinesHovered, "plot-lines-hovered", "Plot lines hovered color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_PlotHistogram, "plot-histogram", "Plot histogram color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_PlotHistogramHovered, "plot-histogram-hovered", "Plot histogram hovered color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_TextSelectedBg, "text-selected-background", "Text selected background");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_DragDropTarget, "drag-drop-target", "Drag and Drop target color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_NavHighlight, "nav-highlight", "Nav highlight color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_NavWindowingHighlight, "nav-window-highlight", "Nav window highlight color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_NavWindowingDimBg, "nav-window-dim-background", "Nav window dim background color");
	RegisterImGuiColor(m_theme, colors, ImGuiCol_ModalWindowDimBg, "modal-window-dim-background", "Modal window dim background color");
}

ImFont* Hexl::UI::UIRenderer::AddFont(const char* path, float size, int options, const ImWchar* glyphRanges) {
	//TODO: Improve this API to allow automatically merging icon fonts etc.
	LOG_VERBOSE(LogCategory::UI, "Loading UI font: %s", path);
	auto& io = ImGui::GetIO();
	const ImWchar* gr = glyphRanges;
	ImFontGlyphRangesBuilder builder;
	if(glyphRanges == nullptr || *glyphRanges == 0) {
		ImWchar rangeFontAwesome[3] = { 0xf000, 0xf2e0, 0 };
		builder.AddRanges(io.Fonts->GetGlyphRangesDefault()); // Add one of the default ranges
		builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic()); // Add one of the default ranges
		builder.AddRanges(rangeFontAwesome);
		builder.BuildRanges(&m_glyphRanges);
		gr = m_glyphRanges.Data;
	}

	auto res = io.Fonts->AddFontFromFileTTF(path, size, nullptr, gr);
	if(res != nullptr) {
		RebuildFontAtlas();
	}

	return res;
}

void Hexl::UI::UIRenderer::LoadFonts() {
	LOG_VERBOSE(LogCategory::UI, "%s", "Loading default UI fonts");
	auto& io = ImGui::GetIO();

	float defaultFontSize = 16.0f;
	float monospaceFontSize = 16.0f;

	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	//auto defaultFont = io.Fonts->AddFontDefault();
	auto defaultFont = io.Fonts->AddFontFromFileTTF("Roboto-Regular.ttf", defaultFontSize);

	ImFontConfig config;
	memcpy(config.Name, FONT_ICON_FILE_NAME_FAR, sizeof(FONT_ICON_FILE_NAME_FAR));
	config.MergeMode = true;
	config.OversampleH = 1;
	config.OversampleV = 1;
	config.PixelSnapH = 1;
	config.GlyphOffset.y = 1.0f;

	io.Fonts->AddFontFromMemoryCompressedBase85TTF(FontAwesome_compressed_data_base85, defaultFontSize, &config, icon_ranges);
	m_defaultFont = defaultFont;

	auto monospaceFont = AddSweet16MonoFont();
	config.GlyphOffset.y = -2.0f;
	io.Fonts->AddFontFromFileTTF("FontAwesome.otf", 16.0f, &config, icon_ranges);
	m_monospacedFont = monospaceFont;

	RebuildFontAtlas();
}

Hexl::UI::Theme& Hexl::UI::UIRenderer::GetTheme() {
	return m_theme;
}
const Hexl::UI::Theme& Hexl::UI::UIRenderer::GetTheme() const {
	return m_theme;
}

ImFont* Hexl::UI::UIRenderer::GetDefaultFont() const {
	return m_defaultFont;
}

ImFont* Hexl::UI::UIRenderer::GetMonospacedFont() const {
	return m_monospacedFont;
}
