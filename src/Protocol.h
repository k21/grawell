#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <map>
#include <string>
#include <queue>
#include <utility>
#include <vector>

static const size_t MAX_PACKET_SIZE = 1024;
static const unsigned short PROTOCOL_VERSION_DEV = 65535;
static const unsigned short PROTOCOL_VERSION = PROTOCOL_VERSION_DEV;

#include "Message.h"

class Encoder {

public:
	Encoder(): currentSize(0), current(0), lastSize(0), last(0), data() {}
	~Encoder();

	void encode(const std::vector<Message> &messages);
	bool next(char *&buffer, size_t &size);

private:
	Encoder(const Encoder &);
	Encoder& operator = (const Encoder &);

	char *encode(const Message &m, size_t &size);
	void append(const char *what, size_t size);
	void append(unsigned long what, size_t size);

	size_t currentSize;
	char *current;
	size_t lastSize;
	char *last;
	std::queue<std::pair<size_t, char *> > data;

};

class Decoder {

public:
	Decoder(): header(), headerSize(0), expected(0), loaded(0), incoming(0),
			current(0), data(), error_(false) {}
	~Decoder();
	void decode(char *buffer, size_t size);
	bool error() const { return error_; }
	bool next(Message *&message);

private:
	Decoder(const Decoder &);
	Decoder& operator = (const Decoder &);

	size_t decodeMessage(char *buffer, size_t len);
	void decodePacket();

	char header[4]; size_t headerSize;
	size_t expected, loaded;
	char *incoming;
	Message *current;
	std::queue<Message *> data;
	bool error_;

};

#endif
