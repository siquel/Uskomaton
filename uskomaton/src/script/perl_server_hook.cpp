#include "script/perl.hpp"
#include "script/perl_server_hook.hpp"
using namespace uskomaton::scripting;
PerlServerHook::PerlServerHook(const HookData* data)
	: ServerHook(data->name), data(data) {

}

const HookData* PerlServerHook::getData() const {
	return data;
}