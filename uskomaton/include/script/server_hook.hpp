#pragma once
#include <string>
namespace uskomaton {
	namespace scripting {
		class ServerHook {
		public:
			ServerHook(const std::string& message);
			const std::string& getMessage() const {
				return message;
			}
		private:
			const std::string message;
		};
	}
}
