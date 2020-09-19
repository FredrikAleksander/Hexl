#pragma once
#ifndef __HEXL_SCHEME__SCHEMEENVIRONMENT_HPP
#define __HEXL_SCHEME__SCHEMEENVIRONMENT_HPP 1

#include <Hexl/PropertyMap.hpp>
#include <chibi/eval.h>
#include <chibi/gc_heap.h>
#include <memory>

namespace Hexl {
namespace Scheme {

class SchemeEnvironment final {
	private:
		sexp_gc_var6(ctx, env, res, err, out, in);
		bool m_initialized;
		SchemeEnvironment();
	public:
		~SchemeEnvironment();
		SchemeEnvironment(const SchemeEnvironment&) = delete;
		SchemeEnvironment(SchemeEnvironment&&)      = delete;

		SchemeEnvironment& operator=(const SchemeEnvironment&) = delete;
		SchemeEnvironment& operator=(SchemeEnvironment&&)      = delete;

		static std::unique_ptr<SchemeEnvironment> Create(const PropertyMap& properties);


		bool Load(const Hexl::String& path);
		bool IsFormComplete(const Hexl::String& input, int& indent);
		Hexl::String Repl(const Hexl::String& input);
};
}
}

#endif
