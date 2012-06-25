#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <boost/cstdint.hpp>

class Message {

	typedef boost::int8_t   int8_t;
	typedef boost::int16_t  int16_t;
	typedef boost::int32_t  int32_t;
	typedef boost::uint8_t  uint8_t;
	typedef boost::uint16_t uint16_t;
	typedef boost::uint32_t uint32_t;

public:
	Message(): text() {
		for (size_t i = 0; i < FIELD_COUNT; ++i) data[i] = 0;
	}

	enum Type : uint8_t {
		JOIN_REQUEST = 0,
		ACTION,
		ROUND_CHECKSUM,

		JOIN_RESPONSE = 128,
		GAME_SETTINGS,
		SHIP_INFO,
		PLANET_INFO,
		PLAYER_INFO,
		SCORE_INFO,
		BULLET_INFO,
		NEW_ROUND,
		PLAYER_READY,
		ACTION_INFO,
	};

	enum Field : uint8_t {
		TYPE = 0,
		VERSION,
		ACCEPTED,
		STATE,
		ID,
		X, Y,
		SPEEDX, SPEEDY,
		SIZE, MASS,
		SCORE,
		ROUND,
		DIRECTION, STRENGTH,
		CHECKSUM,
		FIELD_COUNT
	};

	static const uint16_t CONNECTED = 0;
	static const uint16_t DISCONNECTED = 1;
	static const uint16_t CHECKSUM_MISMATCH = 2;

	bool fromServer() const { return type() >= 128; }

	uint8_t       type() const { return (uint8_t)  data[TYPE     ]; }
	uint16_t   version() const { return (uint16_t) data[VERSION  ]; }
	bool      accepted() const { return (bool)     data[ACCEPTED ]; }
	uint8_t      state() const { return (uint8_t)  data[STATE    ]; }
	uint16_t        id() const { return (uint16_t) data[ID       ]; }
	int32_t          x() const { return (int32_t)  data[X        ]; }
	int32_t          y() const { return (int32_t)  data[Y        ]; }
	int32_t     speedX() const { return (int32_t)  data[SPEEDX   ]; }
	int32_t     speedY() const { return (int32_t)  data[SPEEDY   ]; }
	uint32_t      size() const { return (uint32_t) data[SIZE     ]; }
	uint32_t      mass() const { return (uint32_t) data[MASS     ]; }
	int32_t      score() const { return (int32_t)  data[SCORE    ]; }
	uint32_t     round() const { return (uint32_t) data[ROUND    ]; }
	uint16_t direction() const { return (uint16_t) data[DIRECTION]; }
	uint32_t  strength() const { return (uint32_t) data[STRENGTH ]; }
	uint32_t  checksum() const { return (uint32_t) data[CHECKSUM ]; }

	void      type(uint8_t  v) { data[TYPE     ] = v; }
	void   version(uint16_t v) { data[VERSION  ] = v; }
	void  accepted(bool     v) { data[ACCEPTED ] = v; }
	void     state(uint8_t  v) { data[STATE    ] = v; }
	void        id(uint16_t v) { data[ID       ] = v; }
	void         x(int32_t  v) { data[X        ] = v; }
	void         y(int32_t  v) { data[Y        ] = v; }
	void    speedX(int32_t  v) { data[SPEEDX   ] = v; }
	void    speedY(int32_t  v) { data[SPEEDY   ] = v; }
	void      size(uint32_t v) { data[SIZE     ] = v; }
	void      mass(uint32_t v) { data[MASS     ] = v; }
	void     score(int32_t  v) { data[SCORE    ] = v; }
	void     round(uint32_t v) { data[ROUND    ] = v; }
	void direction(uint16_t v) { data[DIRECTION] = v; }
	void  strength(uint32_t v) { data[STRENGTH ] = v; }
	void  checksum(uint32_t v) { data[CHECKSUM ] = v; }

	uint32_t data[FIELD_COUNT];
	std::string text;

	static Message joinRequest(uint16_t version, std::string name) {
		Message m; m.type(JOIN_REQUEST);
		m.version(version); m.text = name;
		return m;
	}
	static Message action(uint16_t direction, uint32_t strength) {
		Message m; m.type(ACTION);
		m.direction(direction); m.strength(strength);
		return m;
	}
	static Message roundChecksum(uint32_t round, uint32_t checksum) {
		Message m; m.type(ROUND_CHECKSUM);
		m.round(round); m.checksum(checksum);
		return m;
	}

	static Message joinResponse(uint16_t version, bool accepted, uint16_t id) {
		Message m; m.type(JOIN_RESPONSE);
		m.version(version); m.accepted(accepted); m.id(id);
		return m;
	}
	static Message gameSettings() {
		Message m; m.type(GAME_SETTINGS);
		return m;
	}
	static Message shipInfo(uint16_t id, int32_t x, int32_t y) {
		Message m; m.type(SHIP_INFO);
		m.id(id); m.x(x); m.y(y);
		return m;
	}
	static Message planetInfo(uint16_t id, int32_t x, int32_t y,
			uint32_t size, uint32_t mass) {
		Message m; m.type(PLANET_INFO);
		m.id(id); m.x(x); m.y(y); m.size(size); m.mass(mass);
		return m;
	}
	static Message playerInfo(uint16_t id, uint8_t state, std::string name) {
		Message m; m.type(PLAYER_INFO);
		m.id(id); m.state(state); m.text = name;
		return m;
	}
	static Message scoreInfo(uint16_t id, int32_t score) {
		Message m; m.type(SCORE_INFO);
		m.id(id); m.score(score);
		return m;
	}
	static Message bulletInfo(uint16_t owner, int32_t x, int32_t y,
			int32_t speedX, int32_t speedY) {
		Message m; m.type(BULLET_INFO);
		m.id(owner); m.x(x); m.y(y); m.speedX(speedX); m.speedY(speedY);
		return m;
	}
	static Message newRound(uint32_t round) {
		Message m; m.type(NEW_ROUND);
		m.round(round);
		return m;
	}
	static Message playerReady(uint16_t id) {
		Message m; m.type(PLAYER_READY);
		m.id(id);
		return m;
	}
	static Message actionInfo(uint16_t id,
			uint16_t direction, uint32_t strength) {
		Message m; m.type(ACTION_INFO);
		m.id(id); m.direction(direction); m.strength(strength);
		return m;
	}

};

#endif
