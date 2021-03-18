#ifndef SERVER_H_
#define SERVER_H_

#include <cstdint>
#include <list>
#include <memory>
#include <queue>
#include <set>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "Log.h"
#include "Placer.h"
#include "Protocol.h"
#include "Universe.h"

class ClientInfo {

public:
	ClientInfo(const sf::IpAddress &address_,
			std::unique_ptr<sf::TcpSocket> &&socket_, uint16_t id_):
			address(address_), socket(std::move(socket_)), id(id_),
			encoder(), decoder(),
			pending(0), pendingSize(0),
			active(false) {}
	ClientInfo(const ClientInfo &) = delete;
	ClientInfo &operator = (const ClientInfo &) = delete;
	sf::IpAddress address;
	std::unique_ptr<sf::TcpSocket> socket;
	uint16_t id;
	Encoder encoder;
	Decoder decoder;
	char *pending; size_t pendingSize;
	bool active;

};

class Server : public sf::Thread {

public:
	Server(uint16_t port_):
			sf::Thread(&Server::run, this),
			clients(), port(port_), exit_(false),
			serverListener(), universe(), state(SELECT_ACTION),
			checksum(0), waitingForCnt(0),
			placer(200*FIXED_ONE) {}
	Server(const Server &) = delete;
	Server &operator = (const Server &) = delete;
	~Server() {
		while (!clients.empty()) {
			delete clients.front();
			clients.pop_front();
		}
	}

	void run();
	void exit() {
		exit_ = true;
		LOG(INFO) << "Waiting for server to terminate...";
		this->wait();
		LOG(INFO) << "Server terminated";
	}

	static bool isValidName(std::string name);

private:
	int8_t handleMessage(ClientInfo &client, const Message &m);
	void accept(ClientInfo &client, const Message &m,
			std::vector<Message> &toSend);
	void removeShip(uint16_t id);
	void changeState();
	void sendToAll(const std::vector<Message> &m);

	std::list<ClientInfo *> clients;
	uint16_t port;
	volatile bool exit_;
	sf::TcpListener serverListener;
	Universe universe;
	enum {
		SELECT_ACTION, ROUND
	} state;
	uint32_t checksum;
	size_t waitingForCnt;
	Placer placer;

};

#endif
