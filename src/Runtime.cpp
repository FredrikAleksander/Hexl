#include <Hexl/Log.hpp>
#include <Hexl/Runtime.hpp>
#include <Hexl/Graphics/RendererOpenGL3.hpp>
#include <Hexl/UI/LogWindow.hpp>
#include <Hexl/UI/StyleEditorWindow.hpp>
#include <api/runtime-api.hpp>
#include <SDL_filesystem.h>
#include <sstream>
#include <errhandlingapi.h>
#include <memory>
#include <processthreadsapi.h>

#include <Hexl/Scheme/SchemeEnvironment.hpp>
#include <Hexl/String.hpp>
#include <Hexl/UI/TextEditor.hpp>
#include <Hexl/UI/Console.hpp>
#include "Hexl/UI/SchemeConsoleEnvironment.hpp"
#include "imgui.h"
#include "imgui/imgui.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <filesystem>
#include <sstream>
#include <regex>
#ifdef WIN32
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

#ifndef APPLICATION_NAME
#define APPLICATION_NAME "hexl"
#endif

Hexl::Runtime::Runtime(PropertyMap&& properties, std::unique_ptr<Hexl::Scheme::SchemeEnvironment>&& schemeEnvironment, std::unique_ptr<Hexl::Graphics::Renderer>&& renderer) :
	m_properties(std::move(properties)),
	m_schemeEnvironment(std::move(schemeEnvironment)),
	m_renderer(std::move(renderer)),
	m_uiWindows(),
	m_runtimePathAutoloaded(0),
	m_exitRequested(false),
	m_exitRequestedStatus(ExitStatus::OK)
{
	m_uiWindows.emplace_back(std::make_unique<UI::LogWindow>());
	m_uiWindows.emplace_back(std::make_unique<UI::TextEditor>(m_schemeEnvironment.get()));
	m_uiWindows.emplace_back(std::make_unique<UI::StyleEditorWindow>(m_renderer->GetUIRenderer().GetTheme()));
	auto console = std::make_unique<UI::Console>();
	auto consolePtr = console.get();
	m_uiWindows.emplace_back(std::move(console));

	m_schemeConsoleEnvironment = std::make_shared<UI::SchemeConsoleEnvironment>(*m_schemeEnvironment);
	consolePtr->AddEnvironment(m_schemeConsoleEnvironment);
	
	auto& theme = m_renderer->GetUIRenderer().GetTheme();
	for(auto& uiWindow : m_uiWindows) {
		uiWindow->RegisterThemeProperties(theme);
	}
}

Hexl::Runtime::~Runtime() {
	m_uiWindows.clear();
	m_renderer = nullptr;
	m_schemeEnvironment = nullptr;
	LOG_VERBOSE(LogCategory::RUNTIME, "%s", "Runtime shutting down");
	SDL_Quit();
}

void Hexl::Runtime::EnumFileProperties(Hexl::PropertyMap& properties) {
	LOG_VERBOSE(LogCategory::RUNTIME, "%s", "Enumerating properties from properties file");
}

#ifndef WIN32
extern char **environ;
#endif

static void ParseProperty(Hexl::PropertyMap& properties, const Hexl::String& prop) {
	if(strncmp("HEXL_", prop.c_str(), sizeof("HEXL_")-1) == 0) {
		auto i = prop.find("=");
		if(i == prop.npos) {
		}
		Hexl::String x = Hexl::String(prop.c_str() + sizeof("HEXL_")-1, prop.c_str() + i);
		Hexl::String v = Hexl::String(prop.c_str() + i + 1, prop.c_str() + prop.length());
		properties[x] = v;
	}
}

void Hexl::Runtime::EnumEnvVarProperties(Hexl::PropertyMap& properties) {
	LOG_VERBOSE(LogCategory::RUNTIME, "%s", "Enumerating properties from environment variables");
#ifdef WIN32
	std::vector<char> buffer;
	auto envVars = GetEnvironmentStringsA();
	for(auto p = envVars; *p; p++) {
		while(*p)
			buffer.push_back(*p++);
		ParseProperty(properties, buffer.data());
		memset(buffer.data(), 0, buffer.size());
		buffer.clear();
	}
#else
	char** env = environ;
	while(*env != nullptr) {
		ParseProperty(properties, *env);
		++env;
	}
#endif
}

static void CreateDirectoryIfNotExists(const Hexl::String& dir) {
	std::filesystem::path p = std::filesystem::u8path(dir);

	if(!std::filesystem::exists(p)) {
		std::filesystem::create_directories(p);
	}
}


static Hexl::String runtimeAppFolder = "";
static bool runtimeAppFolderInitialized = false;

static void s_initializeRuntimePath() {
#ifndef WIN32
		std::filesystem::path p;
		if(strlen(getenv("XDG_CONFIG_HOME")) > 0) {
			p = std::filesystem::u8path(getenv("XDG_CONFIG_HOME")) / APPLICATION_NAME;
			if(!std::filesystem::exists(p)) {
				std::filesystem::create_directories(p);
				runtimeAppFolder = p.normalize();
				runtimeAppFolderInitialized = true;
				return;
			}
		} 
		if(strlen(getenv("HOME")) > 0) {
			p = std::filesystem::u8path(getenv("HOME")) / ".config" / APPLICATION_NAME;
			if(!std::filesystem::exists(p)) {
				std::filesystem::create_directories(p);
				runtimeAppFolder = p.normalize();
				runtimeAppFolderInitialized = true;
				return;
			}
		}
#endif
		runtimeAppFolder = SDL_GetPrefPath(nullptr, "hexl");

}

static Hexl::String& s_getRuntimePrefPath() {
	if(!runtimeAppFolderInitialized) {
		s_initializeRuntimePath();
	}
	return runtimeAppFolder;
}

std::unique_ptr<Hexl::Runtime> Hexl::Runtime::Create(int argc, const char **argv)
{
	s_initializeRuntimePath();
	Hexl::_internal::initLog();

	PropertyMap properties;
	EnumFileProperties(properties);
	EnumEnvVarProperties(properties);
	LOG_VERBOSE(LogCategory::RUNTIME, "%s", "Enumerating properties from command line");
	{
		char bufk[256];
		char bufv[256];
		for(int i = 1; i < argc; i++) {
			int r = sscanf(argv[i], "--%[^= ]=%s", bufk, bufv);
			if(r == 2) {
				properties[bufk] = bufv;
			}
			else if(r == 1) {
				properties[bufk] = "true";
			}
		}

	}
	for(auto& prop : properties) {
		LOG_INFO(LogCategory::RUNTIME, "Property: %s=%s", prop.first.c_str(), prop.second.c_str());
	}

	auto schemeEnvironment = Hexl::Scheme::SchemeEnvironment::Create(properties);
	if(!schemeEnvironment) {
		LOG_FATAL(LogCategory::RUNTIME, "%s", "Failed to initialize Scheme environment");
		return std::unique_ptr<Hexl::Runtime>();
	}

	LOG_VERBOSE(LogCategory::RUNTIME, "%s", "Initializing SDL");
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		LOG_FATAL(LogCategory::RUNTIME, "%s", SDL_GetError());
		return std::unique_ptr<Hexl::Runtime>();
	}
	
	auto renderer = Graphics::RendererOpenGL3::Create(properties);
	if(!renderer) {
		LOG_FATAL(LogCategory::RUNTIME, "%s", "Failed to create renderer");
		SDL_Quit();
		return std::unique_ptr<Hexl::Runtime>();
	}

	Hexl::Runtime* ptr = new Hexl::Runtime(std::move(properties), std::move(schemeEnvironment), std::move(renderer));
	hexl_api_init(ptr);
	ptr->AddRuntimePath(SDL_GetBasePath());
	if(!ptr->Initialize()) {
		delete ptr;
		return std::unique_ptr<Hexl::Runtime>();
	}
	return std::unique_ptr<Hexl::Runtime>(ptr);
}

bool Hexl::Runtime::Update() {
        SDL_Event event;

	for(; m_runtimePathAutoloaded < m_runtimePath.size(); m_runtimePathAutoloaded++) {
		// Autoload any newly added runtime paths
		Autoload(m_runtimePath[m_runtimePathAutoloaded]);
	}

	Hexl::_internal::flushLog();

	while(!m_callbacks.empty()) {
		m_callbacks.front()(*this);
		m_callbacks.pop();
	}

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
		Exit(ExitStatus::OK);
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(m_renderer->GetWindow()))
		Exit(ExitStatus::OK);
        }


	// Render debug UI
	auto& ui = m_renderer->GetUIRenderer();
	ui.Begin();
	for(auto& window : m_uiWindows) {
		if(window)
			window->Draw(ui);
	}
	ui.End();
	ui.Draw();

	return m_exitRequested;
}

void Hexl::Runtime::AddSingleRuntimePath(const Hexl::String &runtimePath) {
	m_runtimePath.push_back(runtimePath);
	LOG_VERBOSE(LogCategory::RUNTIME, "Added runtime path: %s", runtimePath.c_str());
}

void Hexl::Runtime::AddRuntimePath(const Hexl::String &runtimePath) {
	static const Hexl::String delim = ";";

	if(runtimePath.empty())
		return;

	Hexl::String path;
	auto start = 0U;
	auto end = runtimePath.find(delim);

	while(end != Hexl::String::npos) {
		AddSingleRuntimePath(runtimePath.substr(start, end - start));
		start = end + delim.length();
		end = runtimePath.find(delim, start);
	}
	AddSingleRuntimePath(runtimePath.substr(start, end));
}

static bool IsRuntimeFile(Hexl::String& buffer, const Hexl::String& rtp, const std::filesystem::path& path) {
	std::filesystem::path finalPath = std::filesystem::u8path(rtp.c_str());
	finalPath
		.make_preferred() /= path;

	if(std::filesystem::exists(finalPath)) {
		buffer = finalPath.u8string();
		return true;
	}

	return false;
}

bool Hexl::Runtime::ResolveRuntimePath(const Hexl::String &path, Hexl::String &result) {
	std::filesystem::path p = std::filesystem::u8path(path.c_str());
	p.make_preferred();
	if(p.is_absolute()) {
		bool r = std::filesystem::exists(p);
		if(r) {
			result = p.u8string();
			return true;
		}
		result = "";
		return false;
	}
	for(auto i = m_runtimePath.rbegin(); i != m_runtimePath.rend(); ++i) {
		if(IsRuntimeFile(result, *i, p)) {
			return true;
		}
	}
	result = "";
	return false;
}

bool Hexl::Runtime::Initialize() {
	// TODO: Run `${RUNTIME_PATH}/runtime.scm` to initialize the runtime
	Hexl::String bootstrapScriptPath;
	if(ResolveRuntimePath("runtime.scm", bootstrapScriptPath)) {
		if(!m_schemeEnvironment->Load(bootstrapScriptPath)) {
			LOG_ERROR(LogCategory::RUNTIME, "Error executing bootstrap script: %s", bootstrapScriptPath.c_str());
		}
	}
	else {
		LOG_ERROR(LogCategory::RUNTIME, "%s", "Could not find bootstrap script: runtime.scm");
	}

	return true;
}

void Hexl::Runtime::Autoload(const Hexl::String& runtimePath) {
	// TODO: Use custom autoload instead of load (requires actually maing the autoload first)
	std::filesystem::path autoloadScheme = std::filesystem::u8path(runtimePath) / std::filesystem::u8path("autoload.scm");
	LOG_VERBOSE(LogCategory::RUNTIME, "Checking autoload for runtime path: %s", runtimePath.c_str());
	if(std::filesystem::exists(autoloadScheme)) {
		auto schemePath = autoloadScheme.u8string();
		LOG_VERBOSE(LogCategory::RUNTIME, "Running autoload for runtime path: %s", runtimePath.c_str());
		if(!m_schemeEnvironment->Load(schemePath.c_str())) {
			LOG_ERROR(LogCategory::RUNTIME, "Autoload failed for runtime path: %s", runtimePath.c_str());
		}
	}
}

void Hexl::Runtime::Exit(ExitStatus status) {
	m_exitRequested = true;
	m_exitRequestedStatus = status;
}

bool Hexl::Runtime::ExitRequested() const {
	return m_exitRequested;
}

Hexl::Runtime::ExitStatus Hexl::Runtime::ExitRequestedStatus() const {
	return m_exitRequestedStatus;
}

Hexl::Graphics::Renderer& Hexl::Runtime::GetRenderer() {
	return *m_renderer;
}

Hexl::Scheme::SchemeEnvironment& Hexl::Runtime::GetSchemeEnvironment() {
	return *m_schemeEnvironment;
}

const Hexl::String& Hexl::Runtime::GetRuntimePrefPath() const {
	return s_getRuntimePrefPath();
}

void Hexl::Runtime::Dispatch(std::function<void(Runtime&)>&& callback) {
	m_callbacks.push(std::move(callback));
}

void Hexl::Runtime::SetTextEditorProvider(TextEditorProvider provider) {
	m_textEditorProvider = provider;
}

void Hexl::Runtime::SetCustomTextEditor(const Hexl::String& command) {
	m_customTextEditor = command;
}

static Hexl::String escapeString(const Hexl::String& input) {
	Hexl::String s = input;
	Hexl::ReplaceAll(s, "\"", "\\\"");
	s.insert(0, "\"");
	s.append("\"");
	return s;
}

void Hexl::Runtime::OpenTextEditor(const Hexl::String &path, int line) {
#ifndef EMSCRIPTEN
	Hexl::String command;
	if(m_textEditorProvider == TextEditorProvider::CUSTOM) {
		command = m_customTextEditor;
	}

	if(!command.empty()) {
		std::ostringstream oss;
		oss << (line < 0 ? 0 : line);
		ReplaceAll(command, "%p", escapeString(path));
		ReplaceAll(command, "%l", oss.str());
		// TODO: Spawn process using command
		if(SpawnAsynchronous(Hexl::String(command))) 
			return;
		//LOG_WARN(LogCategory::RUNTIME, "Not implemented process spawn. Cmdline: %s", command.c_str());
	}
#endif
	for(auto& uiWindow : m_uiWindows) {
		auto p = dynamic_cast<UI::TextEditor*>(uiWindow.get());
		if(p != nullptr) {
			p->OpenFile(path, line);
			return;
		}
	}
}

void Hexl::Runtime::AddFont(const char* path, float size, int options, const unsigned short* glyphRanges) {
	Dispatch([=](auto& rt) {
		rt.GetRenderer().GetUIRenderer().AddFont(path, size, options, glyphRanges);
	});
}


#ifndef EMSCRIPTEN
// Simple fire and forget spawn method. Useful when you do not really care about the result
bool Hexl::Runtime::SpawnAsynchronous(const Hexl::String &command_line) {
#ifdef WIN32
	WideString wcmdline = EncodeWideString("cmd.exe /c " + command_line);
	DWORD dwCreationFlags = 0;
#ifdef UNICODE
	dwCreationFlags = CREATE_UNICODE_ENVIRONMENT;
#endif
	STARTUPINFOW StartupInfo;
	PROCESS_INFORMATION ProcessInformation;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));
	auto res = CreateProcessW(
		nullptr,                    // lpApplicationName
		(wchar_t*)wcmdline.c_str(), // lpCommandLine
		nullptr,                    // lpProcessAttributes
		nullptr,                    // lpThreadAttributes
		FALSE,                      // bInheritHandle
		dwCreationFlags,            // dwCreationFlags
		nullptr,                    // lpEnvironment
		nullptr,                    // lpCurrentDirectory
		&StartupInfo,               // lpStartupInfo
		&ProcessInformation);       // lpProcessInformation
	if(res == 0) {
		DWORD err = GetLastError();
		LPWSTR errStr = nullptr;
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errStr, 0, nullptr);
		if(errStr == nullptr) {
			LOG_ERROR(LogCategory::RUNTIME, "Failed to spawn process `%s`: Error Code 0x%X", command_line.c_str(), err);
		}
		else {
			auto errStrUtf8 = DecodeWideString(errStr);
			LOG_ERROR(LogCategory::RUNTIME, "Failed to spawn process `%s`: %s", command_line.c_str(), errStrUtf8.c_str());
		}
		return false;
	}
	CloseHandle(ProcessInformation.hProcess);
	CloseHandle(ProcessInformation.hThread);
	return true;
#else
#endif
}
#endif
