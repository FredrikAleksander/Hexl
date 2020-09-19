#pragma once
#ifndef __HEXL_GRAPHICS__RENDERER_HPP
#define __HEXL_GRAPHICS__RENDERER_HPP 1

#include <Hexl/UI/UIRenderer.hpp>
#include <SDL.h>

namespace Hexl {
namespace Graphics {

class Renderer {
	protected:
		SDL_Window* m_window;

		Renderer(SDL_Window* window);
	public:
		virtual ~Renderer();
		Renderer(const Renderer&)            = delete;
		Renderer(Renderer&&)                 = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&)      = delete;

		SDL_Window* GetWindow() const;
		virtual UI::UIRenderer& GetUIRenderer() = 0;
};

}
}

#endif
