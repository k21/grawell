#ifndef CLIENT_H_
#define CLIENT_H_

#include <queue>
#include <string>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "Protocol.h"

class Client : public sf::Thread {

public:
	Client(const sf::IPAddress &address_, short port_, const std::string &name_):
		address(address_), port(port_), name(name_), exit_(false) {}

	void Run();
	void exit() { exit_ = true; this->Wait(); }

	void send(std::queue<Message> &messages);
	void recv(std::queue<Message> &messages);

private:
	int sendPending();
	int recvPending();

	sf::IPAddress address;
	short port;
	sf::SocketTCP socket;
	std::string name;
	volatile bool exit_;
	sf::Mutex mutexIn, mutexOut;
	std::queue<Message> incoming, outgoing;
	Encoder encoder;
	Decoder decoder;

};

#endif
