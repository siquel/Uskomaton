#include "conf/config.hpp"
#include <fstream>
#include <iostream>

namespace uskomaton {
	namespace util {
		std::vector<std::string> split(const std::string&, char);
	}
}

using namespace uskomaton::config;

uskomaton::config::Configuration::Configuration() {

}

bool uskomaton::config::Configuration::fromFile(const std::string& filename, Configuration& conf) {
	using namespace boost::property_tree;
	std::ifstream settingsFile(filename, std::ifstream::in);
	if (!settingsFile.good()) return false;

	ptree tree;
	ini_parser::read_ini(settingsFile, tree);

	auto& globals = tree.get_child("global");
	std::string username(globals.get_optional<std::string>("username").value_or(""));
	std::string login(globals.get_optional<std::string>("login").value_or(""));
	std::string scriptPath(globals.get_optional<std::string>("script").get_value_or(""));
	std::string autoloadFrom(globals.get_optional<std::string>("autoload").get_value_or(""));
	
	conf.autoload = autoloadFrom;
	conf.scriptPath = scriptPath;

	for (const auto& entry : tree) {
		const std::string& key = entry.first;
		const boost::property_tree::ptree& value = entry.second;

		if (key.find("server") != std::string::npos) {
			size_t index = 0;
			if ((index = key.find(" ")) == std::string::npos) {
				std::cout << "Invalid server configuration, skipping " << key << std::endl;
				continue;
			}

			ServerConfiguration config;

			std::string name = key.substr(index + 1);
			std::cout << "Loading config for " << name << std::endl;
			config.name = name;
			config.hostname = value.get<std::string>("hostname");
			config.port = (value.get_optional<size_t>("port").get_value_or(6667));
			config.ssl = value.get_optional<bool>("ssl").get_value_or(false);
			config.channels = uskomaton::util::split(value.get<std::string>("channels"), ' ');
			config.scripts = uskomaton::util::split(value.get_optional<std::string>("scripts").get_value_or(""), ' ');
			config.auth = value.get_optional<bool>("auth").get_value_or(false);
			config.username = value.get_optional<std::string>("username").get_value_or(username);
			config.login = value.get_optional<std::string>("login").get_value_or(login);
			config.autoloadFrom = value.get_optional<std::string>("autoload").get_value_or(autoloadFrom);

			if (config.auth) {
				config.password = value.get<std::string>("password");
			}
			conf.servers.push_back(config);
		}
	}

	settingsFile.close();

	return true;
}

const std::string& uskomaton::config::Configuration::getAutoloadPath() const {
	return autoload;
}

const std::string& uskomaton::config::Configuration::getScriptPath() const {
	return scriptPath;
}

const std::vector<struct ServerConfiguration> Configuration::getServerConfigs() const {
	return servers;
}
