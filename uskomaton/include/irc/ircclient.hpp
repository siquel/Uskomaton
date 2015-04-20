#pragma once
#include <vector>
#include <functional>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/signals2/signal.hpp>
#include "forward_declare.hpp"
USKOMATON_FORWARD_DECL_1(uskomaton, Bot);
class IrcMessageListener {
public:
	IrcMessageListener() = default;
	~IrcMessageListener() = default;
	virtual void onRawMessage(const std::string& msg, const std::string& command, const std::string& target);
	virtual void onMessage(const std::string& channel, const std::string& message, const std::string& sender);
	virtual void onPrivateMessage(const std::string& target, const std::string& message, const std::string& sender);
	virtual void onServerPing(const std::string& ping);
	virtual void onJoinChannel(const std::string& channel, const std::string& sender);
};
namespace uskomaton {
	namespace irc {

		typedef std::function<void(IrcMessageListener*)> notification_t;

		class IrcClient {
		private:
			uskomaton::Bot* bot;
			std::string server;
			std::string nick;
			std::string login;

			boost::asio::io_service ioService;
			boost::asio::streambuf buffer;
			boost::asio::ip::tcp::socket socket;

			boost::thread inputThread;
			boost::thread outputThread;

			std::vector<IrcMessageListener*> listeners;

			void onRawMessage(const std::string& msg);
			void onServerPing(const std::string& ping);
			void notifyListeners(notification_t what);
			void receive(boost::system::error_code const&, size_t);
			void read();
			void processCommand(const std::string& command, const std::string& target, std::string& sender, std::vector<std::string>& tokens);
			void processServerResponse(int code, const std::string& line, std::vector<std::string>& tokens);

		public:
			IrcClient(uskomaton::Bot*, const std::string& context, const std::string& nick, const std::string& login);
			~IrcClient();
			const std::string& getServerName() const;
			void addListener(IrcMessageListener* listener);
			void connectTo(const std::string& hostname, int port, const std::string& password);
			void connectTo(const std::string& hostname, int port);
			void connectTo(const std::string& hostname);
			void sendMessage(const std::string& channel, const std::string& message);
			void sendRawMessage(const std::string& line);
			void sendNick(const std::string& nick);
			void sendUser(const std::string& login);
			void sendJoinChannel(const std::string& channel, const std::string& password = "");
			void sendPartChannel(const std::string& channel);
			void terminate();
		};
	}
}