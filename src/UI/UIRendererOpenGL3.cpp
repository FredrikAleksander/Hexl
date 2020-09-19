#include "config.h"
#include <Hexl/Log.hpp>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imgui.h"
#include <Hexl/UI/UIRendererOpenGL3.hpp>
#include <imgui.h>
#ifdef HEXL_USE_FREETYPE
#include <imgui_freetype.h>
#endif
#include <vector>
#include <GL/glew.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

Hexl::UI::UIRendererOpenGL3::UIRendererOpenGL3(SDL_Window* window, SDL_GLContext glContext) :
	m_window(window),
	m_glContext(glContext)
{
	m_clearColor[0] = 0.0f;
	m_clearColor[1] = 0.0f;
	m_clearColor[2] = 1.0f;
	m_clearColor[3] = 1.0f;
}

Hexl::UI::UIRendererOpenGL3::~UIRendererOpenGL3()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

std::unique_ptr<Hexl::UI::UIRenderer> Hexl::UI::UIRendererOpenGL3::Create(const PropertyMap &properties, SDL_Window *window, SDL_GLContext glContext, const char* glsl_version) {
	LOG_VERBOSE(LogCategory::UI, "%s", "Creating UI context");
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	
	std::unique_ptr<Hexl::UI::UIRenderer> uiRenderer = std::unique_ptr<Hexl::UI::UIRenderer>(new Hexl::UI::UIRendererOpenGL3(window, glContext));

	uiRenderer->LoadStyle();
	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init(glsl_version);
	uiRenderer->LoadFonts();
	
	return uiRenderer;
}

void Hexl::UI::UIRendererOpenGL3::RebuildFontAtlas() {
	auto& io = ImGui::GetIO();
	unsigned int flags = ImGuiFreeType::NoHinting;
	ImGui_ImplOpenGL3_DestroyFontsTexture();
#ifdef HEXL_USE_FREETYPE
	ImGuiFreeType::BuildFontAtlas(io.Fonts, flags);
#else
	io.Fonts->Build();
#endif
	ImGui_ImplOpenGL3_CreateFontsTexture();
}

void Hexl::UI::UIRendererOpenGL3::Begin() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window);
	ImGui::NewFrame();
}

void Hexl::UI::UIRendererOpenGL3::End() {
	ImGui::Render();
}

void Hexl::UI::UIRendererOpenGL3::Draw() {
	auto& io = ImGui::GetIO();

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        SDL_GL_SwapWindow(m_window);

}
