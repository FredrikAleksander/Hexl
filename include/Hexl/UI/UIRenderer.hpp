#pragma once
#ifndef __HEXL_UI__UIRENDERER_HPP
#define __HEXL_UI__UIRENDERER_HPP 1

#include <Hexl/UI/Theme.hpp>
#include <imgui.h>

namespace Hexl {
namespace UI {

enum class FontOptions : int {
	FONT_NONE,
	FONT_POWERLINE,
	FONT_FONTAWESOME,
	FONT_MERGE_POWERLINE,
	FONT_MERGE_FONTAWESOME,
	FONT_NO_HINTING
};

class UIRenderer {
	protected:
		ImVector<ImWchar> m_glyphRanges;
		ImFont* m_defaultFont;
		ImFont* m_monospacedFont;
		Theme   m_theme;
	protected:
		UIRenderer();
	public:
		UIRenderer(const UIRenderer&)            = delete;
		UIRenderer(UIRenderer&&)                 = delete;
		UIRenderer& operator=(const UIRenderer&) = delete;
		UIRenderer& operator=(UIRenderer&&)      = delete;
		virtual ~UIRenderer();

		virtual void LoadStyle();
		virtual void LoadFonts();

		virtual void Begin() = 0;
		virtual void End()   = 0;
		virtual void Draw()  = 0;

		Theme& GetTheme();
		const Theme& GetTheme() const;

		ImFont* AddFont(const char* path, float size, int options, const ImWchar* glyphRanges);
		virtual void RebuildFontAtlas() = 0;
		
		ImFont* GetDefaultFont() const;
		ImFont* GetMonospacedFont() const;
};

}
}

#endif
