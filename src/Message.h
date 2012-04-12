#ifndef MESSAGE_H_
#define MESSAGE_H_

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

	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned long ulong;

	ushort      type() const { return (ushort)data[TYPE     ]; }
	ushort   version() const { return (ushort)data[VERSION  ]; }
	bool    accepted() const { return (bool)  data[ACCEPTED ]; }
	ushort        id() const { return (ushort)data[ID       ]; }
	long           x() const { return (long)  data[X        ]; }
	long           y() const { return (long)  data[Y        ]; }
	ulong       size() const { return (ulong) data[SIZE     ]; }
	ulong       mass() const { return (ulong) data[MASS     ]; }
	long       score() const { return (long)  data[SCORE    ]; }
	ulong      round() const { return (ulong) data[ROUND    ]; }
	ushort direction() const { return (ushort)data[DIRECTION]; }
	ulong   strength() const { return (ulong) data[STRENGTH ]; }
	ulong   checksum() const { return (ulong) data[CHECKSUM ]; }

	void      type(ushort v) { data[TYPE     ] = v; }
	void   version(ushort v) { data[VERSION  ] = v; }
	void  accepted(bool   v) { data[ACCEPTED ] = v; }
	void        id(ushort v) { data[ID       ] = v; }
	void         x(long   v) { data[X        ] = v; }
	void         y(long   v) { data[Y        ] = v; }
	void      size(ulong  v) { data[SIZE     ] = v; }
	void      mass(ulong  v) { data[MASS     ] = v; }
	void     score(long   v) { data[SCORE    ] = v; }
	void     round(ulong  v) { data[ROUND    ] = v; }
	void direction(ushort v) { data[DIRECTION] = v; }
	void  strength(ulong  v) { data[STRENGTH ] = v; }
	void  checksum(ulong  v) { data[CHECKSUM ] = v; }

	long data[FIELD_COUNT];
	std::string text;

	static Message joinRequest(ushort version, std::string name) {
		Message m; m.type(JOIN_REQUEST);
		m.version(version); m.text = name;
		return m;
	}
	static Message action(ushort direction, ulong strength) {
		Message m; m.type(ACTION);
		m.direction(direction); m.strength(strength);
		return m;
	}
	static Message roundChecksum(ulong round, ulong checksum) {
		Message m; m.type(ROUND_CHECKSUM);
		m.round(round); m.checksum(checksum);
		return m;
	}

	static Message joinResponse(ushort version, bool accepted, ushort id) {
		Message m; m.type(JOIN_RESPONSE);
		m.version(version); m.accepted(accepted); m.id(id);
		return m;
	}
	static Message gameSettings() {
		Message m; m.type(GAME_SETTINGS);
		return m;
	}
	static Message shipInfo(ushort id, long x, long y) {
		Message m; m.type(SHIP_INFO);
		m.id(id); m.x(x); m.y(y);
		return m;
	}
	static Message planetInfo(ushort id, long x, long y, ulong size, ulong mass) {
		Message m; m.type(PLANET_INFO);
		m.id(id); m.x(x); m.y(y); m.size(size); m.mass(mass);
		return m;
	}
	static Message playerInfo(ushort id, std::string name) {
		Message m; m.type(PLAYER_INFO);
		m.id(id); m.text = name;
		return m;
	}
	static Message scoreInfo(ushort id, long score) {
		Message m; m.type(SCORE_INFO);
		m.id(id); m.score(score);
		return m;
	}
	static Message newRound(ulong round) {
		Message m; m.type(NEW_ROUND);
		m.round(round);
		return m;
	}
	static Message playerReady(ushort id) {
		Message m; m.type(PLAYER_READY);
		m.id(id);
		return m;
	}
	static Message actionInfo(ushort id, ushort direction, ulong strength) {
		Message m; m.type(ACTION_INFO);
		m.id(id); m.direction(direction); m.strength(strength);
		return m;
	}
	static Message checksumMismatch(ushort id) {
		Message m; m.type(CHECKSUM_MISMATCH);
		m.id(id);
		return m;
	}

};

#endif
