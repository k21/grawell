#include "Server.h"

using namespace std;
using namespace sf;

unsigned short Server::allocID() {
	unsigned short res;
	if (!freeIDs.empty()) {
		set<unsigned short>::iterator it = freeIDs.begin();
		freeIDs.erase(it);
		res = *it;
	} else {
		res = cntIDs;
		++cntIDs;
	}
	return res;
}

void Server::freeID(unsigned short id) {
	freeIDs.insert(id);
	set<unsigned short>::iterator it;
	while ((it = freeIDs.find((unsigned short)(cntIDs-1))) != freeIDs.end()) {
		freeIDs.erase(it);
		--cntIDs;
	}
}

void Server::accept(ClientInfo &client,
		vector<Message> &toSend) {
	++playersCnt;
	client.id = allocID();
	client.state = ClientInfo::ACCEPTED;
	Message m;
	m = Message::joinResponse(PROTOCOL_VERSION, true, client.id);
	toSend.push_back(m);
	//TODO: send environment info
}

void Server::changeState() {
	if (state == SELECT_ACTION) {
		
		state = ROUND;
	} else {
		
		state = SELECT_ACTION;
	}
}

void Server::sendToAll(const vector<Message> &m) {
	for (ClientInfo *client : clients) {
		if (client->state != ClientInfo::NOTHING) client->encoder.encode(m);
	}
}

int Server::handleMessage(ClientInfo &client, const Message &m) {
	if (m.fromServer()) {
		return 1;
	}
	vector<Message> toSend;
	if (client.state == ClientInfo::NOTHING) {
		if (m.type() != Message::JOIN_REQUEST) {
			return 1;
		}
		if (m.version() == PROTOCOL_VERSION) {
			accept(client, toSend);
		} else {
			Message mm = Message::joinResponse(PROTOCOL_VERSION, false, 0);
			toSend.push_back(mm);
		}
	} else if (client.state == ClientInfo::PLAYING) {
		if (client.ready) return 1;
		if (m.type() == Message::ACTION_INFO) {
			if (state != SELECT_ACTION) return 1;
		} else if (m.type() == Message::ROUND_CHECKSUM) {
			if (state != ROUND) return 1;
			if (m.checksum() != checksum) return 1; //TODO: log
		} else return 1;
		client.ready = true;
		++readyCnt;
		if (readyCnt == playersCnt) {
			changeState();
		}
	} else return 1;
	if (!toSend.empty()) client.encoder.encode(toSend);
	return 0;
}

void Server::Run() {
	if (!serverSocket.Listen(port)) {
		//TODO: error
		return;
	}
	serverSocket.SetBlocking(false);
	IPAddress address;
	SocketTCP clientSocket;
	while (!exit_) {
		Socket::Status status = serverSocket.Accept(clientSocket, &address);
		bool nothing = true;
		if (status == Socket::Done) {
			nothing = false;
			clientSocket.SetBlocking(false);
			ClientInfo *client = new ClientInfo(address, clientSocket, -1);
			clients.push_back(client);
		} else if (status != Socket::NotReady) {
			//TODO: error
			return;
		}
		list<ClientInfo *>::iterator it = clients.begin();
		while (it != clients.end()) {
			ClientInfo &client = **it;
			char buffer[MAX_PACKET_SIZE]; size_t received;
			status = client.socket.Receive(buffer, sizeof buffer, received);
			bool error = false;
			if (status == Socket::Done) {
				nothing = false;
				client.decoder.decode(buffer, received);
				if (client.decoder.error()) error = true;
				else {
					Message *m;
					while (client.decoder.next(m)) {
						if (handleMessage(client, *m)) {
							error = true;
							break;
						}
					}
				}
			}
			if (status == Socket::Disconnected || status == Socket::Error) {
				error = true;
			}
			if (error) {
				if (client.state != ClientInfo::NOTHING) {
					if (client.ready) --readyCnt;
					freeID(client.id);
					--playersCnt;
				}
				client.socket.Close();
				delete *it;
				it = clients.erase(it);
				continue;
			}
			++it;
			//TODO: send
		}
		if (nothing) Sleep(0.05f);
	}
	serverSocket.Close();
}
