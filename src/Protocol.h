#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <map>
#include <string>
#include <queue>
#include <utility>
#include <vector>

const unsigned int MAX_PACKET_SIZE = 1024;

class Message {

public:
	Message(): text() {
		for (size_t i = 0; i < FIELD_COUNT; ++i) data[i] = 0;
	}

	enum Type : unsigned char {
		JOIN_REQUEST = 0,
		ACTION,
		ROUND_CHECKSUM,

		JOIN_RESPONSE = 128,
		GAME_SETTINGS,
		SHIP_INFO,
		PLANET_INFO,
		PLAYER_INFO,
		SCORE_INFO,
		NEW_ROUND,
		PLAYER_READY,
		ACTION_INFO,
		CHECKSUM_MISMATCH,
	};

	enum Field : unsigned char {
		TYPE = 0,
		VERSION,
		ACCEPTED,
		ID,
		X, Y,
		SIZE, MASS,
		SCORE,
		ROUND,
		DIRECTION, STRENGTH,
		CHECKSUM,
		FIELD_COUNT
	};

	bool fromServer() const { return type() >= 128; }

	short      type() const { return (short)data[TYPE     ]; }
	short   version() const { return (short)data[VERSION  ]; }
	bool   accepted() const { return (bool) data[ACCEPTED ]; }
	short        id() const { return (short)data[ID       ]; }
	long          x() const { return (long) data[X        ]; }
	long          y() const { return (long) data[Y        ]; }
	long       size() const { return (long) data[SIZE     ]; }
	long       mass() const { return (long) data[MASS     ]; }
	long      score() const { return (long) data[SCORE    ]; }
	long      round() const { return (long) data[ROUND    ]; }
	short direction() const { return (short)data[DIRECTION]; }
	long   strength() const { return (long) data[STRENGTH ]; }
	unsigned long
	       checksum() const { return (unsigned long)data[CHECKSUM]; }

	void      type(short v) { data[TYPE     ] = v; }
	void   version(short v) { data[VERSION  ] = v; }
	void  accepted(bool  v) { data[ACCEPTED ] = v; }
	void        id(short v) { data[ID       ] = v; }
	void         x(long  v) { data[X        ] = v; }
	void         y(long  v) { data[Y        ] = v; }
	void      size(long  v) { data[SIZE     ] = v; }
	void      mass(long  v) { data[MASS     ] = v; }
	void     score(long  v) { data[SCORE    ] = v; }
	void     round(long  v) { data[ROUND    ] = v; }
	void direction(short v) { data[DIRECTION] = v; }
	void  strength(long  v) { data[STRENGTH ] = v; }
	void  checksum(unsigned long v) { data[CHECKSUM] = v; }

	long data[FIELD_COUNT];
	std::string text;

};

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
	Decoder(): current(0), data(), error_(false) {}
	~Decoder();
	void decode(char *buffer, size_t size);
	bool error() const { return error_; }
	bool next(Message *&message);

private:
	Decoder(const Decoder &);
	Decoder& operator = (const Decoder &);

	Message *current;
	std::queue<Message *> data;
	bool error_;

};

#endif
