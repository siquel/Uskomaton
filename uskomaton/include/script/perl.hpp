#pragma once
#include "scripting.hpp"
#include "script.hpp"
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
USKOMATON_FORWARD_DECL_1(uskomaton, Bot);
namespace uskomaton {
	namespace scripting {
		typedef std::function<int(uskomaton::scripting::ServerHook*)> PerlScriptingHookPredicate;
		class PerlScriptingAPI : public ScriptingAPI {
		private:
			PerlScriptingAPI();
			bool isInitialized;
			std::vector<Script*> loadedScripts;
			void getServerHooks(PerlScriptingHookPredicate pred, std::vector<const HookData*>& to);
		public:
			class _Impl;
			std::unique_ptr<_Impl> pImpl;
			~PerlScriptingAPI();
			void initialize(uskomaton::Bot* bot);
			static PerlScriptingAPI* getInstance();
			void processRawMessage(const std::string& raw, const std::string& context, const std::string& command, const std::string& target) override;
			void processOnMessage(const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) override;
			void autoloadFrom(const std::string& path);
			const std::vector<Script*> getScripts() const;
			void executeHookWithArguments(const HookData* hook, int argcount, ...);
			Script* getScript(ScriptPredicate pred) const override;
			void unload(Script*) override;
		};
	}

}