#pragma once
#ifndef __HEXL__RUNTIME_HPP
#define __HEXL__RUNTIME_HPP 1

#include <Hexl/PropertyMap.hpp>
#include <Hexl/Graphics/Renderer.hpp>
#include <Hexl/UI/Window.hpp>
#include <Hexl/Scheme/SchemeEnvironment.hpp>
#include <Hexl/UI/SchemeConsoleEnvironment.hpp>
#include <SDL.h>
#include <memory>
#include <string>
#include <vector>
#include <queue>

namespace Hexl {

class Runtime {
	public:
		enum class ExitStatus {
			OK = 0,
			UNKNOWN_ERROR
		};
		enum class TextEditorProvider {
			RUNTIME = 0, // Use builtin editor
			CUSTOM       // Let user provide what editor to use
		};
	private:
		PropertyMap                                 m_properties;
		std::unique_ptr<Hexl::Graphics::Renderer>   m_renderer;
		std::vector< std::unique_ptr<UI::Window> >  m_uiWindows;
		std::unique_ptr<Scheme::SchemeEnvironment>  m_schemeEnvironment;
		std::vector< Hexl::String >                 m_runtimePath;
		decltype(m_runtimePath.size())              m_runtimePathAutoloaded;
		bool                                        m_exitRequested;
		ExitStatus                                  m_exitRequestedStatus;
		std::queue< std::function<void(Runtime&)> > m_callbacks;
		TextEditorProvider                          m_textEditorProvider;
		Hexl::String                                m_customTextEditor;
		std::shared_ptr<UI::SchemeConsoleEnvironment> m_schemeConsoleEnvironment;

		static void EnumFileProperties(PropertyMap& properties);
		static void EnumEnvVarProperties(PropertyMap& properties);

		void AddSingleRuntimePath(const Hexl::String& runtimePath);

		void Dispatch(std::function<void(Runtime&)>&& callback);
	protected:
		Runtime(PropertyMap&& properties, std::unique_ptr<Hexl::Scheme::SchemeEnvironment>&& schemeEnvironment, std::unique_ptr<Hexl::Graphics::Renderer>&& renderer);
		virtual bool Initialize();
		virtual void Autoload(const Hexl::String& runtimePath);
	public:
		virtual ~Runtime();
		static std::unique_ptr<Runtime> Create(int argc, const char* argv[]);

		// API
		Hexl::Graphics::Renderer& GetRenderer();
		Hexl::Scheme::SchemeEnvironment& GetSchemeEnvironment();

		void SetTextEditorProvider(TextEditorProvider provider);
		void SetCustomTextEditor(const Hexl::String& command);

		/*!
		 * Open the specified text file in the configured text editor, either the internal
		 * one, or a external one, depending on settings.
		 */
		void OpenTextEditor(const Hexl::String& path, int line=-1);
		const Hexl::String& GetRuntimePrefPath() const;
		void AddRuntimePath(const Hexl::String& runtimePath);
		bool ResolveRuntimePath(const Hexl::String& path, Hexl::String& result);
		void Exit(ExitStatus status);
		bool ExitRequested() const;
		ExitStatus ExitRequestedStatus() const;

		void AddFont(const char* path, float size, int options, const unsigned short* glyphRanges);


#ifndef EMSCRIPTEN
		virtual bool SpawnAsynchronous(const Hexl::String& command_line);
#endif

		virtual bool Update();
};

}

#endif
