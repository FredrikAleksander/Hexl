#pragma once
#ifndef __HEXL_API__RUNTIME_API_HPP
#define __HEXL_API__RUNTIME_API_HPP 1

#ifdef __cplusplus
extern "C" {
#endif
	void hexl_api_init(void*);

	int  hexl_api_get_runtime_pref_path(char* buf, int buf_size); 
	void hexl_api_add_runtime_path(const char* path);
	int  hexl_api_resolve_runtime_path(const char* to_resolve, char* buf, int buf_size);
	void hexl_api_exit(int exit_code);

	void hexl_api_use_text_editor(const char* command);
	void hexl_api_open_text_editor(const char* path, int line);

	int  hexl_api_add_font(const char* path, float size, int options, const unsigned short* glyph_ranges);
	void hexl_api_set_theme_flag(const char* name, int enabled);
	void hexl_api_set_theme_color(const char* name, const float color[4]);
	void hexl_api_set_theme_scalar(const char* name, float scalar);
	void hexl_api_set_theme_vec2(const char* name, const float vec[2]);
	void hexl_api_set_theme_vec3(const char* name, const float vec[3]);
	void hexl_api_set_theme_vec4(const char* name, const float vec[4]);
#ifdef __cplusplus
}
#endif

#endif
