#pragma once
#ifndef __HEXL__STRING_HPP
#define __HEXL__STRING_HPP 1

#include <string>

namespace Hexl {

#ifndef __cpp_char8_t
using char8_t = char;
#endif

using String = std::basic_string<char8_t>;

// The wide character string class and functions are used when interfacing
// with a system that does not support UTF-8, but does support the platform
// dependent wide char implementation.
using WideString = std::basic_string<wchar_t>;

WideString EncodeWideString(const String& utf8);
String     DecodeWideString(const WideString& wstr);


void Trim(Hexl::String& str, const Hexl::String& whitespaceCharacters = "\t\n\r ");
size_t ReplaceAll(Hexl::String& str, const Hexl::String& from, const Hexl::String& to);
void QuoteString(Hexl::String& str);

}

#endif
