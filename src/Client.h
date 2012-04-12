#ifndef CLIENT_H_
#define CLIENT_H_

#include <queue>
#include <string>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "Protocol.h"

class Client : public sf::Thread {

public:
	Client(const sf::IPAddress &address_, short port_):
		address(address_), port(port_), socket(), exit_(false),
		mutexIn(), mutexOut(), incoming(), outgoing(), encoder(), decoder() {}

	void Run();
	void exit() { exit_ = true; this->Wait(); }

	void send(Message &message);
	bool recv(Message &message);

private:
	int sendPending();
	int recvPending();

	sf::IPAddress address;
	short port;
	sf::SocketTCP socket;
	volatile bool exit_;
	sf::Mutex mutexIn, mutexOut;
	std::queue<Message> incoming, outgoing;
	Encoder encoder;
	Decoder decoder;

};

#endif
