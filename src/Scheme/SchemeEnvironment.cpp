#include <Hexl/Scheme/SchemeEnvironment.hpp>
#include <Hexl/Log.hpp>
#include <sstream>

static sexp sexp_meta_env (sexp ctx) {
	if (sexp_envp(sexp_global(ctx, SEXP_G_META_ENV)))
		return sexp_global(ctx, SEXP_G_META_ENV);
	return sexp_context_env(ctx);
}

static sexp sexp_add_import_binding (sexp ctx, sexp env) {
	sexp_gc_var2(sym, tmp);
	sexp_gc_preserve2(ctx, sym, tmp);
	sym = sexp_intern(ctx, "repl-import", -1);
	tmp = sexp_env_ref(ctx, sexp_meta_env(ctx), sym, SEXP_VOID);
	sym = sexp_intern(ctx, "import", -1);
	sexp_env_define(ctx, env, sym, tmp);
	sexp_gc_release3(ctx);
	return env;
}

Hexl::Scheme::SchemeEnvironment::SchemeEnvironment()
	: m_initialized(false)
{
}

Hexl::Scheme::SchemeEnvironment::~SchemeEnvironment() {
	if(m_initialized) {
		LOG_VERBOSE(LogCategory::SCHEME, "%s", "Shutting down Scheme environment");
		LOG_VERBOSE(Hexl::LogCategory::SCHEME, "%s", "Unbinding Scheme standard I/O");
		sexp_close_port(ctx, in);
		sexp_close_port(ctx, out);
		sexp_release_object(ctx, in);
		sexp_release_object(ctx, out);
		LOG_VERBOSE(LogCategory::SCHEME, "%s", "Destroying Scheme context");
		if(sexp_destroy_context(ctx) == SEXP_FALSE) {
			LOG_ERROR(LogCategory::SCHEME, "%s", "Failed to destroy Scheme context");
		}
	}
}


extern "C" sexp sexp_init_runtime_library (sexp ctx, sexp self, sexp_sint_t n, sexp env, const char* version, const sexp_abi_identifier_t abi);

std::unique_ptr<Hexl::Scheme::SchemeEnvironment> Hexl::Scheme::SchemeEnvironment::Create(const PropertyMap &properties) {
	SchemeEnvironment* ptr = new SchemeEnvironment();
	LOG_VERBOSE(LogCategory::SCHEME, "%s", "Creating Scheme context");
	ptr->ctx = sexp_make_eval_context(nullptr, nullptr, nullptr, 0, 0);
	if(sexp_exceptionp(ptr->ctx)) {
		LOG_ERROR(LogCategory::SCHEME, "%s", "Failed to create Scheme context");
		return std::unique_ptr<SchemeEnvironment>();
	}
	
	LOG_VERBOSE(LogCategory::SCHEME, "%s", "Loading Scheme standard libraries");
	
	sexp_load_standard_env(ptr->ctx, nullptr, SEXP_SEVEN);
	ptr->env = sexp_context_env(ptr->ctx);

	sexp_add_import_binding(ptr->ctx, ptr->env);
	sexp res;
	res = sexp_init_runtime_library(ptr->ctx, NULL, 3, ptr->env, sexp_version, SEXP_ABI_IDENTIFIER);

  	if (res == SEXP_ABI_ERROR)
		res = sexp_global(ptr->ctx, SEXP_G_ABI_ERROR);

	
	sexp_gc_var2(in, out);
	LOG_VERBOSE(Hexl::LogCategory::SCHEME, "%s", "Binding Scheme standard I/O");
	
	in = sexp_open_input_string(ptr->ctx, sexp_c_string(ptr->ctx, "", -1));
	out = sexp_open_output_string(ptr->ctx);
	
	sexp_gc_preserve2(ptr->ctx, in, out);
	
	sexp_port_no_closep(in) = 1;
	sexp_port_no_closep(out) = 1;
	
	sexp_set_parameter(ptr->ctx, ptr->env, sexp_global(ptr->ctx, SEXP_G_CUR_IN_SYMBOL), in);
	sexp_set_parameter(ptr->ctx, ptr->env, sexp_global(ptr->ctx, SEXP_G_CUR_OUT_SYMBOL), out);
	sexp_set_parameter(ptr->ctx, ptr->env, sexp_global(ptr->ctx, SEXP_G_CUR_ERR_SYMBOL), out);
	
	ptr->in = in;
	ptr->out = out;
	ptr->err = out;
	
	sexp_preserve_object(ptr->ctx, ptr->in);
	sexp_preserve_object(ptr->ctx, ptr->out);
	
	sexp_gc_release2(ptr->ctx);

	ptr->m_initialized = true;
	
	LOG_VERBOSE(LogCategory::SCHEME, "%s", "Initialized Scheme environment");

	return std::unique_ptr<SchemeEnvironment>(ptr);
}

static Hexl::String ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

static Hexl::String EscapePath(Hexl::String&& input) {
	static Hexl::String slash = "\\";
	static Hexl::String escaped_slash = "\\\\";
	return ReplaceAll(input, slash, escaped_slash);
}

static void PrintOutput(Hexl::LogLevel level, Hexl::String&& lines) {
	static const Hexl::String delim = "\n";

	if(lines.empty())
		return;

	std::istringstream iss(std::move(lines));
	
	for (std::string line; std::getline(iss, line); )
	{
		if(line.find("WARNING: ") == 0) {
			Hexl::Log(Hexl::LogCategory::SCHEME, Hexl::LogLevel::LOG_WARN, "%s", &line[sizeof("WARNING: ")-1]);
		}
		else if(line.find("ERROR: ") == 0) {
			Hexl::Log(Hexl::LogCategory::SCHEME, Hexl::LogLevel::LOG_ERROR, "%s", &line[sizeof("ERROR: ")-1]);
		}
		else {
			Hexl::Log(Hexl::LogCategory::SCHEME, level, "%s", line.c_str());
		}
	}
}

bool Hexl::Scheme::SchemeEnvironment::Load(const Hexl::String &path) {
	//LOG_ERROR(LogCategory::SCHEME, "Not implemented `bool Hexl::Scheme::SchemeEnvironment::Load(const Hexl::String& path)`: %s:%d", __FILE__, __LINE__);
	sexp_gc_var5(tmp, res, obj, out, out_str);
	sexp_gc_preserve5(ctx, tmp, res, obj, out, out_str);

	bool retVal = true;

	if(env == nullptr) {
		env = sexp_context_env(ctx);
	}

	out = sexp_open_output_string(ctx);
	sexp_set_parameter(ctx, env, sexp_global(ctx, SEXP_G_CUR_ERR_SYMBOL), out);
	sexp_set_parameter(ctx, env, sexp_global(ctx, SEXP_G_CUR_OUT_SYMBOL), out);

	Hexl::String input = Hexl::String("(load \"") + EscapePath(Hexl::String(path)) + Hexl::String("\")");

	obj = sexp_eval_string(ctx, input.c_str(), -1, NULL); // (ctx, in);
	if (sexp_exceptionp(obj))
	{
		sexp_print_exception(ctx, obj, out);
		retVal = false;
	}
	else
	{
		sexp_context_top(ctx) = 0;
		if (!(sexp_idp(obj) || sexp_pairp(obj) || sexp_nullp(obj)))
			obj = sexp_make_lit(ctx, obj);
		tmp = sexp_env_bindings(env);
		res = sexp_eval(ctx, obj, env);
		sexp_warn_undefs(ctx, sexp_env_bindings(env), tmp, res);

		if (res && sexp_exceptionp(res))
		{
			retVal = false;
			sexp_print_exception(ctx, res, out);
			if (res != sexp_global(ctx, SEXP_G_OOS_ERROR))
			{
				sexp_stack_trace(ctx, out);
			}
		}
		else if (res != SEXP_VOID)
		{
			sexp_write(ctx, res, out);
		}
	}
	out_str = sexp_get_output_string(ctx, out);

	auto ret = sexp_string_data(out_str);

	if(retVal) {
		PrintOutput(Hexl::LogLevel::LOG_INFO, ret);
	} else {
		PrintOutput(Hexl::LogLevel::LOG_ERROR, ret);
	}

	sexp_gc_release5(ctx);

	sexp_set_parameter(ctx, env, sexp_global(ctx, SEXP_G_CUR_ERR_SYMBOL), err);
	sexp_set_parameter(ctx, env, sexp_global(ctx, SEXP_G_CUR_OUT_SYMBOL), out);


	return retVal;
}

bool Hexl::Scheme::SchemeEnvironment::IsFormComplete(const Hexl::String& str, int& indent) {
	int count = 0;
	for (auto& ch : str) {
		if (ch == '(')
			count++;
		if (ch == ')')
			count--;
	}
	
	if (count < 0)
	{
		indent = -1;
		return false;
	}
	else if (count == 0)
	{
		return true;
	}
	
	int count2 = 0;
	indent = 1;
	for (auto& ch : str)
	{
		if (ch == '(')
			count2++;
		if (ch == ')')
			count2--;
		if (count2 == count)
		{
			break;
		}
		indent++;
	}
	return false;

}

Hexl::String Hexl::Scheme::SchemeEnvironment::Repl(const Hexl::String& input) {
	sexp_gc_var5(tmp, res, obj, out, out_str);
	sexp_gc_preserve5(ctx, tmp, res, obj, out, out_str);

	if(env == nullptr) {
		env = sexp_context_env(ctx);
	}

	out = sexp_open_output_string(ctx);
	sexp_set_parameter(ctx, env, sexp_global(ctx, SEXP_G_CUR_ERR_SYMBOL), out);
	sexp_set_parameter(ctx, env, sexp_global(ctx, SEXP_G_CUR_OUT_SYMBOL), out);

	obj = sexp_eval_string(ctx, input.c_str(), -1, NULL); // (ctx, in);
	if (sexp_exceptionp(obj))
	{
		sexp_print_exception(ctx, obj, out);
	}
	else
	{
		sexp_context_top(ctx) = 0;
		if (!(sexp_idp(obj) || sexp_pairp(obj) || sexp_nullp(obj)))
			obj = sexp_make_lit(ctx, obj);
		tmp = sexp_env_bindings(env);
		res = sexp_eval(ctx, obj, env);
		sexp_warn_undefs(ctx, sexp_env_bindings(env), tmp, res);

		if (res && sexp_exceptionp(res))
		{
			sexp_print_exception(ctx, res, out);
			if (res != sexp_global(ctx, SEXP_G_OOS_ERROR))
			{
				sexp_stack_trace(ctx, out);
			}
		}
		else if (res != SEXP_VOID)
		{
			sexp_write(ctx, res, out);
		}
	}
	out_str = sexp_get_output_string(ctx, out);

	auto ret = sexp_string_data(out_str);
	sexp_gc_release5(ctx);

	sexp_set_parameter(ctx, env, sexp_global(ctx, SEXP_G_CUR_ERR_SYMBOL), err);
	sexp_set_parameter(ctx, env, sexp_global(ctx, SEXP_G_CUR_OUT_SYMBOL), out);

	return ret;
}
