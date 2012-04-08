#ifndef SERVER_H_
#define SERVER_H_

#include <list>
#include <queue>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "Protocol.h"
#include "Universe.h"

class ClientInfo {

public:
	sf::IPAddress address;
	sf::SocketTCP socket;
	int id;

};

class Server : public sf::Thread {

public:
	Server(short port_): port(port_), exit_(false) {}

	void Run();
	void exit() { exit_ = true; this->Wait(); }

private:
	std::list<ClientInfo> clients;
	short port;
	volatile bool exit_;
	sf::SocketTCP serverSocket;
	Universe universe;

};

#endif
