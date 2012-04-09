#include "gtest/gtest.h"

#include "../src/Protocol.h"

using namespace std;

class EncoderTest : public ::testing::Test {
public:
	EncoderTest(): encoder(), m1(), m2(), v1(), v2(), v3(), v4(), v5(),
			buffer(0), size(-1) {}

	Encoder encoder;
	Message m1, m2;
	vector<Message> v1, v2, v3, v4, v5;
	char *buffer; size_t size;

	virtual void SetUp() {
		m1.type(Message::ACTION);
		m1.direction(4500);
		m1.strength(15000);
		m2.type(Message::SHIP_INFO);
		m2.id(7);
		m2.x(123);
		m2.y(456);
		v1.push_back(m1);
		for (size_t i = 0; i < 256; ++i) {
			v2.push_back(m1);
			v2.push_back(m2);
		}
		for (size_t i = 0; i < 255; ++i) {
			v3.push_back(m2);
		}
		for (size_t i = 0; i < 256; ++i) {
			v4.push_back(m2);
		}
		for (size_t i = 0; i < 128; ++i) {
			v5.push_back(m1);
			v5.push_back(m1);
			v5.push_back(m2);
		}
		buffer = 0; size = -1;
	}

	bool next() {
		buffer = 0; size = -1;
		return encoder.next(buffer, size);
	}

	unsigned char at(size_t i) const { return buffer[i]; }
	unsigned long get(size_t begin, size_t len) const {
		unsigned long res = 0;
		for (size_t i = 0; i < len; ++i) {
			res |= at(begin+i)<<(8*i);
		}
		return res;
	}

	void loadMultiple(size_t expectedSize);
	void checkM1Result(size_t cnt);

private:
	EncoderTest(const EncoderTest &);
	const EncoderTest& operator = (const EncoderTest &);

};

void EncoderTest::loadMultiple(size_t expectedSize) {
	char *b = new char[expectedSize];
	size_t loaded = 0;
	while (loaded < expectedSize) {
		ASSERT_TRUE(next());
		ASSERT_TRUE(buffer);
		ASSERT_EQ(min(expectedSize-loaded, MAX_PACKET_SIZE), size);
		for (size_t i = 0; i < size; ++i) {
			b[loaded+i] = buffer[i];
		}
		loaded += size;
	}
	ASSERT_EQ(expectedSize, loaded);
	ASSERT_FALSE(next());
	buffer = b;
}

void EncoderTest::checkM1Result(size_t cnt) {
	loadMultiple(10*cnt);
	for (size_t i = 0; i < cnt; ++i) {
		size_t begin = 10*i;
		EXPECT_EQ(6, get(begin+0,2));
		EXPECT_EQ(Message::ACTION, get(begin+2,1));
		EXPECT_EQ(1, get(begin+3,1));
		EXPECT_EQ(4500, get(begin+4,2));
		EXPECT_EQ(15000, get(begin+6,4));
	}
	delete [] buffer;
	buffer = 0; size = -1;
}

TEST_F(EncoderTest, SingleMessage) {
	encoder.encode(v1);
	checkM1Result(1);
}

TEST_F(EncoderTest, SingleMessageRepeated) {
	for (size_t i = 0; i < 1024; ++i) {
		encoder.encode(v1);
		checkM1Result(1);
	}
}

TEST_F(EncoderTest, SingleMessageJoin) {
	for (size_t i = 0; i < 10; ++i) {
		encoder.encode(v1);
	}
	checkM1Result(10);
}

TEST_F(EncoderTest, SingleMessageJoinMultiplePackets) {
	for (size_t i = 0; i < 1024; ++i) {
		encoder.encode(v1);
	}
	checkM1Result(1024);
}

TEST_F(EncoderTest, MultipleDifferentMessages) {
	encoder.encode(v2);
	loadMultiple(256*(10+14));
	for (size_t i = 0; i < 256; ++i) {
		size_t begin = (10+14)*i;
		EXPECT_EQ(6, get(begin+0,2));
		EXPECT_EQ(Message::ACTION, get(begin+2,1));
		EXPECT_EQ(1, get(begin+3,1));
		EXPECT_EQ(4500, get(begin+4,2));
		EXPECT_EQ(15000, get(begin+6,4));
		EXPECT_EQ(10, get(begin+10,2));
		EXPECT_EQ(Message::SHIP_INFO, get(begin+12,1));
		EXPECT_EQ(1, get(begin+13,1));
		EXPECT_EQ(7, get(begin+14,2));
		EXPECT_EQ(123, get(begin+16,4));
		EXPECT_EQ(456, get(begin+20,4));
	}
	delete [] buffer;
	buffer = 0; size = -1;
}

TEST_F(EncoderTest, MultipleMergedMessages) {
	encoder.encode(v3);
	loadMultiple(4+255*10);
	EXPECT_EQ(255*10, get(0,2));
	EXPECT_EQ(Message::SHIP_INFO, get(2,1));
	EXPECT_EQ(255, get(3,1));
	for (size_t i = 0; i < 255; ++i) {
		size_t begin = 4+i*10;
		EXPECT_EQ(7, get(begin+0,2));
		EXPECT_EQ(123, get(begin+2,4));
		EXPECT_EQ(456, get(begin+6,4));
	}
	delete [] buffer;
	buffer = 0; size = -1;
}

TEST_F(EncoderTest, MultipleMergedMessagesOverflow) {
	encoder.encode(v4);
	loadMultiple(4+255*10+4+10);
	EXPECT_EQ(255*10, get(0,2));
	EXPECT_EQ(Message::SHIP_INFO, get(2,1));
	EXPECT_EQ(255, get(3,1));
	for (size_t i = 0; i < 255; ++i) {
		size_t begin = 4+i*10;
		EXPECT_EQ(7, get(begin+0,2));
		EXPECT_EQ(123, get(begin+2,4));
		EXPECT_EQ(456, get(begin+6,4));
	}
	{
		size_t begin = 4+255*10;
		EXPECT_EQ(10, get(begin+0,2));
		EXPECT_EQ(Message::SHIP_INFO, get(begin+2,1));
		EXPECT_EQ(1, get(begin+3,1));
		EXPECT_EQ(7, get(begin+4,2));
		EXPECT_EQ(123, get(begin+6,4));
		EXPECT_EQ(456, get(begin+10,4));
	}
	delete [] buffer;
	buffer = 0; size = -1;
}

TEST_F(EncoderTest, MultipleMessagesMixed) {
	encoder.encode(v5);
	loadMultiple(128*(4+6+6+4+10));
	for (size_t i = 0; i < 128; ++i) {
		size_t begin = i*(4+6+6+4+10);
		EXPECT_EQ(12, get(begin+0,2));
		EXPECT_EQ(Message::ACTION, get(begin+2,1));
		EXPECT_EQ(2, get(begin+3,1));
		EXPECT_EQ(4500, get(begin+4,2));
		EXPECT_EQ(15000, get(begin+6,4));
		EXPECT_EQ(4500, get(begin+10,2));
		EXPECT_EQ(15000, get(begin+12,4));
		EXPECT_EQ(10, get(begin+16,2));
		EXPECT_EQ(Message::SHIP_INFO, get(begin+18,1));
		EXPECT_EQ(1, get(begin+19,1));
		EXPECT_EQ(7, get(begin+20,2));
		EXPECT_EQ(123, get(begin+22,4));
		EXPECT_EQ(456, get(begin+26,4));
	}
	delete [] buffer;
	buffer = 0; size = -1;
}
