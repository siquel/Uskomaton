#pragma once
#include "scripting.hpp"
#include <EXTERN.h>               
#include <perl.h>      
#include <XSUB.h>

namespace uskomaton {
	namespace scripting {
		class PerlScriptingAPI : public ScriptingAPI {
		private:
			PerlInterpreter* my_perl;
		public:
			PerlScriptingAPI();
			~PerlScriptingAPI();
			void initialize();
		};
	}

}