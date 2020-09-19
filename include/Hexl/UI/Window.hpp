#pragma once
#ifndef __HEXL_UI__WINDOW_HPP
#define __HEXL_UI__WINDOW_HPP 1

#include <Hexl/String.hpp>
#include <Hexl/UI/UIRenderer.hpp>
#include <Hexl/UI/Theme.hpp>

namespace Hexl {
	class Runtime;
	namespace UI {

class Window {
	friend class ::Hexl::Runtime;
	private:
		Hexl::String m_id;
		Hexl::String m_icon;
		Hexl::String m_name;
		Hexl::String m_label;
		bool         m_receiveFocus;
	protected:
		Window(const Hexl::String& id, const Hexl::String& name);

		virtual void RegisterThemeProperties(UI::Theme& theme);
		bool ReceiveFocus();
	public:
		Window(const Window&)                   = delete;
		Window(Window&&)                        = delete;

		Window& operator=(const Window&)        = delete;
		Window& operator=(Window&&)             = delete;

		virtual ~Window()                       = default;

		void Focus();

		void SetName(const Hexl::String& name);
		void SetIcon(const Hexl::String& icon);

		const Hexl::String& GetName() const;
		const Hexl::String& GetIcon() const;
		const Hexl::String& GetLabel() const;

		virtual void Draw(UIRenderer& renderer) = 0;
};

}
}

#endif
