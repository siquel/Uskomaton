#pragma once

#include <functional>
#include <vector>
#include "forward_declare.hpp"
USKOMATON_FORWARD_DECL_2(uskomaton, scripting, Script);
USKOMATON_FORWARD_DECL_1(uskomaton, Bot);
namespace uskomaton {
	namespace scripting {
		typedef std::function<bool(Script* script)> ScriptPredicate;
		class ScriptingAPI {
		public:
			ScriptingAPI() {};
			virtual ~ScriptingAPI() {};
			virtual void initialize(uskomaton::Bot* bot) = 0;
			virtual void processRawMessage(const std::string& raw, const std::string& context, const std::string& command, const std::string& target) = 0;
			virtual void processOnMessage(const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) = 0;
			virtual void autoloadFrom(const std::string& path) = 0;
			virtual const std::vector<Script*> getScripts() const = 0;
			virtual Script* getScript(ScriptPredicate pred) const = 0;
			virtual void unload(Script*) = 0;
		};
	}
}