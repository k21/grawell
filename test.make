TESTED_SOURCES := src/Encoder.cpp src/Decoder.cpp
TESTED_OBJS := $(TESTED_SOURCES:src/%.cpp=build/objs/%.o)

TEST_SOURCES := $(wildcard test/*.cpp)
TEST_OBJS := $(TEST_SOURCES:test/%.cpp=test/%.o)

GTEST_SOURCES := $(wildcard test/gtest/*.cc)
GTEST_OBJS := $(GTEST_SOURCES:test/gtest/%.cc=test/gtest/%.o)

COMMON_TEST_CXXFLAGS := $(CXXFLAGS)
COMMON_TEST_CXXFLAGS += -O0 -ggdb
COMMON_TEST_CXXFLAGS += -fno-exceptions
COMMON_TEST_CXXFLAGS += -mfpmath=sse -msse2
COMMON_TEST_CXXFLAGS += -isystem test/gtest

TEST_CXXFLAGS := $(COMMON_TEST_CXXFLAGS)
TEST_CXXFLAGS += -std=gnu++0x
TEST_CXXFLAGS += -Wall -Wextra -Weffc++ -Wshadow -Wconversion

GTEST_CXXFLAGS := $(COMMON_TEST_CXXFLAGS)
GTEST_CXXFLAGS += -std=c++98
GTEST_CXXFLAGS += -DGTEST_HAS_PTHREAD=0


test/grawell_test: $(TESTED_OBJS) $(TEST_OBJS) $(GTEST_OBJS)
	$(CXX) $(COMMON_TEST_CXXFLAGS) -o test/grawell_test \
		$(TESTED_OBJS) $(TEST_OBJS) $(GTEST_OBJS)

test/%.o: test/%.cpp
	$(CXX) -c -MMD -MP $(TEST_CXXFLAGS) -o $@ $<

test/gtest/%.o: test/gtest/%.cc
	$(CXX) -c -MMD -MP $(GTEST_CXXFLAGS) -o $@ $<


.PHONY: check clean_test

check: test/grawell_test
	./test/grawell_test

clean_test:
	rm -f test/grawell_test test/*.o test/gtest/*.o
