#include "command.hpp"
#include "boost/filesystem.hpp"
#include "perl.hpp"
#include <boost/algorithm/string/predicate.hpp>

// TODO this may not be the greatest idea...
static void* ph = uskomaton_perl_new();
using namespace uskomaton::scripting;

extern "C" {
	static void xs_init(pTHX);
	static int perl_load_file(char* filename);
}

class PerlScriptingAPI::_Impl {
private:
	PerlScriptingAPI* api;
	PerlInterpreter* my_perl;
	Bot* bot;
public:
	_Impl(PerlScriptingAPI* api) : api(api), my_perl(nullptr), bot(nullptr) {

	}
	std::vector<HookData*> hooks;
	PerlInterpreter* getPerl() const {
		return my_perl;
	}

	void initialize(uskomaton::Bot* bot) {
		this->bot = bot;
		char *perl_args[] = { "", "-e", "0", "-w" };
		int arg_count = 4;
		char* env[] = { "" };

		static const char def[] = { 
			#include "api/api.pm.h"
		};

		PERL_SYS_INIT3(&arg_count, (char ***)&perl_args, (char ***)&env);
		my_perl = perl_alloc();
		perl_construct(my_perl);
		PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
		PL_perl_destruct_level = 1;
		perl_parse(my_perl, xs_init, arg_count, perl_args, (char**)NULL);

		eval_pv(def, TRUE);

		//perl_load_file("src\\api\\test.pl");
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

	void hookCommand(const char* filename, const HookData* data) {
		Script* script = getScript(filename);
		assert(script != nullptr);
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
	for (size_t i = 0; i < pImpl->hooks.size(); i++) {
		HookData* hook = pImpl->hooks[0];
		if (hook->name == "PRIVMSG") {
			PERL_SET_CONTEXT(pImpl->getPerl());
			{
				dSP;

				ENTER;
				SAVETMPS;

				PUSHMARK(SP);
				XPUSHs(sv_2mortal(newSVpv(context.c_str(), 0)));
				XPUSHs(sv_2mortal(newSVpv(channel.c_str(), 0)));
				XPUSHs(sv_2mortal(newSVpv(message.c_str(), 0)));
				XPUSHs(sv_2mortal(newSVpv(sender.c_str(), 0)));
				PUTBACK;
				call_sv(hook->callback, G_DISCARD);
				FREETMPS;
				LEAVE;
			}
		}
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
		ret_value = POPi;
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
	char* message;
	SV* callback;
	SV* package;
	HookData* hookdata = nullptr;
	dXSARGS;
	if (items == 3) {
		message = SvPV_nolen(ST(0));
		callback = ST(1);
		package = ST(2);
		hookdata = new HookData;
		hookdata->callback = newSVsv(callback);
		hookdata->package = newSVsv(package);
		hookdata->name = std::string(message);
		uskomaton_perl_hook_server(ph, hookdata);
	}
	else {
		std::cout << "Usage: Uskomaton::Internal::hookServer($message, $callback, $package)" << std::endl;
	}
}

XS(XS_uskomaton_get_channels) {
	char* context;
	dXSARGS;
	if (items == 1) {
		context = SvPV_nolen(ST(0));
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

	}
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

static void uskomaton_perl_hook_server(void* handle, HookData* data) {
	PerlScriptingAPI* api = static_cast<PerlScriptingAPI*>(handle);
	api->pImpl->hooks.push_back(data);
}

static void uskomaton_perl_send_message(void* handle, const char* context, const char* channel, const char* message) {
	PerlScriptingAPI* api = static_cast<PerlScriptingAPI*>(handle);
	api->pImpl->sendMessage(context, channel, message);
}

static void uskomaton_perl_hook_command(void* handle, const char* filename, HookData* data) {
	PerlScriptingAPI* api = static_cast<PerlScriptingAPI*>(handle);
	api->pImpl->hookCommand(filename, data);
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
