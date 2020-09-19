#include "Hexl/UI/Theme.hpp"
#include "imgui.h"
#include <Hexl/UI/StyleEditorWindow.hpp>
#include <cmath>

Hexl::UI::StyleEditorWindow::StyleEditorWindow(Hexl::UI::Theme& theme) :
	Window("_styleEditor", "Style Editor"),
	m_showWindow(true),
	m_theme(theme),
	m_lastChange(0)

{
	SetIcon("\ue22b");
}

Hexl::UI::StyleEditorWindow::~StyleEditorWindow() {
}
// [Internal] Display details for a single font, called by ShowStyleEditor().
static void NodeFont(ImFont* font)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    bool font_details_opened = ImGui::TreeNode(font, "Font: \"%s\"\n%.2f px, %d glyphs, %d file(s)",
        font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size, font->ConfigDataCount);
    ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) { io.FontDefault = font; }
    if (!font_details_opened)
        return;

    ImGui::PushFont(font);
    ImGui::Text("The quick brown fox jumps over the lazy dog");
    ImGui::PopFont();
    ImGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
//    ImGui::SameLine(); HelpMarker(
//        "Note than the default embedded font is NOT meant to be scaled.\n\n"
//        "Font are currently rendered into bitmaps at a given size at the time of building the atlas. "
//        "You may oversample them to get some flexibility with scaling. "
//        "You can also render at multiple sizes and select which one to use at runtime.\n\n"
//        "(Glimmer of hope: the atlas system will be rewritten in the future to make scaling more flexible.)");
    ImGui::InputFloat("Font offset", &font->DisplayOffset.y, 1, 1, "%.0f");
    ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
    ImGui::Text("Fallback character: '%c' (U+%04X)", font->FallbackChar, font->FallbackChar);
    ImGui::Text("Ellipsis character: '%c' (U+%04X)", font->EllipsisChar, font->EllipsisChar);
    const int surface_sqrt = (int)sqrtf((float)font->MetricsTotalSurface);
    ImGui::Text("Texture Area: about %d px ~%dx%d px", font->MetricsTotalSurface, surface_sqrt, surface_sqrt);
    for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
        if (font->ConfigData)
            if (const ImFontConfig* cfg = &font->ConfigData[config_i])
                ImGui::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d",
                    config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
    if (ImGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
    {
        // Display all glyphs of the fonts in separate pages of 256 characters
        const ImU32 glyph_col = ImGui::GetColorU32(ImGuiCol_Text);
        for (unsigned int base = 0; base <= IM_UNICODE_CODEPOINT_MAX; base += 256)
        {
            // Skip ahead if a large bunch of glyphs are not present in the font (test in chunks of 4k)
            // This is only a small optimization to reduce the number of iterations when IM_UNICODE_MAX_CODEPOINT
            // is large // (if ImWchar==ImWchar32 we will do at least about 272 queries here)
            if (!(base & 4095) && font->IsGlyphRangeUnused(base, base + 4095))
            {
                base += 4096 - 256;
                continue;
            }

            int count = 0;
            for (unsigned int n = 0; n < 256; n++)
                if (font->FindGlyphNoFallback((ImWchar)(base + n)))
                    count++;
            if (count <= 0)
                continue;
            if (!ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
                continue;
            float cell_size = font->FontSize * 1;
            float cell_spacing = style.ItemSpacing.y;
            ImVec2 base_pos = ImGui::GetCursorScreenPos();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            for (unsigned int n = 0; n < 256; n++)
            {
                // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions
                // available here and thus cannot easily generate a zero-terminated UTF-8 encoded string.
                ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
                ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
                const ImFontGlyph* glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
                draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                if (glyph)
                    font->RenderChar(draw_list, cell_size, cell_p1, glyph_col, (ImWchar)(base + n));
                if (glyph && ImGui::IsMouseHoveringRect(cell_p1, cell_p2))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Codepoint: U+%04X", base + n);
                    ImGui::Separator();
                    ImGui::Text("Visible: %d", glyph->Visible);
                    ImGui::Text("AdvanceX: %.1f", glyph->AdvanceX);
                    ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                    ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                    ImGui::EndTooltip();
                }
            }
            ImGui::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    ImGui::TreePop();
}

void Hexl::UI::StyleEditorWindow::Draw(UIRenderer &renderer) {
	auto& io = ImGui::GetIO();
	auto& style = ImGui::GetStyle();
	int i;

	if(!ImGui::Begin(GetLabel().c_str(), &m_showWindow)) {
		ImGui::End();
		return;
	}


	ImGui::ShowFontSelector("Fonts");

	i = 0;
	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {
	        if (ImGui::BeginTabItem("Colors"))
	        {
			for(auto& color : m_theme.m_colors) {
				ImGui::PushID(i);
				auto res = ImGui::ColorEdit4("##color", color.second.value);
                		ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                		ImGui::TextUnformatted(color.first.c_str());

				if(res) {
					m_theme.SetColor(color.first, color.second.value[0], color.second.value[1], color.second.value[2], color.second.value[3]);
				}
				ImGui::PopID();
				i++;
			}
	        	ImGui::EndTabItem();

	        }
		if(ImGui::BeginTabItem("Properties")) {
			for(auto& flag: m_theme.m_flags) {
				bool old = flag.second.value;
				ImGui::PushID(i);
				ImGui::Checkbox(flag.first.c_str(), &flag.second.value);
				ImGui::PopID();
				i++;
				if(flag.second.value != old) {
					m_theme.SetFlag(flag.first, flag.second.value);
				}
			}
			ImGui::EndTabItem();

		}
		if(ImGui::BeginTabItem("Fonts")) {
			for(auto font : io.Fonts->Fonts) {
				NodeFont(font);

			}
			ImGui::EndTabItem();
		}
		if(ImGui::BeginTabItem("Classic Editor")) {
			ImGui::ShowStyleEditor();
			ImGui::EndTabItem();
		}
		if(ImGui::BeginTabItem("Export")) {
			static Hexl::String currentSettings;
			if(m_lastChange < renderer.GetTheme().GetNumChanges())
				currentSettings = renderer.GetTheme().ExportCurrentSettings();
			ImGui::Text("%s", currentSettings.c_str());
			ImGui::EndTabItem();
		}
        	ImGui::EndTabBar();
	}
	ImGui::End();
}
