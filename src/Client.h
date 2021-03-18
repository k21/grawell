#ifndef CLIENT_H_
#define CLIENT_H_

#include <cstdint>
#include <queue>
#include <string>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "Log.h"
#include "Protocol.h"

class Client : public sf::Thread {

public:
	Client(const sf::IpAddress &address_, int16_t port_):
		sf::Thread(&Client::run, this),
		address(address_), port(port_), socket(),
		exit_(false), isConnected_(false), error_(false),
		mutexIn(), mutexOut(), incoming(), outgoing(), encoder(), decoder() {}
	Client(const Client &) = delete;
	Client &operator = (const Client &) = delete;

	void run();
	void exit() {
		exit_ = true;
		LOG(INFO) << "Waiting for client to terminate...";
		this->wait();
		LOG(INFO) << "Client terminated";
	}
	bool isConnected() const {
		return isConnected_;
	}
	bool error() const {
		return error_;
	}

	void send(Message &message);
	bool recv(Message &message);

private:
	int8_t sendPending();
	int8_t recvPending();

	sf::IpAddress address;
	uint16_t port;
	sf::TcpSocket socket;
	volatile bool exit_;
	volatile bool isConnected_;
	volatile bool error_;
	sf::Mutex mutexIn, mutexOut;
	std::queue<Message> incoming, outgoing;
	Encoder encoder;
	Decoder decoder;

};

#endif
