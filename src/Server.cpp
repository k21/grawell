#include <utility>

#include "Server.h"

using namespace std;
using namespace sf;

bool Server::isValidName(string name) {
	if (name.size() > 32) return false;
	for (char ch : name) {
		if (ch < 32 || ch > 126) return false;
	}
	return true;
}

void Server::removeShip(uint16_t id) {
	for (Bullet &b : universe.bullets) {
		if (b.owner() == id) b.owner(Message::NO_PLAYER);
	}
	Ship &s = universe.ships[id];
	if (s.alive) placer.remove(s);
	universe.ships.free(s);
}

void Server::accept(ClientInfo &client, const Message &req,
		vector<Message> &toSend) {
	uint16_t id = universe.ships.alloc().id();
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
			if (s.inGame && s.connected && s.ready) {
				Message m = Message::actionInfo(s.id(), s.direction, s.strength);
				toSend.push_back(m);
				s.shoot(universe.bullets);
			}
		}
		Message m = Message::actionInfo(Message::NO_PLAYER, 0, 0);
		toSend.push_back(m);
		sendToAll(toSend);
		list<uint16_t> hits;
		for (size_t i = 0; i < 8192; ++i) {
			universe.update(hits, false, 0);
			for (uint16_t to : hits) {
				placer.remove(universe.ships[to]);
			}
			hits.clear();
			if (universe.bullets.empty()) break;
		}
		state = ROUND;
	} else {
		for (Ship &s : universe.ships) {
			if (s.inGame && !s.connected) {
				removeShip(s.id());
				for (size_t i = 0; i < 2; ++i) {
					Planet &p = *(--universe.planets.end());
					placer.remove(p);
					universe.planets.free(p);
				}
			} else if (!s.inGame && s.connected) {
				s.inGame = true;
				for (size_t i = 0; i < 2; ++i) {
					int32_t size = placer.random(10,70)*FIXED_ONE;
					Planet &p = universe.planets.alloc();
					p.radius = size;
					p.mass = placer.random(size*100, size*200);
					placer.place(p);
				}
			}
			if (s.inGame && !s.alive) {
				placer.place(s);
				s.alive = true;
			}
		}
		vector<Message> toSend;
		toSend.push_back(Message::gameSettings());
		for (Ship &s : universe.ships) {
			if (s.inGame) {
				toSend.push_back(Message::playerInfo(
						s.id(), Message::CONNECTED, s.name));
			}
		}
		for (Ship &s : universe.ships) {
			if (s.inGame) {
				toSend.push_back(Message::scoreInfo(s.id(), s.score));
			}
		}
		for (Planet &p : universe.planets) {
			if (p.active()) {
				toSend.push_back(Message::planetInfo(
						p.id(), p.center.x, p.center.y, p.radius, p.mass));
			}
		}
		for (Ship &s : universe.ships) {
			if (s.inGame) {
				toSend.push_back(Message::shipInfo(s.id(), s.center.x, s.center.y));
			}
		}
		for (Bullet &b : universe.bullets) {
			if (b.active()) {
				toSend.push_back(Message::bulletInfo(b.id(), b.owner(),
						b.center.x, b.center.y, b.speed.x, b.speed.y));
			}
		}
		toSend.push_back(Message::newRound(0));
		sendToAll(toSend);
		state = SELECT_ACTION;
	}
	waitingForCnt = 0;
	for (Ship &s : universe.ships) {
		s.ready = false;
		if (s.connected && s.inGame) ++waitingForCnt;
	}
}

void Server::sendToAll(const vector<Message> &m) {
	for (ClientInfo *c : clients) {
		if (c->active && universe.ships[c->id].inGame) c->encoder.encode(m);
	}
}

int8_t Server::handleMessage(ClientInfo &client, const Message &m) {
	LOG(DEBUG) << "Server received message "
			<< Message::typeToString((Message::Type)m.type());
	if (m.fromServer()) {
		return 1;
	}
	vector<Message> toSend;
	if (!client.active) {
		if (m.type() != Message::JOIN_REQUEST) {
			return 1;
		}
		if (m.version() == PROTOCOL_VERSION && isValidName(m.text)) {
			accept(client, m, toSend);
		} else {
			Message mm = Message::joinResponse(PROTOCOL_VERSION, false, 0);
			toSend.push_back(mm);
		}
	} else {
		if (universe.ships[client.id].ready) return 1;
		if (m.type() == Message::ACTION) {
			if (state != SELECT_ACTION) return 1;
			if (m.direction() >= 36000) return 1;
			if (m.strength() > MAX_STRENGTH) return 1;
			if (m.direction() % 5 != 0) return 1;
			if (m.strength() % 25 != 0) return 1;
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
		LOG(ERR) << "Server cannot listen";
		return;
	}
	serverSocket.SetBlocking(false);
	IPAddress address;
	SocketTCP clientSocket;
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
				LOG(DEBUG) << "Received packet of size " << received;
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
					LOG(DEBUG) << "Sent packet of size " << client.pendingSize;
					client.pending = 0; client.pendingSize = 0;
				} else if (status != Socket::NotReady) {
					error = true;
				}
			}
			if (error) {
				uint16_t id = client.id;
				if (client.active) {
					universe.ships[id].connected = false;
					if (universe.ships[id].inGame) {
						if (!universe.ships[id].ready) --waitingForCnt;
						universe.ships[id].ready = true;
					}
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
