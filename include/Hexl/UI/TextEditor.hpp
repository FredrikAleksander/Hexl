#pragma once
#ifndef __HEXL_UI__TEXTEDITOR_HPP
#define __HEXL_UI__TEXTEDITOR_HPP 1

#include <Hexl/UI/Window.hpp>
#include <Hexl/Scheme/SchemeEnvironment.hpp>

namespace Hexl {
namespace UI {

struct TextEditorContainer;
class TextEditor final: public Window {
	private:
		bool                 m_showWindow;
		TextEditorContainer* m_container;
	protected:
		virtual void RegisterThemeProperties(Theme& theme) override;
	public:
		TextEditor(Scheme::SchemeEnvironment* schemeEnvironment);
		virtual ~TextEditor();

		void OpenFile(const Hexl::String& path, int line = -1);
		virtual void Draw(UIRenderer& renderer) override;

};

}
}

#endif
