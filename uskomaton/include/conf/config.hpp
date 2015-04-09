#pragma once
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <vector>
namespace uskomaton {
	namespace config {
		class Configuration {
		private:
			std::vector<struct ServerConfiguration> servers;
		public:
			Configuration();
			static bool fromFile(const std::string&, Configuration& conf);
			const std::vector<struct ServerConfiguration> getServerConfigs() const;
			const std::string& getAutoloadPath() const;
			const std::string& getScriptPath() const;
		private:
			std::string autoload;
			std::string scriptPath;
		};

		struct ServerConfiguration {
			std::string name;
			std::string login;
			std::string hostname;
			std::string username;
			std::string password;
			std::vector<std::string> channels;
			std::vector<std::string> scripts;
			std::string autoloadFrom;
			bool auth;
			int port;
			bool ssl;
		};
		
	}
}