#include "gtest/gtest.h"

#include "../src/Protocol.h"

using namespace std;

class DecoderTest : public ::testing::Test {
public:
	Encoder encoder;
	Decoder decoder;
	Message m1, m2;
	vector<Message> v1, v2, v3, v4, v5;

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
	}

	void check(const vector<Message>& messages, size_t cnt = 1);
};

static bool same(const Message& m1, const Message& m2) {
	for (size_t i = 0; i < Message::FIELD_COUNT; ++i) {
		if (m1.data[i] != m2.data[i]) return false;
	}
	if (m1.text != m2.text) return false;
	return true;
}

void DecoderTest::check(const vector<Message>& messages, size_t cnt) {
	for (size_t i = 0; i < cnt; ++i) encoder.encode(messages);
	char *buffer; size_t size;
	while (encoder.next(buffer, size)) decoder.decode(buffer, size);
	Message *m;
	for (size_t j = 0; j < cnt; ++j) {
		for (size_t i = 0; i < messages.size(); ++i) {
			ASSERT_TRUE(decoder.next(m));
			ASSERT_TRUE(same(messages[j], *m));
		}
	}
	ASSERT_FALSE(decoder.next(m));
}

TEST_F(DecoderTest, SingleMessage) {
	check(v1);
}

TEST_F(DecoderTest, SingleMessageRepeated) {
	for (size_t i = 0; i < 1024; ++i) {
		check(v1);
	}
}

TEST_F(DecoderTest, SingleMessageJoin) {
	check(v1, 10);
}

TEST_F(DecoderTest, SingleMessageJoinMultiplePackets) {
	check(v1, 1024);
}

TEST_F(DecoderTest, MultipleDifferentMessages) {
	check(v2);
}

TEST_F(DecoderTest, MultipleMergedMessages) {
	check(v3);
}

TEST_F(DecoderTest, MultipleMergedMessagesOverflow) {
	check(v4);
}

TEST_F(DecoderTest, MultipleMessagesMixed) {
	check(v5);
}
