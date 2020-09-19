#include "Hexl/Log.hpp"
#include "Hexl/UI/Theme.hpp"
#include <Hexl/UI/Window.hpp>

Hexl::UI::Window::Window(const Hexl::String& id, const Hexl::String& name)
	: m_id(id)
	, m_name()
	, m_receiveFocus(false)
{
	SetName(name);
}

void Hexl::UI::Window::RegisterThemeProperties(Hexl::UI::Theme& theme) {
}

void Hexl::UI::Window::SetName(const Hexl::String& name) {
	m_name  = name;
	m_label = m_icon + m_name + Hexl::String("###") + m_id;
}

void Hexl::UI::Window::SetIcon(const Hexl::String& icon) {
	if(icon.back() != ' ')
		m_icon = icon + " ";
	else
		m_icon = icon;
	m_label = m_icon + m_name + "###" + m_id;
}

void Hexl::UI::Window::Focus() {
	LOG_VERBOSE(LogCategory::RUNTIME, "%s", "Hmm.");
	m_receiveFocus = true;
}

bool Hexl::UI::Window::ReceiveFocus() {
	auto r = m_receiveFocus;
	m_receiveFocus = false;
	return r;
}

const Hexl::String& Hexl::UI::Window::GetIcon() const {
	return m_icon;
}
const Hexl::String& Hexl::UI::Window::GetName() const {
	return m_name;
}
const Hexl::String& Hexl::UI::Window::GetLabel() const {
	return m_label;
}
