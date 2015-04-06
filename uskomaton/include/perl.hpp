#pragma once
#include "scripting.hpp"
#include <iostream>
#include <EXTERN.h>               
#include <perl.h>      
#include <XSUB.h>

namespace uskomaton {
	namespace scripting {
		class PerlScriptingAPI : public ScriptingAPI {
		public:
			class _Impl;
			// todo unique_ptr
			_Impl* pImpl;
			PerlScriptingAPI();
			~PerlScriptingAPI();
			void initialize();
		};
	}

}

EXTERN_C static void uskomaton_perl_register(void* handle, char* name, char* filename);
EXTERN_C static void* uskomaton_perl_new();
EXTERN_C void* getPerl();