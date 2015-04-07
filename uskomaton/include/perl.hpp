#pragma once
#include "scripting.hpp"
#include <iostream>
#include <memory>
#include <EXTERN.h>               
#include <perl.h>      
#include <XSUB.h>

typedef struct HookData {
	std::string name;
	SV* callback;
	SV* package;
} HookData;
namespace uskomaton {
	class Bot;
	namespace scripting {
		class PerlScriptingAPI : public ScriptingAPI {
		private:
			PerlScriptingAPI();
		public:
			class _Impl;
			std::unique_ptr<_Impl> pImpl;
			~PerlScriptingAPI();
			void initialize(uskomaton::Bot* bot);
			static PerlScriptingAPI* getInstance();
			void processRawMessage(const std::string& raw);
		};
	}

}

EXTERN_C static void uskomaton_perl_register(void* handle, char* name, char* filename);
EXTERN_C static void uskomaton_perl_hook_server(void* handle, HookData* data);
EXTERN_C static void* uskomaton_perl_new();