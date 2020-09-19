#include <api/runtime-api.hpp>
#include <Hexl/String.hpp>
#include <Hexl/Runtime.hpp>

Hexl::Runtime* g_runtime = nullptr;

void hexl_api_init(void* runtime) {
	g_runtime = (Hexl::Runtime*)runtime;
}

int hexl_api_get_runtime_pref_path(char *buf, int buf_size) {
	assert(buf != nullptr);
	auto runtimePath = g_runtime->GetRuntimePrefPath();
	if(runtimePath.length() >= buf_size) {
		return 1;
	}
	memcpy(buf, runtimePath.c_str(), runtimePath.length()+1);
	return 0;
}

void hexl_api_add_runtime_path(const char* path) {
	g_runtime->AddRuntimePath(path);
}

int hexl_api_resolve_runtime_path(const char* to_resolve, char *buf, int buf_size) {
	Hexl::String tmp;
	assert(buf != nullptr);
	if(g_runtime->ResolveRuntimePath(to_resolve, tmp)) {
		if(tmp.length() >= buf_size) {
			return tmp.length()+1;
		}
		memcpy(buf, tmp.c_str(), tmp.length()+1);
		return 0;
	}
	if(buf_size > 0) {
		buf[0] = '\0';
	}
	return 0;
}

void hexl_api_exit(int exit_code) {
	Hexl::Runtime::ExitStatus status = Hexl::Runtime::ExitStatus::UNKNOWN_ERROR;
	switch(exit_code) {
		case 0:
			status = Hexl::Runtime::ExitStatus::OK;
			break;
	}

	g_runtime->Exit(status);
}

void hexl_api_use_text_editor(const char *path) {
	Hexl::String str(path == nullptr ? "" : path);
	Hexl::Trim(str);
	if(str.empty()) {
		g_runtime->SetTextEditorProvider(Hexl::Runtime::TextEditorProvider::RUNTIME);
		g_runtime->SetCustomTextEditor("");
	}
	else {
		g_runtime->SetTextEditorProvider(Hexl::Runtime::TextEditorProvider::CUSTOM);
		g_runtime->SetCustomTextEditor(path);
	}
}

void hexl_api_open_text_editor(const char *path, int line) {
	g_runtime->OpenTextEditor(path, line);
}

int hexl_api_add_font(const char *path, float size, int options, const unsigned short *glyph_ranges) {
	g_runtime->AddFont(path, size, options, glyph_ranges);
	return 0;
}

void hexl_api_set_theme_color(const char *name, const float *color) {
	g_runtime->GetRenderer().GetUIRenderer().GetTheme().SetColor(name, color[0], color[1], color[2], color[3]);
}

void hexl_api_set_theme_flag(const char *name, int enabled) {
	g_runtime->GetRenderer().GetUIRenderer().GetTheme().SetFlag(name, enabled ? true : false);
}

void hexl_api_set_theme_scalar(const char *name, float scalar) {
	g_runtime->GetRenderer().GetUIRenderer().GetTheme().SetScalar(name, scalar);
}

void hexl_api_set_theme_vec2(const char *name, const float *vec) {
	g_runtime->GetRenderer().GetUIRenderer().GetTheme().SetVec2(name, vec[0], vec[1]);
}

void hexl_api_set_theme_vec3(const char *name, const float *vec) {
	g_runtime->GetRenderer().GetUIRenderer().GetTheme().SetVec3(name, vec[0], vec[1], vec[2]);
}

void hexl_api_set_theme_vec4(const char *name, const float *vec) {
	g_runtime->GetRenderer().GetUIRenderer().GetTheme().SetVec4(name, vec[0], vec[1], vec[2], vec[3]);
}
