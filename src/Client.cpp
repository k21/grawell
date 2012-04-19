#include "Protocol.h"

#include "Client.h"

using namespace std;
using namespace boost;
using namespace sf;

void Client::Run() {
	LOG(INFO) << "Connecting to server at " << address << ':' << port << "...";
	static const int16_t RETRY = 10;
	Socket::Status status = socket.Connect(port, address);
	int16_t r = 1;
	while (status != Socket::Done && r < RETRY) {
		LOG(WARN) << "Connection attempt unsuccessful";
		Sleep(1.f);
		LOG(WARN) << "Trying to connect again...";
		status = socket.Connect(port, address);
		++r;
	}
	if (status != Socket::Done) {
		//TODO: error
		LOG(ERROR) << "Could not connect to server";
		return;
	}
	socket.SetBlocking(false);
	while (!exit_) {
		bool nothing = true;
		int8_t s = sendPending();
		if (s == -1) {
			//TODO: error
			LOG(ERROR) << "An error has occured when sending packets";
			return;
		} else if (s == 1) nothing = false;
		s = recvPending();
		if (s == -1) {
			//TODO: error
			LOG(ERROR) << "An error has occured when receiving packets";
			return;
		} else if (s == 1) nothing = false;
		if (nothing) Sleep(0.05f);
	}
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
				status = socket.Send(buffer, size);
			} while (status == Socket::NotReady);
			if (status != Socket::Done) return -1;
		}
	}
	return something;
}

int8_t Client::recvPending() {
	bool something = false;
	Socket::Status status;
	while (true) {
		char buffer[MAX_PACKET_SIZE]; size_t size;
		status = socket.Receive(buffer, sizeof buffer, size);
		if (status == Socket::Done) {
			something = true;
			decoder.decode(buffer, size);
			if (decoder.error()) {
				LOG(ERROR) << "Received an invalid packet from server";
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
