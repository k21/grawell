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
		Ship nShip = Ship(res);
		universe.ships.push_back(nShip);
	}
	return res;
}

void Server::freeID(unsigned short id) {
	freeIDs.insert(id);
	universe.ships[id].active = false;
	set<unsigned short>::iterator it;
	while ((it = freeIDs.find((unsigned short)(cntIDs-1))) != freeIDs.end()) {
		freeIDs.erase(it);
		--cntIDs;
		universe.ships.pop_back();
	}
}

void Server::accept(ClientInfo &client, const Message &req,
		vector<Message> &toSend) {
	++playersCnt;
	++readyCnt;
	client.id = allocID();
	universe.ships[client.id].name = req.text;
	Message m;
	m = Message::playerInfo(client.id, Message::CONNECTED, req.text);
	sendToAll(vector<Message>(1,m));
	client.state = ClientInfo::ACCEPTED;
	m = Message::joinResponse(PROTOCOL_VERSION, true, client.id);
	toSend.push_back(m);
	m = Message::gameSettings();
	toSend.push_back(m);
	for (size_t i = 0; i < universe.ships.size(); ++i) {
		if (!universe.ships[i].active) continue;
		m = Message::playerInfo(
				(unsigned short)i,
				Message::CONNECTED,
				universe.ships[i].name
				);
		toSend.push_back(m);
	}
	for (size_t i = 0; i < universe.ships.size(); ++i) {
		if (!universe.ships[i].active) continue;
		m = Message::scoreInfo((unsigned short)i, universe.ships[i].score);
		toSend.push_back(m);
	}
	for (size_t pi = 0; pi < universe.planets.size(); ++pi) {
		m = Message::planetInfo((unsigned short)pi,
				(long)(universe.planets[pi].center.x*1024),
				(long)(universe.planets[pi].center.y*1024),
				(long)universe.planets[pi].radius,
				(long)universe.planets[pi].mass
				);
		toSend.push_back(m);
	}
	for (size_t i = 0; i < universe.ships.size(); ++i) {
		if (!universe.ships[i].active) continue;
		m = Message::shipInfo((unsigned short)i,
				(long)(universe.ships[i].center.x*1024),
				(long)(universe.ships[i].center.y*1024)
				);
		toSend.push_back(m);
	}
}

void Server::changeState() {
	for (Ship &s : universe.ships) {
		s.ready = false;
	}
	readyCnt = 0;
	if (state == SELECT_ACTION) {
		vector<Message> toSend;
		for (ClientInfo *c : clients) {
			if (c->state != ClientInfo::PLAYING) continue;
			Message m = Message::actionInfo(c->id,
					universe.ships[c->id].direction,
					universe.ships[c->id].strength
					);
			toSend.push_back(m);
		}
		sendToAll(toSend);
		for (Ship &s : universe.ships) {
			if (!s.active) continue;
			universe.bullets.push_back(s.shoot());
		}
		list<unsigned short> destroyed;
		for (size_t i = 0; i < 8192; ++i) {
			universe.update(destroyed, 1.0/1024);
			if (universe.bullets.empty()) break;
		}
		for (unsigned short i : destroyed) {
			placer.remove(universe.ships[i]);
			placer.place(universe.ships[i]);
			Message m = Message::shipInfo(i,
					(long)(universe.ships[i].center.x*1024),
					(long)(universe.ships[i].center.y*1024)
					);
			roundEnd.push_back(m);
		}
		state = ROUND;
	} else {
		//TODO: check checksums
		for (ClientInfo *c : clients) {
			if (c->state == ClientInfo::ACCEPTED) {
				c->state = ClientInfo::PLAYING;
				universe.ships[c->id].active = true;
				placer.place(universe.ships[c->id]);
				Message m = Message::shipInfo(c->id,
						(long)(universe.ships[c->id].center.x*1024),
						(long)(universe.ships[c->id].center.y*1024)
						);
				roundEnd.push_back(m);
			}
		}
		for (Ship &s : universe.ships) {
			if (s.deactivate) {
				s.active = false;
				s.deactivate = false;
			}
		}
		Message m = Message::newRound(0);
		roundEnd.push_back(m);
		sendToAll(roundEnd);
		roundEnd.clear();
		state = SELECT_ACTION;
	}
}

void Server::sendToAll(const vector<Message> &m) {
	for (ClientInfo *client : clients) {
		if (client->state != ClientInfo::NOTHING) client->encoder.encode(m);
	}
}

int Server::handleMessage(ClientInfo &client, const Message &m) {
	LOG(DEBUG) << "Server received packet type " << m.type();
	if (m.fromServer()) {
		return 1;
	}
	vector<Message> toSend;
	if (client.state == ClientInfo::NOTHING) {
		if (m.type() != Message::JOIN_REQUEST) {
			return 1;
		}
		if (m.version() == PROTOCOL_VERSION) {
			accept(client, m, toSend);
		} else {
			Message mm = Message::joinResponse(PROTOCOL_VERSION, false, 0);
			toSend.push_back(mm);
		}
	} else {
		if (universe.ships[client.id].ready) return 1;
		if (m.type() == Message::ACTION) {
			if (state != SELECT_ACTION) return 1;
			universe.ships[client.id].direction = m.direction();
			universe.ships[client.id].strength = m.strength();
		} else if (m.type() == Message::ROUND_CHECKSUM) {
			if (state != ROUND) return 1;
			if (m.checksum() != checksum) {
				LOG(WARN) << "Client " << m.id() << " is out of sync";
				return 1;
			}
		} else return 1;
		universe.ships[client.id].ready = true;
		++readyCnt;
	}
	if (!toSend.empty()) client.encoder.encode(toSend);
	return 0;
}

void Server::Run() {
	LOG(INFO) << "Starting server";
	if (!serverSocket.Listen(port)) {
		//TODO: error
		LOG(ERROR) << "Server cannot listen";
		return;
	}
	serverSocket.SetBlocking(false);
	IPAddress address;
	SocketTCP clientSocket;
	universe.planets.push_back(Planet(Point(0,0),40,8000000));
	placer.place(universe.planets.back());
	while (!exit_) {
		Socket::Status status = serverSocket.Accept(clientSocket, &address);
		bool nothing = true;
		if (status == Socket::Done) {
			LOG(INFO) << "Client connected from " << address;
			nothing = false;
			clientSocket.SetBlocking(false);
			ClientInfo *client = new ClientInfo(address, clientSocket, -1);
			clients.push_back(client);
		} else if (status != Socket::NotReady) {
			//TODO: error
			LOG(ERROR) << "An error occured when waiting for client";
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
			if (!error && !client.pending) {
				if (!client.encoder.next(client.pending, client.pendingSize)) {
					client.pending = 0;
				}
			}
			if (!error && client.pending) {
				status = client.socket.Send(client.pending, client.pendingSize);
				if (status == Socket::Done) {
					client.pending = 0; client.pendingSize = 0;
				} else if (status != Socket::NotReady) {
					error = true;
				}
			}
			if (error) {
				unsigned short id = client.id;
				if (client.state != ClientInfo::NOTHING) {
					if (universe.ships[id].ready) --readyCnt;
					universe.ships[id].deactivate = true;
					freeID(id);
					--playersCnt;
				}
				client.socket.Close();
				delete *it;
				it = clients.erase(it);
				sendToAll(vector<Message>(
						1, Message::playerInfo(id, Message::DISCONNECTED, "")));
				continue;
			}
			++it;
		}
		if (readyCnt == playersCnt && playersCnt) {
			changeState();
		}
		if (nothing) Sleep(0.05f);
	}
	serverSocket.Close();
}
