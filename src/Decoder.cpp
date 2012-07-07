#include "Protocol.h"

using namespace std;

Decoder::~Decoder() {
	delete current;
	while (data.size()) {
		delete data.front();
		data.pop();
	}
	delete [] incoming;
}

static uint32_t get(char *data, size_t size) {
	uint32_t incoming = 0;
	for (size_t i = 0; i < size; ++i) {
		incoming |= ((uint8_t)data[i])<<(8*i);
	}
	return incoming;
}

#define FAIL() do {error_ = true; delete m; return 0; } while (0)
size_t Decoder::decodeMessage(char *buffer, size_t len) {
	Message *m = new Message;
	uint8_t type = (uint8_t)get(header+2, 1);
	size_t size;
	m->type(type);
	switch (type) {
		case Message::JOIN_REQUEST:
			if (len < 3) FAIL();
			m->version((uint16_t)get(buffer+0, 2));
			size = 3 + get(buffer+2, 1);
			if (len < size) FAIL();
			m->text = string(buffer+3, size-3);
			break;
		case Message::ACTION:
			size = 6; if (len < size) FAIL();
			m->direction((uint16_t)get(buffer+0, 2));
			m->strength(get(buffer+2, 4));
			break;
		case Message::ROUND_CHECKSUM:
			size = 8; if (len < size) FAIL();
			m->round(get(buffer+0, 4));
			m->checksum(get(buffer+4, 4));
			break;
		case Message::JOIN_RESPONSE:
			size = 5; if (len < size) FAIL();
			m->version((uint16_t)get(buffer+0, 2));
			m->accepted((bool)get(buffer+2, 1));
			m->id((uint16_t)get(buffer+3, 2));
			break;
		case Message::GAME_SETTINGS:
			size = 1; if (len < size) FAIL();
			buffer[0] = 0;
			break;
		case Message::SHIP_INFO:
			size = 10; if (len < size) FAIL();
			m->id((uint16_t)get(buffer+0, 2));
			m->x(get(buffer+2, 4));
			m->y(get(buffer+6, 4));
			break;
		case Message::PLANET_INFO:
			size = 18; if (len < size) FAIL();
			m->id((uint16_t)get(buffer+ 0, 2));
			m->x(get(buffer+ 2, 4));
			m->y(get(buffer+ 6, 4));
			m->size(get(buffer+10, 4));
			m->mass(get(buffer+14, 4));
			break;
		case Message::PLAYER_INFO:
			if (len < 4) FAIL();
			m->id((uint16_t)get(buffer+0, 2));
			m->state((uint8_t)get(buffer+2, 1));
			size = 4 + get(buffer+3, 1);
			if (len < size) FAIL();
			m->text = string(buffer+4, size-4);
			break;
		case Message::SCORE_INFO:
			size = 6; if (len < size) FAIL();
			m->id((uint16_t)get(buffer+0, 2));
			m->score(get(buffer+2, 4));
			break;
		case Message::BULLET_INFO:
			size = 20; if (len < size) FAIL();
			m->id((uint16_t)get(buffer+0, 2));
			m->owner((uint16_t)get(buffer+2, 2));
			m->x(get(buffer+4, 4));
			m->y(get(buffer+8, 4));
			m->speedX(get(buffer+12, 4));
			m->speedY(get(buffer+16, 4));
			break;
		case Message::NEW_ROUND:
			size = 4; if (len < size) FAIL();
			m->round(get(buffer+0, 4));
			break;
		case Message::PLAYER_READY:
			size = 2; if (len < size) FAIL();
			m->id((uint16_t)get(buffer+0, 2));
			break;
		case Message::ACTION_INFO:
			size = 8; if (len < size) FAIL();
			m->id((uint16_t)get(buffer+0, 2));
			m->direction((uint16_t)get(buffer+2, 2));
			m->strength(get(buffer+4, 4));
			break;
		default:
			FAIL();
	}
	data.push(m);
	return size;
}
#undef FAIL

void Decoder::decodePacket() {
	size_t count = (size_t)get(header+3, 1);
	size_t at = 0;
	for (size_t i = 0; i < count; ++i) {
		at += decodeMessage(incoming+at, loaded-at);
		if (error_) return;
	}
}

void Decoder::decode(char *buffer, size_t size) {
	size_t i = 0;
	while (headerSize < 4 && i < size) {
		header[headerSize] = buffer[i];
		++headerSize; ++i;
		if (headerSize == 4) {
			expected = get(header, 2);
			if (expected == 0) {
				headerSize = 0;
			}
			incoming = new char[expected];
			loaded = 0;
		}
	}
	if (i >= size) return;
	while (i < size && loaded < expected) {
		incoming[loaded] = buffer[i];
		++loaded; ++i;
		if (loaded == expected) {
			decodePacket();
			if (error_) return;
			delete [] incoming; incoming = 0;
			expected = 0; loaded = 0;
			headerSize = 0;
		}
	}
	if (i >= size) return;
	decode(buffer+i, size-i);
}

bool Decoder::next(Message *&message) {
	bool hasNext = data.size();
	delete current;
	if (hasNext) {
		current = data.front();
		data.pop();
	} else {
		current = 0;
	}
	message = current;
	return hasNext;
}
