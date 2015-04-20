#pragma once
#include "server_hook.hpp"

struct HookData;

namespace uskomaton {
	namespace scripting {
		class PerlServerHook : public uskomaton::scripting::ServerHook {
		public:
			PerlServerHook(const HookData* data);
			const HookData* getData() const;
		private:
			const HookData* data;
		};
	}
}