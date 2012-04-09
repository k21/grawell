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
	ClientInfo(): address(), socket(), id(-1) {}
	sf::IPAddress address;
	sf::SocketTCP socket;
	int id;

};

class Server : public sf::Thread {

public:
	Server(short port_): clients(), port(port_), exit_(false),
			serverSocket(), universe() {}

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
