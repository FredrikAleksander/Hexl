#pragma once
#include "Hexl/UI/Theme.hpp"
#ifndef __HEXL_UI__STYLEEDITORWINDOW_HPP
#define __HEXL_UI__STYLEEDITORWINDOW_HPP 1

#include <Hexl/UI/Window.hpp>

namespace Hexl {
namespace UI {

class StyleEditorWindow : public Window {
	private:
		bool             m_showWindow;
		Hexl::UI::Theme& m_theme;
		size_t           m_lastChange;
	public:
		StyleEditorWindow(Hexl::UI::Theme& theme);
		virtual ~StyleEditorWindow();

		virtual void Draw(UIRenderer& renderer) override;
};

}
}

#endif
