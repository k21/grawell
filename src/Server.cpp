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
	freeIDs.insert(id);
	universe.ships[id] = Ship(id);
	auto it = freeIDs.end();
	while ((it = freeIDs.find((uint16_t)(cntIDs-1))) != freeIDs.end()) {
		freeIDs.erase(it);
		--cntIDs;
		universe.ships.pop_back();
	}
}

void Server::accept(ClientInfo &client, const Message &req,
		vector<Message> &toSend) {
	++playersCnt;
	client.id = allocID();
	Message m;
	m = Message::playerInfo(client.id, Message::CONNECTED, req.text);
	sendToAll(vector<Message>(1,m));
	client.hasShip = true;
	universe.ships[client.id].name = req.text;
	universe.ships[client.id].hasClient = true;
	if (state == SELECT_ACTION) {
		++readyCnt;
		universe.ships[client.id].ready = true;
	}
	m = Message::joinResponse(PROTOCOL_VERSION, true, client.id);
	toSend.push_back(m);
	m = Message::gameSettings();
	toSend.push_back(m);
	for (Ship &s : universe.ships) {
		if ((s.hasClient || s.visible) && s.id != client.id) {
			m = Message::playerInfo(s.id, Message::CONNECTED, s.name);
			toSend.push_back(m);
		}
	}
	for (Ship &s : universe.ships) {
		if ((s.hasClient || s.visible) && s.id != client.id) {
			m = Message::scoreInfo(s.id, s.score);
			toSend.push_back(m);
		}
	}
	for (size_t pi = 0; pi < universe.planets.size(); ++pi) {
		m = Message::planetInfo((uint16_t)pi,
				universe.planets[pi].center.x,
				universe.planets[pi].center.y,
				universe.planets[pi].radius,
				universe.planets[pi].mass
				);
		toSend.push_back(m);
	}
	for (Ship &s : universe.ships) {
		if ((s.hasClient || s.visible) && s.id != client.id) {
			m = Message::shipInfo(s.id, s.center.x, s.center.y);
		}
		toSend.push_back(m);
	}
	m = Message::newRound(0);
	toSend.push_back(m);
	for (Ship &s : universe.ships) {
		if (s.leftBeforeRound) {
			m = Message::playerInfo(s.id, Message::DISCONNECTED, "");
			toSend.push_back(m);
		}
	}
	if (state == ROUND) {
		for (Ship &s : universe.ships) {
			if ((s.visible || s.hasClient) && !s.leftBeforeRound) {
				//TODO: bug - shot, disconnected, but his shot still flies
				m = Message::actionInfo(s.id, s.direction, s.strength);
				toSend.push_back(m);
			}
		}
		for (Ship &s : universe.ships) {
			if (s.leftDuringRound) {
				m = Message::playerInfo(s.id, Message::DISCONNECTED, "");
				toSend.push_back(m);
			}
		}
	}
}

void Server::changeState() {
	for (Ship &s : universe.ships) {
		s.ready = false;
	}
	readyCnt = 0;
	if (state == SELECT_ACTION) {
		vector<Message> toSend;
		for (Ship &s : universe.ships) {
			if (s.hasClient && s.visible) {
				Message m = Message::actionInfo(s.id, s.direction, s.strength);
				toSend.push_back(m);
				universe.bullets.push_back(s.shoot());
			}
		}
		sendToAll(toSend);
		list<pair<uint16_t, uint16_t>> hits;
		for (size_t i = 0; i < 8192; ++i) {
			universe.update(hits);
			if (universe.bullets.empty()) break;
		}
		for (pair<uint16_t, uint16_t> p : hits) {
			uint16_t from = p.first, to = p.second;
			++universe.ships[from].score;
			universe.ships[to].visible = false;
			placer.remove(universe.ships[to]);
		}
		state = ROUND;
	} else {
		//TODO: check checksums
		for (Ship &s : universe.ships) {
			if (s.leftBeforeRound || s.leftDuringRound) {
				freeID(s.id);
			}
		}
		vector<Message> toSend;
		for (Ship &s : universe.ships) {
			if (s.hasClient && !s.visible) {
				placer.place(s);
				s.visible = true;
				Message m = Message::shipInfo(s.id, s.center.x, s.center.y);
				toSend.push_back(m);
			}
		}
		Message m = Message::newRound(0);
		toSend.push_back(m);
		sendToAll(toSend);
		state = SELECT_ACTION;
	}
}

void Server::sendToAll(const vector<Message> &m) {
	for (ClientInfo *client : clients) {
		if (client->hasShip) client->encoder.encode(m);
	}
}

int8_t Server::handleMessage(ClientInfo &client, const Message &m) {
	LOG(DEBUG) << "Server received packet type " << (uint16_t)m.type();
	if (m.fromServer()) {
		return 1;
	}
	vector<Message> toSend;
	if (!client.hasShip) {
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
				if (client.hasShip) {
					if (universe.ships[id].ready) --readyCnt;
					if (state == SELECT_ACTION) {
						universe.ships[id].leftBeforeRound = true;
					} else {
						universe.ships[id].leftDuringRound = true;
					}
					--playersCnt;
					universe.ships[id].hasClient = false;
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
