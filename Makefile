GRAWELL_LIBS := $(addprefix -l,\
	m \
	sfml-graphics \
	sfml-window \
	sfml-audio \
	sfml-network \
	sfml-system \
	)
GRAWELL_CXXFLAGS := $(CXXFLAGS)
#GRAWELL_CXXFLAGS += -O2 -flto -ggdb
GRAWELL_CXXFLAGS += -O0 -ggdb
GRAWELL_CXXFLAGS += -std=gnu++0x
GRAWELL_CXXFLAGS += -Wall -Wextra -Weffc++ -Wshadow -Wconversion
GRAWELL_CXXFLAGS += -fno-exceptions
GRAWELL_CXXFLAGS += -mfpmath=sse -msse2
GRAWELL_SOURCES := $(wildcard src/*.cpp)
GRAWELL_OBJS := $(GRAWELL_SOURCES:src/%.cpp=build/objs/%.o)


all: dist/grawell

dist/grawell: build/grawell | dist
	cp -f build/grawell dist/grawell

build/grawell: $(GRAWELL_OBJS) | build
	$(CXX) $(GRAWELL_CXXFLAGS) $(GRAWELL_LIBS) -o build/grawell $(GRAWELL_OBJS)

-include $(GRAWELL_OBJS:.o=.d)

build/objs/%.o: src/%.cpp | build/objs
	$(CXX) -c -MMD -MP $(GRAWELL_CXXFLAGS) -o $@ $<


dist:
	mkdir dist

build/objs: | build
	mkdir build/objs

build:
	mkdir build


include test.make


.PHONY: clean

clean: clean_test
	rm -rf build dist
