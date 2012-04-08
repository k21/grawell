#include "Protocol.h"

using namespace std;

Decoder::~Decoder() {
	//TODO
}

void Decoder::decode(char *buffer, size_t size) {
	//TODO
}

bool Decoder::next(Message *&message) {
	bool hasNext = data.size();
	delete current;
	if (hasNext) {
		current = data.front();
		data.pop();
	} else {
		current = 0;
		return false;
	}
	message = current;
	return hasNext;
}
