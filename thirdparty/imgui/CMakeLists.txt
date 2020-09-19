set(IMGUI_HEADERS 
	include/imgui/imgui.h
	include/imgui/imconfig.h
	include/imgui/imgui_freetype.h
	include/imgui/imgui_impl_sdl.h
	include/imgui/imgui_impl_opengl3.h
	include/imgui/imgui_internal.h
	include/imgui/imstb_rectpack.h
	include/imgui/imstb_textedit.h
	include/imgui/imstb_truetype.h
)
set(IMGUI_SOURCES
	src/imgui.cpp
	src/imgui_demo.cpp
	src/imgui_draw.cpp
	src/imgui_freetype.cpp
	src/imgui_impl_opengl3.cpp
	src/imgui_impl_sdl.cpp
	src/imgui_widgets.cpp
)

if(TARGET freetype)
	set(IMGUI_HEADERS ${IMGUI_HEADERS} include/imgui/imgui_freetype.h)
	set(IMGUI_SOURCES ${IMGUI_SOURCES} src/imgui_freetype.cpp)
endif()

add_library(imgui STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_include_directories(imgui 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	       ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)
target_link_libraries(imgui PUBLIC freetype GLEW::GLEW SDL2::SDL2)
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW)
