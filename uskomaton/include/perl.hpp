#pragma once
#include "scripting.hpp"
#include <iostream>
#include <memory>
#include <EXTERN.h>               
#include <perl.h>      
#include <XSUB.h>

namespace uskomaton {
	namespace scripting {
		class PerlScriptingAPI : public ScriptingAPI {
		private:
			PerlScriptingAPI();
		public:
			class _Impl;
			std::unique_ptr<_Impl> pImpl;
			~PerlScriptingAPI();
			void initialize();
			static PerlScriptingAPI* getInstance();
		};
	}

}

EXTERN_C static void uskomaton_perl_register(void* handle, char* name, char* filename);
EXTERN_C static void* uskomaton_perl_new();