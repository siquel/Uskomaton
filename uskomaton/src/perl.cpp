#include "command.hpp"
#include "perl.hpp"

// TODO this may not be the greatest idea...
static void* ph = uskomaton_perl_new();
using namespace uskomaton::scripting;

EXTERN_C static void xs_init(pTHX);
int perl_load_file(char* filename);

class PerlScriptingAPI::_Impl {
private:
	PerlInterpreter* my_perl;
	Bot* bot;
public:
	PerlScriptingAPI::_Impl() : my_perl(nullptr), bot(nullptr) {

	}
	std::vector<HookData*> hooks;
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
		perl_parse(my_perl, xs_init, arg_count, perl_args, (char**)NULL);

		eval_pv(def, TRUE);

		perl_load_file("src\\api\\test.pl");
	}
	void deinitialize() {
		if (my_perl == NULL) return;
		perl_destruct(my_perl);
		perl_free(my_perl);
		PERL_SYS_TERM();
	}

	void registerPlugin(const char* name, const char* filename) {
		std::cout << name << std::endl << filename << std::endl;
		
		bot->addCommand(new PerlCallback(name));
	}

	void sendMessage(const char* channel, const char* message) {
		bot->sendMessage(channel, message);
	}
};


PerlScriptingAPI::PerlScriptingAPI() : pImpl(new PerlScriptingAPI::_Impl()) {

}

PerlScriptingAPI::~PerlScriptingAPI() {
	pImpl->deinitialize();
}

void PerlScriptingAPI::processRawMessage(const std::string& raw, const std::string& command, const std::string& target) {
	for (size_t i = 0; i < pImpl->hooks.size(); i++) {
		HookData* hook = pImpl->hooks[0];
		if (hook->name == command) {
			dSP;
			ENTER;
			SAVETMPS;
			PUSHMARK(SP);
			call_sv(hook->callback, G_DISCARD | G_NOARGS);
			PUTBACK;
			FREETMPS;
			LEAVE;
		}
	}
}

#pragma region XS

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
	char* channel;
	char* message;
	dXSARGS;
	if (items == 2) {
		channel = SvPV_nolen(ST(0));
		message = SvPV_nolen(ST(1));
		uskomaton_perl_send_message(ph, channel, message);
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
		return;
	}


}

XS(XS_uskomaton_print) {
	char* str;
	dXSARGS;
	if (items == 1) {
		str = SvPV_nolen(ST(0));
		std::cout << str;
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
}

EXTERN_C static void xs_init(pTHX) {
	newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, __FILE__);
	newXS("Uskomaton::Internal::register", XS_uskomaton_register, __FILE__);
	newXS("Uskomaton::Internal::print", XS_uskomaton_print, __FILE__);
	newXS("Uskomaton::Internal::hookServer", XS_uskomaton_hook_server, __FILE__);
	newXS("Uskomaton::Internal::sendMessage", XS_uskomaton_send_message, __FILE__);
}

#pragma endregion

#pragma region C binds
void* uskomaton_perl_new() {
	return PerlScriptingAPI::getInstance();
}

void uskomaton_perl_register(void* handle, char* name, char* filename) {
	PerlScriptingAPI* api = static_cast<PerlScriptingAPI*>(handle);

	api->pImpl->registerPlugin(name, filename);
}

void uskomaton_perl_hook_server(void* handle, HookData* data) {
	PerlScriptingAPI* api = static_cast<PerlScriptingAPI*>(handle);
	api->pImpl->hooks.push_back(data);
}

void uskomaton_perl_send_message(void* handle, const char* channel, const char* message) {
	PerlScriptingAPI* api = static_cast<PerlScriptingAPI*>(handle);
	api->pImpl->sendMessage(channel, message);
}
#pragma endregion

void PerlScriptingAPI::initialize(uskomaton::Bot* bot) {
	pImpl->initialize(bot);
}

PerlScriptingAPI* uskomaton::scripting::PerlScriptingAPI::getInstance() {
	static PerlScriptingAPI* api = new PerlScriptingAPI();
	return api;
}
