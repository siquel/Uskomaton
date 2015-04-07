#pragma once
#include <vector>
#include <functional>

class IrcMessageListener {
public:
	IrcMessageListener() = default;
	~IrcMessageListener() = default;
	virtual void onRawMessage(const std::string& msg);
};

typedef std::function<void(IrcMessageListener*)> notification_t;

class IrcClient {
private:
	std::vector<IrcMessageListener*> listeners;
	void onRawMessage(const std::string& msg);
	void notifyListeners(notification_t what);
	std::string login;
	std::string nick;
public:
	IrcClient(const std::string& nick, const std::string& login);
	void addListener(IrcMessageListener* listener);
	void connectTo(const std::string& hostname, int port, const std::string& password);
	void connectTo(const std::string& hostname, int port);
	void connectTo(const std::string& hostname);
	void sendMessage(const std::string& channel, const std::string& message);
	void sendRawMessage(const std::string& line);
	// debug
	void start();
};