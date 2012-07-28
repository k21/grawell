DEBUG := no

GRAWELL_LIBS := $(addprefix -l, \
	sfml-graphics \
	sfml-window \
	sfml-audio \
	sfml-network \
	sfml-system \
	m \
	)
GRAWELL_LIBS += $(shell wx-config --libs)

GRAWELL_FLAGS := -fno-exceptions
GRAWELL_FLAGS += -mfpmath=sse -msse2

GRAWELL_CXXFLAGS := $(GRAWELL_FLAGS)
ifeq ($(DEBUG), yes)
    GRAWELL_CXXFLAGS += -O0 -g3 -ggdb
else
    GRAWELL_CXXFLAGS += -O2 -flto -g0
endif
GRAWELL_CXXFLAGS += -std=gnu++0x
GRAWELL_CXXFLAGS += -Wall -Wextra -Wshadow
GRAWELL_CXXFLAGS_WX := $(GRAWELL_CXXFLAGS)
GRAWELL_CXXFLAGS_WX += $(shell wx-config --cxxflags)
GRAWELL_CXXFLAGS += -Weffc++ -Wconversion

GRAWELL_SOURCES := $(wildcard src/*.cpp)
GRAWELL_SOURCES_WX := $(wildcard src-wx/*.cpp)

GRAWELL_OBJS := $(GRAWELL_SOURCES:src/%.cpp=build/objs/%.o)
GRAWELL_OBJS_WX := $(GRAWELL_SOURCES_WX:src-wx/%.cpp=build/objs-wx/%.o)


all: dist/grawell

dist/grawell: build/grawell | dist
	cp -f build/grawell dist/grawell

build/grawell: $(GRAWELL_OBJS) $(GRAWELL_OBJS_WX) | build
	$(CXX) $(GRAWELL_FLAGS) -o build/grawell \
		$(GRAWELL_OBJS) $(GRAWELL_OBJS_WX) $(GRAWELL_LIBS)

-include $(GRAWELL_OBJS:.o=.d)
-include $(GRAWELL_OBJS_WX:.o=.d)

build/objs/%.o: src/%.cpp Makefile | build/objs
	$(CXX) -c -MMD -MP $(GRAWELL_CXXFLAGS) -o $@ $<

build/objs-wx/%.o: src-wx/%.cpp Makefile | build/objs-wx
	$(CXX) -c -MMD -MP $(GRAWELL_CXXFLAGS_WX) -o $@ $<


dist:
	mkdir dist

build/objs-wx: | build
	mkdir build/objs-wx

build/objs: | build
	mkdir build/objs

build:
	mkdir build


.PHONY: clean

clean:
	rm -rf build dist
