#include <Hexl/Graphics/RendererOpenGL3.hpp>
#include <Hexl/UI/UIRendererOpenGL3.hpp>
#include <Hexl/Log.hpp>
#include <SDL.h>
#include <GL/glew.h>

Hexl::Graphics::RendererOpenGL3::RendererOpenGL3(SDL_Window* window, SDL_GLContext glContext, std::unique_ptr<UI::UIRenderer>&& uiRenderer) :
	Renderer(window),
	m_glContext(glContext),
	m_uiRenderer(std::move(uiRenderer))
{
}

Hexl::Graphics::RendererOpenGL3::~RendererOpenGL3() {
	SDL_GL_DeleteContext(m_glContext);
}

Hexl::UI::UIRenderer& Hexl::Graphics::RendererOpenGL3::GetUIRenderer() {
	return *m_uiRenderer;
}

std::unique_ptr<Hexl::Graphics::Renderer> Hexl::Graphics::RendererOpenGL3::Create(const PropertyMap &properties) {
	LOG_VERBOSE(LogCategory::RENDERER, "%s", "Creating OpenGL 3.0 renderer");
	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
	LOG_VERBOSE(LogCategory::RENDERER, "%s", "Creating renderer window");
	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("Hexl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	LOG_VERBOSE(LogCategory::RENDERER, "%s", "Loading OpenGL extensions");
	// Initialize OpenGL loader
	bool err = glewInit() != GLEW_OK;
	if (err)
	{
		LOG_FATAL(LogCategory::RENDERER, "%s", "Failed to initialize OpenGL loader!");
		return std::unique_ptr<Hexl::Graphics::Renderer>();
	}

	std::unique_ptr<UI::UIRenderer> uiRenderer = UI::UIRendererOpenGL3::Create(properties, window, gl_context, glsl_version);
	return std::unique_ptr<Hexl::Graphics::Renderer>(new RendererOpenGL3(window, gl_context, std::move(uiRenderer)));
}
