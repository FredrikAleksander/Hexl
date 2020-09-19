#include <Hexl/UI/Theme.hpp>
#include <sstream>

Hexl::UI::Theme::Theme()
	: m_change(1)
{
}

size_t Hexl::UI::Theme::GetNumChanges() const {
	return m_change;
}

bool Hexl::UI::Theme::IsFlagDescribed(const Hexl::String& name) const {
	auto i = m_flags.find(name);
	if(i != m_flags.end()) {
		return !i->second.info.description.empty();
	}
	return false;
}
bool Hexl::UI::Theme::IsFlagDefined(const Hexl::String &name) const {
	auto i = m_flags.find(name);
	if(i != m_flags.end()) {
		return i->second.defined;
	}
	return false;
}
bool Hexl::UI::Theme::IsColorDescribed(const Hexl::String &name) const {
	auto i = m_colors.find(name);
	if(i != m_colors.end()) {
		return !i->second.info.description.empty();
	}
	return false;
}

bool Hexl::UI::Theme::IsColorDefined(const Hexl::String &name) const {
	auto i = m_colors.find(name);
	if(i != m_colors.end()) {
		return i->second.defined;
	}
	return false;
}

bool Hexl::UI::Theme::IsScalarDescribed(const Hexl::String &name) const {
	auto i = m_scalars.find(name);
	if(i != m_scalars.end()) {
		return !i->second.info.description.empty();
	}
	return false;
}

bool Hexl::UI::Theme::IsScalarDefined(const Hexl::String &name) const {
	auto i = m_scalars.find(name);
	if(i != m_scalars.end()) {
		return i->second.defined;
	}
	return false;
}

bool Hexl::UI::Theme::IsVec2Described(const Hexl::String& name) const {
	auto i = m_vec2s.find(name);
	if(i != m_vec2s.end()) {
		return !i->second.info.description.empty();
	}
	return false;
}
bool Hexl::UI::Theme::IsVec2Defined(const Hexl::String& name) const {
	auto i = m_vec2s.find(name);
	if(i != m_vec2s.end()) {
		return i->second.defined;
	}
	return false;
}

bool Hexl::UI::Theme::IsVec3Described(const Hexl::String& name) const {
	auto i = m_vec3s.find(name);
	if(i != m_vec3s.end()) {
		return !i->second.info.description.empty();
	}
	return false;
}
bool Hexl::UI::Theme::IsVec3Defined(const Hexl::String& name) const {
	auto i = m_vec3s.find(name);
	if(i != m_vec3s.end()) {
		return i->second.defined;
	}
	return false;
}

bool Hexl::UI::Theme::IsVec4Described(const Hexl::String& name) const {
	auto i = m_vec4s.find(name);
	if(i != m_vec4s.end()) {
		return !i->second.info.description.empty();
	}
	return false;
}
bool Hexl::UI::Theme::IsVec4Defined(const Hexl::String& name) const {
	auto i = m_vec4s.find(name);
	if(i != m_vec4s.end()) {
		return i->second.defined;
	}
	return false;
}

Hexl::UI::Theme::ThemeFlag& Hexl::UI::Theme::GetOrCreateFlag(const Hexl::String& name) {
	auto i = m_flags.find(name);
	if(i == m_flags.end()) {
		m_flags[name] = ThemeFlag();
		i = m_flags.find(name);
		i->second.defined = false;
		i->second.value = false;
	}
	return i->second;
}

Hexl::UI::Theme::ThemeColor& Hexl::UI::Theme::GetOrCreateColor(const Hexl::String& name) {
	auto i = m_colors.find(name);
	if(i == m_colors.end()) {
		m_colors[name] = ThemeColor();
		i = m_colors.find(name);
		i->second.defined = false;
		i->second.value[0] = 0.0f; 
		i->second.value[1] = 0.0f;
		i->second.value[2] = 0.0f;
		i->second.value[3] = 0.0f;
	}
	return i->second;

}

Hexl::UI::Theme::ThemeScalar& Hexl::UI::Theme::GetOrCreateScalar(const Hexl::String& name) {
	auto i = m_scalars.find(name);
	if(i == m_scalars.end()) {
		m_scalars[name] = ThemeScalar();
		i = m_scalars.find(name);
		i->second.defined = false;
		i->second.value = 0.0f; 
	}
	return i->second;
}

Hexl::UI::Theme::ThemeVec2& Hexl::UI::Theme::GetOrCreateVec2(const Hexl::String& name) {
	auto i = m_vec2s.find(name);
	if(i == m_vec2s.end()) {
		m_vec2s[name] = ThemeVec2();
		i = m_vec2s.find(name);
		i->second.defined = false;
		i->second.value[0] = 0.0f; 
		i->second.value[1] = 0.0f;
	}
	return i->second;
}


Hexl::UI::Theme::ThemeVec3& Hexl::UI::Theme::GetOrCreateVec3(const Hexl::String& name) {
	auto i = m_vec3s.find(name);
	if(i == m_vec3s.end()) {
		m_vec3s[name] = ThemeVec3();
		i = m_vec3s.find(name);
		i->second.defined = false;
		i->second.value[0] = 0.0f; 
		i->second.value[1] = 0.0f;
		i->second.value[2] = 0.0f;
	}
	return i->second;
}


Hexl::UI::Theme::ThemeVec4& Hexl::UI::Theme::GetOrCreateVec4(const Hexl::String& name) {
	auto i = m_vec4s.find(name);
	if(i == m_vec4s.end()) {
		m_vec4s[name] = ThemeVec4();
		i = m_vec4s.find(name);
		i->second.defined = false;
		i->second.value[0] = 0.0f; 
		i->second.value[1] = 0.0f;
		i->second.value[2] = 0.0f;
		i->second.value[3] = 0.0f;
	}
	return i->second;
}

void Hexl::UI::Theme::DescribeFlag(const Hexl::String &name, Hexl::String &&description) {
	auto& color = GetOrCreateFlag(name);
	color.info.description = std::move(description);
}

void Hexl::UI::Theme::DescribeColor(const Hexl::String &name, Hexl::String &&description) {
	auto& color = GetOrCreateColor(name);
	color.info.description = std::move(description);
}

void Hexl::UI::Theme::DescribeScalar(const Hexl::String &name, Hexl::String &&description) {
	auto& scalar = GetOrCreateScalar(name);
	scalar.info.description = description;
}

void Hexl::UI::Theme::DescribeVec2(const Hexl::String &name, Hexl::String &&description) {
	auto& vec = GetOrCreateVec2(name);
	vec.info.description = std::move(description);
}
void Hexl::UI::Theme::DescribeVec3(const Hexl::String &name, Hexl::String &&description) {
	auto& vec = GetOrCreateVec3(name);
	vec.info.description = std::move(description);

}
void Hexl::UI::Theme::DescribeVec4(const Hexl::String &name, Hexl::String &&description) {
	auto& vec = GetOrCreateVec4(name);
	vec.info.description = std::move(description);
}

void Hexl::UI::Theme::BindFlag(const Hexl::String &name, FlagChangeHandler &&handler, bool immediate) {
	auto& flag = GetOrCreateFlag(name);
	if(immediate && flag.defined) {
		handler(name, flag.value);
	}
	flag.changeHandlers.emplace_back(handler);
}

void Hexl::UI::Theme::BindColor(const Hexl::String &name, ColorChangeHandler &&handler, bool immediate) {
	auto& color = GetOrCreateColor(name);
	if(immediate && color.defined) {
		handler(name, color.value[0], color.value[1], color.value[2], color.value[3]);
	}
	color.changeHandlers.emplace_back(std::move(handler));
}

void Hexl::UI::Theme::BindScalar(const Hexl::String &name, ScalarChangeHandler &&handler, bool immediate) {
	auto& scalar = GetOrCreateScalar(name);
	if(immediate && scalar.defined) {
		handler(name, scalar.value);
	}
	scalar.changeHandlers.emplace_back(std::move(handler));
}

void Hexl::UI::Theme::BindVec2(const Hexl::String &name, Vec2ChangeHandler &&handler, bool immediate) {
	auto vec = GetOrCreateVec2(name);
	if(immediate && vec.defined) {
		handler(name, vec.value[0], vec.value[1]);
	}
	vec.changeHandlers.emplace_back(handler);
}
void Hexl::UI::Theme::BindVec3(const Hexl::String &name, Vec3ChangeHandler &&handler, bool immediate) {
	auto vec = GetOrCreateVec3(name);
	if(immediate && vec.defined) {
		handler(name, vec.value[0], vec.value[1], vec.value[2]);
	}
	vec.changeHandlers.emplace_back(handler);

}
void Hexl::UI::Theme::BindVec4(const Hexl::String &name, Vec4ChangeHandler &&handler, bool immediate) {
	auto vec = GetOrCreateVec4(name);
	if(immediate && vec.defined) {
		handler(name, vec.value[0], vec.value[1], vec.value[2], vec.value[3]);
	}
	vec.changeHandlers.emplace_back(handler);
}

void Hexl::UI::Theme::SetFlag(const Hexl::String &name, bool value) {
	auto& flag = GetOrCreateFlag(name);
	flag.value = value;
	flag.defined = true;
	++m_change;

	for(auto& handler : flag.changeHandlers) {
		handler(name, value);
	}
}

void Hexl::UI::Theme::SetColor(const Hexl::String &name, float r, float g, float b, float a) {
	auto& color = GetOrCreateColor(name);
	color.value[0] = r;
	color.value[1] = g;
	color.value[2] = b;
	color.value[3] = a;
	color.defined = true;
	++m_change;

	for(auto& handler : color.changeHandlers) {
		handler(name, r, g, b, a);
	}
}

void Hexl::UI::Theme::SetScalar(const Hexl::String &name, float value) {
	auto& scalar = GetOrCreateScalar(name);
	scalar.value = value;
	scalar.defined = true;
	++m_change;
}

void Hexl::UI::Theme::SetVec2(const Hexl::String &name, float r, float g) {
	auto& vec = GetOrCreateVec2(name);
	vec.value[0] = r;
	vec.value[1] = g;
	vec.defined = true;
	++m_change;

	for(auto& handler : vec.changeHandlers) {
		handler(name, r, g);
	}
}

void Hexl::UI::Theme::SetVec3(const Hexl::String &name, float r, float g, float b) {
	auto& vec = GetOrCreateVec3(name);
	vec.value[0] = r;
	vec.value[1] = g;
	vec.value[2] = b;
	vec.defined = true;
	++m_change;

	for(auto& handler : vec.changeHandlers) {
		handler(name, r, g, b);
	}
}

void Hexl::UI::Theme::SetVec4(const Hexl::String &name, float r, float g, float b, float a) {
	auto& vec = GetOrCreateVec4(name);
	vec.value[0] = r;
	vec.value[1] = g;
	vec.value[2] = b;
	vec.value[3] = a;
	vec.defined = true;
	++m_change;

	for(auto& handler : vec.changeHandlers) {
		handler(name, r, g, b, a);
	}
}

Hexl::String Hexl::UI::Theme::ExportCurrentSettings() {
	std::ostringstream oss;

	oss.setf(std::ios_base::dec, std::ios_base::basefield);
	oss.setf(std::ios_base::fixed, std::ios_base::floatfield);

	for(auto& color : m_colors) {
		oss << "(runtime/set-theme-color \"" << color.first << "\" '(" << color.second.value[0] << " " << color.second.value[1] << " " << color.second.value[2] << " " << color.second.value[3] << "))" << std::endl;
	}

	for(auto& flag : m_flags) {
		oss << "(runtime/set-theme-flag \"" << flag.first << "\" '(" << flag.second.value << "))" << std::endl;
	}

	for(auto& scalar : m_scalars) {
		oss << "(runtime/set-theme-scalar \"" << scalar.first << "\" '(" << scalar.second.value << "))" << std::endl;
	}

	for(auto& vec : m_vec2s) {
		oss << "(runtime/set-theme-vec2 \"" << vec.first << "\" '(" << vec.second.value[0] << " " << vec.second.value[1] << "))" << std::endl;
	}

	for(auto& vec : m_vec3s) {
		oss << "(runtime/set-theme-vec3 \"" << vec.first << "\" '(" << vec.second.value[0] << " " << vec.second.value[1] << " " << vec.second.value[2] << "))" << std::endl;
	}

	for(auto& vec : m_vec4s) {
		oss << "(runtime/set-theme-vec4 \"" << vec.first << "\" '(" << vec.second.value[0] << " " << vec.second.value[1] << " " << vec.second.value[2] << " " << vec.second.value[3] << "))" << std::endl;
	}

	return oss.str();
}
