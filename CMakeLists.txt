project(Hexl)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(freetype)

if(TARGET freetype)
	set(HEXL_USE_FREETYPE TRUE)
	set(FREETYPE_TARGETS freetype)
endif()

add_subdirectory(thirdparty/zep)
add_subdirectory(thirdparty/imgui)

#include(src/zep/CMakeLists.txt)

add_custom_command(OUTPUT src/bindings.c
	           DEPENDS src/bindings.stub
		   COMMAND chibi-ffi ${CMAKE_CURRENT_SOURCE_DIR}/src/bindings.stub ${CMAKE_CURRENT_BINARY_DIR}/src/bindings.c
)

set(HEXL_HEADERS ${HEXL_HEADERS}
	include/Hexl/Runtime.hpp
	include/Hexl/Log.hpp

	include/Hexl/Graphics/Renderer.hpp
	include/Hexl/Graphics/RendererOpenGL3.hpp

	include/Hexl/UI/UIRenderer.hpp
	include/Hexl/UI/UIRendererOpenGL3.hpp
	include/Hexl/UI/Window.hpp
	include/Hexl/UI/LogWindow.hpp
	include/Hexl/UI/StyleEditorWindow.hpp
	include/Hexl/UI/TextEditor.hpp
	include/Hexl/UI/Theme.hpp
	include/Hexl/UI/Console.hpp
	include/Hexl/UI/SchemeConsoleEnvironment.hpp

	include/Hexl/Scheme/SchemeEnvironment.hpp

	src/api/runtime-api.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/config.h
)
set(HEXL_SOURCES ${HEXL_SOURCES}
	src/Runtime.cpp
	src/Log.cpp
	src/String.cpp

	src/Graphics/Renderer.cpp
	src/Graphics/RendererOpenGL3.cpp

	src/UI/UIRenderer.cpp
	src/UI/UIRendererOpenGL3.cpp
	src/UI/Window.cpp
	src/UI/LogWindow.cpp
	src/UI/StyleEditorWindow.cpp
	src/UI/TextEditorWindow.cpp
	src/UI/Console.cpp
	src/UI/Theme.cpp
	src/UI/SchemeConsoleEnvironment.cpp

	src/Scheme/SchemeEnvironment.cpp

	src/api/runtime-api.cpp
	${CMAKE_CURRENT_BINARY_DIR}/src/bindings.c

	src/main.cpp
)


if(MINGW)
	set(MINGW32_LIBRARY mingw32 "-mwindows" CACHE STRING "link flags for MinGW")
	set(SDL_TARGETS mingw32 SDL2::SDL2main SDL2::SDL2)
else()
	set(SDL_TARGETS SDL2::SDL2)
endif()

configure_file(src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)


add_executable(Hexl ${HEXL_HEADERS} ${HEXL_SOURCES})
target_include_directories(Hexl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src)
target_link_libraries(Hexl ${SDL_TARGETS} GLEW::GLEW OpenGL::GL ${FREETYPE_TARGETS} Threads::Threads winpthread /home/fredrik/.mingw64/lib/libchibi-scheme.a ws2_32.dll zep imgui)
set_source_files_properties(src/bindings.c PROPERTIES COMPILE_DEFINITIONS sexp_init_library=sexp_init_runtime_library) # Hacky way to rename sexp_init_library to sexp_init_runtime_library
