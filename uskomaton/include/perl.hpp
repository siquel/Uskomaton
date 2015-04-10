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
			bool isInitialized;
			PerlScriptingAPI();
		public:
			class _Impl;
			std::unique_ptr<_Impl> pImpl;
			~PerlScriptingAPI();
			void initialize(uskomaton::Bot* bot);
			static PerlScriptingAPI* getInstance();
			void processRawMessage(const std::string& raw, const std::string& context, const std::string& command, const std::string& target);
			void autoloadFrom(const std::string& path);
		};
	}

}
extern "C" {
	static void uskomaton_perl_register(void* handle, char* name, char* filename);
	static void uskomaton_perl_hook_server(void* handle, HookData* data);
	static void uskomaton_perl_send_message(void* handle, const char* context, const char* channel, const char* message);
	static void* uskomaton_perl_new();
}
