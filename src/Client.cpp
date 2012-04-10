#include "Protocol.h"

#include "Client.h"

using namespace std;
using namespace sf;

void Client::Run() {
	if (socket.Connect(port, address) != Socket::Done) {
		//TODO: error
		return;
	}
	socket.SetBlocking(false);
	while (!exit_) {
		bool nothing = true;
		int s = sendPending();
		if (s == -1) {
			//TODO: error
			return;
		} else if (s == 1) nothing = false;
		s = recvPending();
		if (s == -1) {
			//TODO: error
			return;
		} else if (s == 1) nothing = false;
		if (nothing) Sleep(0.05f);
	}
}

int Client::sendPending() {
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

int Client::recvPending() {
	bool something = false;
	Socket::Status status;
	while (true) {
		char buffer[MAX_PACKET_SIZE]; size_t size;
		status = socket.Receive(buffer, sizeof buffer, size);
		if (status == Socket::Done) {
			something = true;
			decoder.decode(buffer, size);
			if (decoder.error()) return -1;
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

void Client::send(queue<Message> &messages) {
	Lock lock(mutexOut);
	while (messages.size()) {
		outgoing.push(messages.front());
		messages.pop();
	}
}

void Client::recv(queue<Message> &messages) {
	Lock lock(mutexIn);
	while (!incoming.empty()) {
		messages.push(incoming.front());
		incoming.pop();
	}
}
