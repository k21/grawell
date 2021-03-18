#include "Protocol.h"

#include "Client.h"

using namespace std;
using namespace sf;

void Client::run() {
	LOG(INFO) << "Connecting to server at " << address << ':' << port << "...";
	static const int16_t RETRY = 10;
	Socket::Status status = socket.connect(address, port);
	int16_t r = 1;
	while (status != Socket::Done && r < RETRY && !exit_) {
		LOG(WARN) << "Connection attempt unsuccessful";
		sf::sleep(seconds(1.f));
		if (exit_) break;
		LOG(WARN) << "Trying to connect again...";
		status = socket.connect(address, port);
		++r;
	}
	if (status != Socket::Done) {
		error_ = true;
		LOG(ERR) << "Could not connect to server";
		return;
	}
	isConnected_ = true;
	socket.setBlocking(false);
	while (!exit_) {
		bool nothing = true;
		int8_t s = sendPending();
		if (s == -1) {
			error_ = true;
			LOG(ERR) << "An error has occured when sending packets";
			break;
		} else if (s == 1) nothing = false;
		s = recvPending();
		if (s == -1) {
			error_ = true;
			LOG(ERR) << "An error has occured when receiving packets";
			break;
		} else if (s == 1) nothing = false;
		if (nothing) sf::sleep(seconds(0.05f));
	}
	isConnected_ = false;
}

int8_t Client::sendPending() {
	bool something = false;
	vector<Message> toSend;
	{
		Lock lock(mutexOut);
		if (!outgoing.empty()) something = true;
		while (!outgoing.empty()) {
			toSend.push_back(outgoing.front());
			outgoing.pop();
		}
	}
	encoder.encode(toSend);
	{
		char *buffer; size_t size;
		while (encoder.next(buffer, size)) {
			Socket::Status status;
			do {
				status = socket.send(buffer, size);
			} while (status == Socket::NotReady);
			if (status != Socket::Done) return -1;
			LOG(DEBUG) << "Sent packet of size " << size;
		}
	}
	return something;
}

int8_t Client::recvPending() {
	bool something = false;
	Socket::Status status;
	while (true) {
		char buffer[MAX_PACKET_SIZE]; size_t size;
		status = socket.receive(buffer, sizeof buffer, size);
		if (status == Socket::Done) {
			LOG(DEBUG) << "Received packet of size " << size;
			something = true;
			decoder.decode(buffer, size);
			if (decoder.error()) {
				LOG(ERR) << "Received an invalid packet from server";
				return -1;
			}
		} else break;
	}
	if (status != Socket::NotReady) return -1;
	{
		Lock lock(mutexIn);
		Message *message;
		while (decoder.next(message)) {
			incoming.push(*message);
		}
	}
	return something;
}

void Client::send(Message &message) {
	Lock lock(mutexOut);
	outgoing.push(message);
}

bool Client::recv(Message &message) {
	Lock lock(mutexIn);
	if (incoming.empty()) return false;
	message = incoming.front();
	incoming.pop();
	return true;
}
