#pragma once
#ifndef __HEXL_GRAPHICS__RENDEREROPENGL3_HPP
#define __HEXL_GRAPHICS__RENDEREROPENGL3_HPP 1

#include <Hexl/Graphics/Renderer.hpp>
#include <Hexl/PropertyMap.hpp>
#include "Hexl/UI/UIRenderer.hpp"
#include <SDL.h>
#include <memory>

namespace Hexl {
namespace Graphics {

class RendererOpenGL3 final: public Renderer {
	private:
		SDL_GLContext m_glContext;
		std::unique_ptr<UI::UIRenderer> m_uiRenderer;
		RendererOpenGL3(SDL_Window* window, SDL_GLContext glContext, std::unique_ptr<UI::UIRenderer>&& uiRenderer);
	public:
		virtual ~RendererOpenGL3();

		static std::unique_ptr<Renderer> Create(const PropertyMap& properties);

		virtual UI::UIRenderer& GetUIRenderer() override;
};

}
}

#endif
