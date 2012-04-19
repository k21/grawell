#include "Protocol.h"

using namespace std;
using namespace boost;

Encoder::~Encoder() {
	delete [] current;
	delete [] last;
	while (data.size()) {
		delete data.front().second;
		data.pop();
	}
}

static void set(char *dest, uint32_t data, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		dest[i] = (char)(data & 0xFF);
		data >>= 8;
	}
}

char *Encoder::encode(const Message &m, size_t &size) {
	char *res = 0;
	switch (m.type()) {
		case Message::JOIN_REQUEST:
			size = 3 + m.text.size(); res = new char[size];
			set(res+0, m.version(), 2);
			set(res+2, m.text.size(), 1);
			for (size_t i = 0; i < (m.text.size()&0xFF); ++i) res[3+i] = m.text[i];
			break;
		case Message::ACTION:
			size = 6; res = new char[size];
			set(res+0, m.direction(), 2);
			set(res+2, m.strength(), 4);
			break;
		case Message::ROUND_CHECKSUM:
			size = 8; res = new char[size];
			set(res+0, m.round(), 4);
			set(res+4, m.checksum(), 4);
			break;
		case Message::JOIN_RESPONSE:
			size = 5; res = new char[size];
			set(res+0, m.version(), 2);
			set(res+2, m.accepted(), 1);
			set(res+3, m.id(), 2);
			break;
		case Message::GAME_SETTINGS:
			size = 1; res = new char[size];
			res[0] = 0;
			break;
		case Message::SHIP_INFO:
			size = 10; res = new char[size];
			set(res+0, m.id(), 2);
			set(res+2, m.x(), 4);
			set(res+6, m.y(), 4);
			break;
		case Message::PLANET_INFO:
			size = 18; res = new char[size];
			set(res+ 0, m.id(), 2);
			set(res+ 2, m.x(), 4);
			set(res+ 6, m.y(), 4);
			set(res+10, m.size(), 4);
			set(res+14, m.mass(), 4);
			break;
		case Message::PLAYER_INFO:
			size = 4 + m.text.size(); res = new char[size];
			set(res+0, m.id(), 2);
			set(res+2, m.state(), 1);
			set(res+3, m.text.size(), 1);
			for (size_t i = 0; i < (m.text.size()&0xFF); ++i) res[4+i] = m.text[i];
			break;
		case Message::SCORE_INFO:
			size = 6; res = new char[size];
			set(res+0, m.id(), 2);
			set(res+2, m.score(), 4);
			break;
		case Message::NEW_ROUND:
			size = 4; res = new char[size];
			set(res+0, m.round(), 4);
			break;
		case Message::PLAYER_READY:
			size = 2; res = new char[size];
			set(res+0, m.id(), 2);
			break;
		case Message::ACTION_INFO:
			size = 8; res = new char[size];
			set(res+0, m.id(), 2);
			set(res+2, m.direction(), 2);
			set(res+4, m.strength(), 4);
			break;
	}
	return res;
}

void Encoder::append(const char *what, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		if (last && lastSize >= MAX_PACKET_SIZE) {
			data.push(make_pair(lastSize, last));
			last = 0; lastSize = 0;
		}
		if (!last) {
			last = new char[MAX_PACKET_SIZE];
			lastSize = 0;
		}
		last[lastSize] = what[i];
		++lastSize;
	}
}

void Encoder::append(uint32_t what, size_t size) {
	char c[size];
	for (size_t i = 0; i < size; ++i) {
		c[i] = (char)(what & 0xFF);
		what >>= 8;
	}
	append(c, size);
}

void Encoder::encode(const std::vector<Message> &messages) {
	size_t n = messages.size();
	if (!n) return;
	char *enc[n];
	size_t sizes[n];
	for (size_t i = 0; i < n; ++i) {
		enc[i] = encode(messages[i], sizes[i]);
	}
	uint8_t type = messages[0].type();
	size_t begin = 0, end = 1;
	size_t tlen = sizes[0];
	do {
		while (end < n && end-begin < 255 && messages[end].type() == type) {
			tlen += sizes[end];
			++end;
		}
		append(tlen, 2);
		append(type, 1);
		append(end-begin, 1);
		for (size_t i = begin; i < end; ++i) {
			append(enc[i], sizes[i]);
			delete [] enc[i];
		}
		begin = end;
		if (begin < n) type = messages[begin].type();
		tlen = 0;
	} while (end < n);
}

bool Encoder::next(char *&buffer, size_t &size) {
	bool hasNext = data.size();
	if (!hasNext && last) {
		data.push(make_pair(lastSize, last));
		last = 0; lastSize = 0;
		hasNext = true;
	}
	delete [] current;
	if (hasNext) {
		currentSize = data.front().first;
		current = data.front().second;
		data.pop();
	} else {
		currentSize = 0;
		current = 0;
	}
	buffer = current; size = currentSize;
	return hasNext;
}
