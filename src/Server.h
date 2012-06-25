#ifndef SERVER_H_
#define SERVER_H_

#include <list>
#include <queue>
#include <set>

#include <boost/cstdint.hpp>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "Log.h"
#include "Placer.h"
#include "Protocol.h"
#include "Universe.h"

class ClientInfo {

public:
	ClientInfo(const sf::IPAddress &address_,
			const sf::SocketTCP &socket_, boost::uint16_t id_):
			address(address_), socket(socket_), id(id_),
			encoder(), decoder(),
			pending(0), pendingSize(0),
			active(false) {}
	ClientInfo(const ClientInfo &) = delete;
	ClientInfo &operator = (const ClientInfo &) = delete;
	sf::IPAddress address;
	sf::SocketTCP socket;
	boost::uint16_t id;
	Encoder encoder;
	Decoder decoder;
	char *pending; size_t pendingSize;
	bool active;

};

class Server : public sf::Thread {

public:
	Server(boost::uint16_t port_): clients(), port(port_), exit_(false),
			serverSocket(), universe(), state(SELECT_ACTION),
			checksum(0), freeIDs(), cntIDs(0), waitingForCnt(0),
			placer(200*FIXED_ONE) {}
	Server(const Server &) = delete;
	Server &operator = (const Server &) = delete;
	~Server() {
		while (!clients.empty()) {
			delete clients.front();
			clients.pop_front();
		}
	}

	void Run();
	void exit() {
		exit_ = true;
		LOG(INFO) << "Waiting for server to terminate...";
		this->Wait();
		LOG(INFO) << "Server terminated";
	}

private:
	boost::int8_t handleMessage(ClientInfo &client, const Message &m);
	void accept(ClientInfo &client, const Message &m,
			std::vector<Message> &toSend);
	boost::uint16_t allocID();
	void freeID(boost::uint16_t id);
	void changeState();
	void sendToAll(const std::vector<Message> &m);

	std::list<ClientInfo *> clients;
	boost::uint16_t port;
	volatile bool exit_;
	sf::SocketTCP serverSocket;
	Universe universe;
	enum {
		SELECT_ACTION, ROUND
	} state;
	boost::uint32_t checksum;
	std::set<boost::uint16_t> freeIDs;
	boost::uint16_t cntIDs;
	size_t waitingForCnt;
	Placer placer;

};

#endif
