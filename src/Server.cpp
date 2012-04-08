#include "Server.h"

using namespace std;
using namespace sf;

void Server::Run() {
	if (!serverSocket.Listen(port)) {
		//TODO: error
		return;
	}
	serverSocket.SetBlocking(false);
	IPAddress address;
	SocketTCP clientSocket;
	ClientInfo info;
	while (!exit_) {
		Socket::Status status = serverSocket.Accept(clientSocket, &address);
		bool nothing = true;
		if (status == Socket::Done) {
			nothing = false;
			info.address = address;
			info.socket = clientSocket;
			info.id = 0;
			clients.push_back(info);
		} else if (status != Socket::NotReady) {
			//TODO: error
			return;
		}
		list<ClientInfo>::iterator it = clients.begin();
		while (it != clients.end()) {
			char buffer[MAX_PACKET_SIZE]; size_t received;
			status = it->socket.Receive(buffer, sizeof buffer, received);
			if (status == Socket::Done) {
				nothing = false;
				//TODO: handle message
				++it;
			} else if (status != Socket::NotReady) {
				it->socket.Close();
				it = clients.erase(it);
			} else ++it;
		}
		if (nothing) Sleep(0.05);
	}
	serverSocket.Close();
}
