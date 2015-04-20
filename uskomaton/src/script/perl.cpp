#include "bot.hpp"
#include "command/command.hpp"
#include "boost/filesystem.hpp"
#include "script/perl.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include "script/perl_server_hook.hpp"
#include "command/perl_script_command.hpp"
using namespace uskomaton::scripting;
using namespace uskomaton::command;

extern "C" {
	static void xs_init(pTHX);
	static int perl_load_file(char* filename);
	static void uskomaton_perl_register(void* handle, char* name, char* filename);
	static void uskomaton_perl_hook_server(void* handle, const char* filename, HookData* data);
	static void uskomaton_perl_hook_command(void* handle, const char* filename, HookData* data);
	static void uskomaton_perl_send_message(void* handle, const char* context, const char* channel, const char* message);
	static void uskomaton_perl_join_channel(void* handle, const char* context, const char* channel, const char* password);
	static const char* uskomaton_perl_get_nick(void* handle, const char* context);
	static void uskomaton_perl_part_channel(void* handle, const char* context, const char* channel);
	static void uskomaton_perl_get_channels(void* handle, const char* context);
	static void* uskomaton_perl_new();

}

// TODO this may not be the greatest idea...
static void* ph = uskomaton_perl_new();

class PerlScriptingAPI::_Impl {
private:
	PerlScriptingAPI* api;
	PerlInterpreter* my_perl;
	Bot* bot;
public:
	_Impl(PerlScriptingAPI* api) : api(api), my_perl(nullptr), bot(nullptr) {

	}
	PerlInterpreter* getPerl() const {
		return my_perl;
	}

	void initialize(uskomaton::Bot* bot) {
		this->bot = bot;
		char *perl_args[] = { "", "-e", "0", "-w" };
		int arg_count = 4;
		char* env[] = { "" };

		static const char def[] = { 
			#include "../perl/api.pm.h"
		};

		PERL_SYS_INIT3(&arg_count, (char ***)&perl_args, (char ***)&env);
		my_perl = perl_alloc();
		perl_construct(my_perl);
		PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
		PL_perl_destruct_level = 1;
		perl_parse(my_perl, xs_init, arg_count, perl_args, (char**)NULL);

		eval_pv(def, TRUE);
	}
	void deinitialize() {
		if (my_perl == NULL) return;
		PERL_SET_CONTEXT(my_perl);
		perl_destruct(my_perl);
		perl_free(my_perl);
		PERL_SYS_TERM();
	}

	void registerPlugin(const char* name, const char* filename) {
		std::cout << name << std::endl << filename << std::endl;
		api->loadedScripts.push_back(new Script(filename, name));
	}

	void hookServer(const char* filename, HookData* data) {
		Script* script = getScript(filename);
		assert(script != nullptr);
		script->hookServer(new PerlServerHook(data));
	}

	void hookCommand(const char* filename, const HookData* data) {
		Script* script = getScript(filename);
		assert(script != nullptr);
		script->hookCommand(new PerlScriptCommand(data));
	}

	Script* getScript(const char* filename) {
		for (size_t i = 0; i < api->loadedScripts.size(); i++) {
			if (api->loadedScripts[i]->getFilename() == filename)
				return api->loadedScripts[i];
		}
		return nullptr;
	}

	void sendMessage(const char* context, const char* channel, const char* message) {
		bot->sendMessage(context, channel, message);
	}
};


PerlScriptingAPI::PerlScriptingAPI() 
	: isInitialized(false), pImpl(new PerlScriptingAPI::_Impl(this)) {

}

PerlScriptingAPI::~PerlScriptingAPI() {
	pImpl->deinitialize();
}

void PerlScriptingAPI::processOnMessage(const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) {
	std::vector<const HookData*> hooks;
	getServerHooks([](ServerHook* hook) {
		return hook->getMessage() == "PRIVMSG";
	}, hooks);

	for (size_t i = 0; i < hooks.size(); i++) {
		const HookData* hook = hooks[i];
		executeHookWithArguments(hook, 4, context.c_str(), channel.c_str(), message.c_str(), sender.c_str());
	}
}

void PerlScriptingAPI::processRawMessage(const std::string& raw, const std::string& context, const std::string& command, const std::string& target) {
	
}


EXTERN_C void boot_DynaLoader (pTHX_ CV* cv);

static int execute_perl(SV* func, char* args) {
	int count, ret_value = 1;

	dSP;
	ENTER;
	SAVETMPS;

	PUSHMARK (SP);
	XPUSHs (sv_2mortal (newSVpv (args, 0)));
	PUTBACK;

	count = call_sv (func, G_EVAL | G_SCALAR);
	SPAGAIN;
	if (SvTRUE (ERRSV)) {
		printf("Perl error: %s\n", SvPV_nolen (ERRSV));
		if (!SvOK (POPs)) {}		/* remove undef from the top of the stack */
	} else if (count != 1) {
		printf ("Perl error: expected 1 value from %s, "
						  "got: %d\n", SvPV_nolen (func), count);
	} else {
		ret_value = static_cast<int>(POPi);
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	return ret_value;
}

static int perl_load_file(char* filename) {
	return execute_perl(sv_2mortal(newSVpv("Uskomaton::load", 0)), filename);
}

XS(XS_uskomaton_send_message) {
	char* context;
	char* channel;
	char* message;
	dXSARGS;
	if (items == 3) {
		context = SvPV_nolen(ST(0));
		channel = SvPV_nolen(ST(1));
		message = SvPV_nolen(ST(2));
		uskomaton_perl_send_message(ph, context, channel, message);
	}
	else {
		std::cout << "Usage: Uskomaton::Internal::sendMessage($context, $channel, $message)" << std::endl;
	}
}

XS (XS_uskomaton_register) {
	char* name;
	char* filename;
	dXSARGS;
	if (items == 2) {
		name = SvPV_nolen(ST(0));
		filename = SvPV_nolen(ST(1));

		uskomaton_perl_register(ph, name, filename);
		// loaded
		XSRETURN_IV(1);
	} else {
		std::cout << "Usage: Uskomaton::Internal::register($name, $filename)" << std::endl;
	}


}

XS(XS_uskomaton_print) {
	char* str;
	dXSARGS;
	if (items == 1) {
		str = SvPV_nolen(ST(0));
		std::cout << str;
	}
	else {
		std::cout << "Usage: Uskomaton::Internal::print($str)" << std::endl;
	}
}

XS(XS_uskomaton_hook_server) {
	char* filename;
	char* message;
	SV* callback;
	SV* package;
	HookData* hookdata = nullptr;
	dXSARGS;
	if (items == 4) {
		filename = SvPV_nolen(ST(0));
		message = SvPV_nolen(ST(1));
		callback = ST(2);
		package = ST(3);
		hookdata = new HookData;
		hookdata->callback = newSVsv(callback);
		hookdata->package = newSVsv(package);
		hookdata->name = std::string(message);
		uskomaton_perl_hook_server(ph, filename, hookdata);
		XSRETURN_IV(PTR2IV(hookdata));
	}
	else {
		std::cout << "Usage: Uskomaton::Internal::hookServer($filename, $message, $callback, $package)" << std::endl;
	}
}

XS(XS_uskomaton_get_channels) {
	char* context;
	dXSARGS;
	if (items == 1) {
		context = SvPV_nolen(ST(0));
		uskomaton_perl_get_channels(ph, context);
	}
	else {
		std::cout << "Usage: Uskomaton::Irc::getChannels($context)" << std::endl;
	}
}

XS(XS_uskomaton_get_nick) {
	char* context;
	dXSARGS;
	if (items == 1) {
		context = SvPV_nolen(ST(0));
		const char* nick = uskomaton_perl_get_nick(ph, context);
		// TODO return
	}
	else {
		std::cout << "Usage: Uskomaton::Irc::getNick($context)" << std::endl;
	}
}

XS(XS_uskomaton_join_channel) {
	char* context;
	char* channel;
	char* password;
	dXSARGS;
	if (items == 3) {
		context = SvPV_nolen(ST(0));
		channel = SvPV_nolen(ST(1));
		password = SvPV_nolen(ST(2));
		uskomaton_perl_join_channel(ph, context, channel, password);
	}
	else {
		std::cout << "Usage: Uskomaton::Irc::joinChannel($context, $channel, $password)" << std::endl;
	}
}

XS(XS_uskomaton_part_channel) {
	char* context;
	char* channel;
	dXSARGS;
	if (items == 2) {
		context = SvPV_nolen(ST(0));
		channel = SvPV_nolen(ST(1));
		uskomaton_perl_part_channel(ph, context, channel);
	}
	else {
		std::cout << "Usage: Uskomaton::Irc::partChannel($context, $channel)" << std::endl;
	}
}

XS(XS_uskomaton_hook_command) {
	char* filename;
	char* name;
	SV* package;
	SV* callback;
	HookData* data;
	dXSARGS;
	if (items == 4) {
		filename = SvPV_nolen(ST(0));
		name = SvPV_nolen(ST(1));
		package = ST(2);
		callback = ST(3);
		data = new HookData;
		data->callback = newSVsv(callback);
		data->package = newSVsv(package);
		data->name = std::string(name);
		uskomaton_perl_hook_command(ph, filename, data);
		XSRETURN_IV(PTR2IV(data));
	}
}

XS(XS_uskomaton_unhook) {
	HookData* data;
	dXSARGS;
	if (items == 1) {
		data = INT2PTR(HookData*, SvUV(ST(0)));
		if (data != NULL) {
			if (data->callback != NULL) {
				SvREFCNT_dec(data->callback);
			}
			if (data->package != NULL) {
				SvREFCNT_dec(data->package);
			}
			delete data;
		}

	}
	XSRETURN_EMPTY;
}

static void xs_init(pTHX) {
	newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, __FILE__);
	newXS("Uskomaton::Internal::register", XS_uskomaton_register, __FILE__);
	newXS("Uskomaton::Internal::print", XS_uskomaton_print, __FILE__);
	newXS("Uskomaton::Internal::hookServer", XS_uskomaton_hook_server, __FILE__);
	newXS("Uskomaton::Internal::sendMessage", XS_uskomaton_send_message, __FILE__);
	newXS("Uskomaton::Internal::joinChannel", XS_uskomaton_join_channel, __FILE__);
	newXS("Uskomaton::Internal::partChannel", XS_uskomaton_part_channel, __FILE__);
	newXS("Uskomaton::Internal::hookCommand", XS_uskomaton_hook_command, __FILE__);
	newXS("Uskomaton::Internal::unhook", XS_uskomaton_unhook, __FILE__);

	newXS("Uskomaton::Irc::getChannels", XS_uskomaton_get_channels, __FILE__);
	newXS("Uskomaton::Irc::getNick", XS_uskomaton_get_nick, __FILE__);
}

static void* uskomaton_perl_new() {
	return PerlScriptingAPI::getInstance();
}

static void uskomaton_perl_register(void* handle, char* name, char* filename) {
	PerlScriptingAPI* api = static_cast<PerlScriptingAPI*>(handle);

	api->pImpl->registerPlugin(name, filename);
}

static void uskomaton_perl_hook_server(void* handle, const char* filename, HookData* data) {
	PerlScriptingAPI* api = static_cast<PerlScriptingAPI*>(handle);
	api->pImpl->hookServer(filename, data);
}

static void uskomaton_perl_send_message(void* handle, const char* context, const char* channel, const char* message) {
	PerlScriptingAPI* api = static_cast<PerlScriptingAPI*>(handle);
	api->pImpl->sendMessage(context, channel, message);
}

static void uskomaton_perl_hook_command(void* handle, const char* filename, HookData* data) {
	PerlScriptingAPI* api = static_cast<PerlScriptingAPI*>(handle);
	api->pImpl->hookCommand(filename, data);
}

static void uskomaton_perl_join_channel(void* handle, const char* context, const char* channel, const char* password) {

}

static void uskomaton_perl_part_channel(void* handle, const char* context, const char* channel) {

}

static const char* uskomaton_perl_get_nick(void* handle, const char* context) {
	return nullptr;
}

void uskomaton_perl_get_channels(void* handle, const char* context) {

}

// TODO gotta pass context?
void PerlScriptingAPI::initialize(uskomaton::Bot* bot) {
	if (!isInitialized)
		pImpl->initialize(bot);
	isInitialized = true;
}

PerlScriptingAPI* uskomaton::scripting::PerlScriptingAPI::getInstance() {
	static PerlScriptingAPI* api = new PerlScriptingAPI();
	return api;
}

void uskomaton::scripting::PerlScriptingAPI::autoloadFrom(const std::string& path) {
	namespace fs = boost::filesystem;
	fs::path dir(path);
	fs::directory_iterator end;
	if (fs::exists(dir) && fs::is_directory(dir)) {
		for (fs::directory_iterator iter(dir); iter != end; ++iter) {
			if (fs::is_regular_file(iter->status())) {
				if (!boost::algorithm::ends_with(iter->path().string(), ".pl")) continue;
				std::string filename = iter->path().string();
				std::cout << "loading file " << filename << std::endl;
				char file[255] = { 0 };
				strcpy(file, filename.c_str());
				perl_load_file(file);
			}
		}
	}
	else {
		std::cout << path << " doesn't exist or isn't a directory" << std::endl;
	}

}

const std::vector<uskomaton::scripting::Script*> uskomaton::scripting::PerlScriptingAPI::getScripts() const {
	return loadedScripts;
}

void uskomaton::scripting::PerlScriptingAPI::unload(Script* s) {
	const char* cfile = s->getFilename().c_str();
	char file[255];
	strcpy(file, cfile);
	execute_perl(sv_2mortal(newSVpv("Uskomaton::unload", 0)), file);
	// TODO remove script
	loadedScripts.erase(std::find(loadedScripts.begin(), loadedScripts.end(), s));
	delete s;
}

void uskomaton::scripting::PerlScriptingAPI::getServerHooks(PerlScriptingHookPredicate pred, std::vector<const HookData*>& to) {
	for (size_t i = 0; i < loadedScripts.size(); i++) {
		auto hooks = loadedScripts[i]->getServerHooks();
		for (size_t j = 0; j < hooks.size(); j++) {
			ServerHook* hook = hooks[j];
			if (pred(hook)) {
				to.push_back(static_cast<PerlServerHook*>(hook)->getData());
			}
		}
	}
}

Script* PerlScriptingAPI::getScript(ScriptPredicate pred) const {
	for (size_t i = 0; i < loadedScripts.size(); i++) {
		if (pred(loadedScripts[i])) return loadedScripts[i];
	}
	return nullptr;
}

void PerlScriptingAPI::executeHookWithArguments(const HookData* hook, int argc, ...) {
	// TODO 
	assert(argc > 0);
	std::vector<char*> arguments;
	va_list vl;
	va_start(vl, argc);
	for (int i = 0; i < argc; i++) {
		arguments.push_back(va_arg(vl, char*));
	}
	va_end(vl);
	PERL_SET_CONTEXT(pImpl->getPerl());
	{
		dSP;

		ENTER;
		SAVETMPS;

		PUSHMARK(SP);
		for (size_t i = 0; i < arguments.size(); i++) {
			XPUSHs(sv_2mortal(newSVpv(arguments[i], 0)));
		}
		PUTBACK;
		call_sv(hook->callback, G_DISCARD);
		FREETMPS;
		LEAVE;
	}
}
