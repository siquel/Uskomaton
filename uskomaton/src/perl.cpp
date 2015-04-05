#include "perl.hpp"

using namespace uskomaton::scripting;

PerlScriptingAPI::PerlScriptingAPI() : my_perl(nullptr) {

}

PerlScriptingAPI::~PerlScriptingAPI() {
	if (my_perl == NULL) return;
	perl_destruct(my_perl);
	perl_free(my_perl);
	PERL_SYS_TERM();
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

XS (XS_uskomaton_register) {
	char* name;
	char* filename;
	dXSARGS;
	if (items == 2) {
		name = SvPV_nolen(ST(0));
		filename = SvPV_nolen(ST(1));
		// loaded
		XSRETURN_IV(1);
	} else {
		return;
	}


}

EXTERN_C static void xs_init(pTHX) {
	newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, __FILE__);
	newXS("Uskomaton::Internal::register", XS_uskomaton_register, __FILE__);
}

#pragma endregion

void PerlScriptingAPI::initialize() {
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