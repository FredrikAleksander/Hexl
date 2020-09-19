#include "zep/mcommon/string/stringutils.h"
#include <Hexl/String.hpp>
#include <string>
#include <locale>
#include <iomanip>
#include <codecvt>

void Hexl::Trim(Hexl::String& str, const Hexl::String& whitespace) {
	// TODO: Use own code
	Zep::Trim(str, whitespace.c_str());
}

size_t Hexl::ReplaceAll(Hexl::String& str, const Hexl::String& from, const Hexl::String& to) {
    if(from.empty())
        return 0;
    size_t n = 0;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
	n++;
    }
    return n;
}

template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

Hexl::WideString Hexl::EncodeWideString(const Hexl::String& utf8) {
	std::wstring_convert<
        	deletable_facet<std::codecvt<wchar_t, char, std::mbstate_t>>, wchar_t> wconv;
	return wconv.from_bytes(utf8.c_str());
}

Hexl::String Hexl::DecodeWideString(const Hexl::WideString &wstr) {
	std::wstring_convert<
        	deletable_facet<std::codecvt<wchar_t, char, std::mbstate_t>>, wchar_t> wconv;
	return wconv.to_bytes(wstr.c_str());
}
