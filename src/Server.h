#ifndef SERVER_H_
#define SERVER_H_

#include <list>
#include <queue>
#include <set>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "Protocol.h"
#include "Universe.h"

class ClientInfo {

public:
	ClientInfo(const sf::IPAddress &address_,
			const sf::SocketTCP &socket_, unsigned short id_):
			address(address_), socket(socket_), id(id_),
			encoder(), decoder(),
			state(NOTHING),
			pending(0), pendingSize(0) {}
	sf::IPAddress address;
	sf::SocketTCP socket;
	unsigned short id;
	Encoder encoder;
	Decoder decoder;
	enum {
		NOTHING, ACCEPTED, PLAYING
	} state;
	char *pending; size_t pendingSize;

private:
	ClientInfo(const ClientInfo &);
	const ClientInfo &operator = (const ClientInfo &);

};

class Server : public sf::Thread {

public:
	Server(short port_): clients(), port(port_), exit_(false),
			serverSocket(), universe(), state(ROUND),
			checksum(0), freeIDs(), cntIDs(0), readyCnt(0), playersCnt(0) {}
	~Server() {
		while (!clients.empty()) {
			delete clients.front();
			clients.pop_front();
		}
	}

	void Run();
	void exit() { exit_ = true; this->Wait(); }

private:
	Server(const Server&);
	const Server& operator = (const Server&);

	int handleMessage(ClientInfo &client, const Message &m);
	void accept(ClientInfo &client, const Message &m,
			std::vector<Message> &toSend);
	unsigned short allocID();
	void freeID(unsigned short id);
	void changeState();
	void sendToAll(const std::vector<Message> &m);

	std::list<ClientInfo *> clients;
	short port;
	volatile bool exit_;
	sf::SocketTCP serverSocket;
	Universe universe;
	enum {
		SELECT_ACTION, ROUND
	} state;
	unsigned long checksum;
	std::set<unsigned short> freeIDs;
	unsigned short cntIDs;
	size_t readyCnt;
	size_t playersCnt;

};

#endif
