#pragma once
#ifndef __HEXL_UI__UIRENDEREROPENGL3_HPP
#define __HEXL_UI__UIRENDEREROPENGL3_HPP

#include <Hexl/UI/UIRenderer.hpp>
#include <Hexl/PropertyMap.hpp>
#include <SDL.h>
#include <memory>

namespace Hexl {
namespace UI {

class UIRendererOpenGL3 final: public UIRenderer {
	private:
		SDL_Window* m_window;
		SDL_GLContext m_glContext;
		float m_clearColor[4];
		UIRendererOpenGL3(SDL_Window* window, SDL_GLContext glContext);
	public:
		virtual ~UIRendererOpenGL3();

		static std::unique_ptr<UIRenderer> Create(const PropertyMap& properties, SDL_Window* window, SDL_GLContext glContext, const char* glsl_version);

		virtual void RebuildFontAtlas() override;

		virtual void Begin() override;
		virtual void End() override;
		virtual void Draw() override;
};

}
}

#endif
