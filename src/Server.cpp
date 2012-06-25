#include <utility>

#include "Server.h"

using namespace std;
using namespace boost;
using namespace sf;

uint16_t Server::allocID() {
	uint16_t res;
	if (!freeIDs.empty()) {
		auto it = freeIDs.begin();
		res = *it;
		freeIDs.erase(it);
	} else {
		res = cntIDs;
		++cntIDs;
		Ship nShip = Ship(res);
		universe.ships.push_back(nShip);
	}
	return res;
}

void Server::freeID(uint16_t id) {
	Ship &s = universe.ships[id];
	if (s.alive) placer.remove(s);
	freeIDs.insert(id);
	s = Ship(id);
	auto it = freeIDs.end();
	while ((it = freeIDs.find((uint16_t)(cntIDs-1))) != freeIDs.end()) {
		freeIDs.erase(it);
		--cntIDs;
		universe.ships.pop_back();
	}
}

void Server::accept(ClientInfo &client, const Message &req,
		vector<Message> &toSend) {
	uint16_t id = allocID();
	client.id = id;
	client.active = true;
	universe.ships[id].name = req.text;
	universe.ships[id].connected = true;
	Message m;
	m = Message::joinResponse(PROTOCOL_VERSION, true, client.id);
	toSend.push_back(m);
}

void Server::changeState() {
	if (state == SELECT_ACTION) {
		vector<Message> toSend;
		for (Ship &s : universe.ships) {
			if (s.active && s.connected && s.ready) {
				Message m = Message::actionInfo(s.id, s.direction, s.strength);
				toSend.push_back(m);
				universe.bullets.push_back(s.shoot());
			}
		}
		Message m = Message::actionInfo(65535, 0, 0);
		toSend.push_back(m);
		sendToAll(toSend);
		list<pair<uint16_t, uint16_t>> hits;
		for (size_t i = 0; i < 8192; ++i) {
			universe.update(hits);
			if (universe.bullets.empty()) break;
		}
		for (pair<uint16_t, uint16_t> p : hits) {
			uint16_t from = p.first, to = p.second;
			++universe.ships[from].score;
			universe.ships[to].alive = false;
			placer.remove(universe.ships[to]);
		}
		state = ROUND;
	} else {
		for (Ship &s : universe.ships) {
			if (s.active && !s.connected) {
				freeID(s.id);
			} else if (!s.active && s.connected) {
				s.active = true;
			}
			if (s.active && !s.alive) {
				placer.place(s);
				s.alive = true;
			}
		}
		vector<Message> toSend;
		toSend.push_back(Message::gameSettings());
		for (Ship &s : universe.ships) {
			if (s.active) {
				toSend.push_back(Message::playerInfo(
						s.id, Message::CONNECTED, s.name));
			}
		}
		for (Ship &s : universe.ships) {
			if (s.active) {
				toSend.push_back(Message::scoreInfo(s.id, s.score));
			}
		}
		uint16_t pi = 0;
		for (Planet &p : universe.planets) {
			toSend.push_back(Message::planetInfo(
					pi, p.center.x, p.center.y, p.radius, p.mass));
			++pi;
		}
		for (Ship &s : universe.ships) {
			if (s.active) {
				toSend.push_back(Message::shipInfo(s.id, s.center.x, s.center.y));
			}
		}
		//TODO: send bullets
		toSend.push_back(Message::newRound(0));
		sendToAll(toSend);
		state = SELECT_ACTION;
	}
	waitingForCnt = 0;
	for (Ship &s : universe.ships) {
		s.ready = false;
		if (s.connected && s.active) ++waitingForCnt;
	}
}

void Server::sendToAll(const vector<Message> &m) {
	for (ClientInfo *c : clients) {
		if (c->active && universe.ships[c->id].active) c->encoder.encode(m);
	}
}

int8_t Server::handleMessage(ClientInfo &client, const Message &m) {
	LOG(DEBUG) << "Server received packet type " << (uint16_t)m.type();
	if (m.fromServer()) {
		return 1;
	}
	vector<Message> toSend;
	if (!client.active) {
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
		--waitingForCnt;
	}
	if (!toSend.empty()) client.encoder.encode(toSend);
	return 0;
}

void Server::Run() {
	LOG(INFO) << "Starting server";
	if (!serverSocket.Listen(port)) {
		//TODO: error
		LOG(ERR) << "Server cannot listen";
		return;
	}
	serverSocket.SetBlocking(false);
	IPAddress address;
	SocketTCP clientSocket;
	for (size_t i = 0; i < 10; ++i) {
		int32_t size = (uint32_t)(rand()%60+10)*FIXED_ONE;
		universe.planets.push_back(Planet(Point(0,0),size,256000000));
		placer.place(universe.planets.back());
	}
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
			LOG(ERR) << "An error occured when waiting for client";
			return;
		}
		auto it = clients.begin();
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
				uint16_t id = client.id;
				if (client.active && universe.ships[id].active) {
					if (!universe.ships[id].ready) --waitingForCnt;
					universe.ships[id].connected = false;
					universe.ships[id].ready = true;
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
		//TODO: only change state if someone is connected
		if (waitingForCnt == 0) {
			changeState();
		}
		if (nothing) Sleep(0.05f);
	}
	serverSocket.Close();
}
