#include <Hexl/Runtime.hpp>
#include <Hexl/Log.hpp>
#include <stdio.h>
#include <SDL.h>
#include <chibi/eval.h>

static std::unique_ptr<Hexl::Runtime> g_runtime = nullptr;

extern "C" int main(int argc, char* argv[]) {
	sexp_scheme_init();
	g_runtime = Hexl::Runtime::Create(argc, (const char**)argv);
	if(!g_runtime) {
		Hexl::_internal::flushLog();
		fprintf(stderr, "Failed to initialize runtime\n");
		return 1;
	}
	while(!g_runtime->Update()) {}
	auto status = Hexl::Runtime::ExitStatus::OK;
	if(g_runtime->ExitRequested()) {
		status = g_runtime->ExitRequestedStatus();
	}
	g_runtime=nullptr;
	return (int)status;
}
