#include <Hexl/Graphics/Renderer.hpp>
#include <Hexl/Log.hpp>

Hexl::Graphics::Renderer::Renderer(SDL_Window* window) :
	m_window(window)
{
}

Hexl::Graphics::Renderer::~Renderer() {
	LOG_VERBOSE(LogCategory::RENDERER, "%s", "Renderer shutting down");
	if(m_window) {
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}
}

SDL_Window* Hexl::Graphics::Renderer::GetWindow() const {
	return m_window;
}
