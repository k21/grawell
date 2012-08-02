DEBUG := no

GRAWELL_LIBS := $(addprefix -l, \
	sfml-graphics \
	sfml-window \
	sfml-audio \
	sfml-network \
	sfml-system \
	m \
	)
GRAWELL_LIBS += $(shell wx-config --unicode=yes --libs core)

GRAWELL_FLAGS := -fno-exceptions
GRAWELL_FLAGS += -mfpmath=sse -msse2

ifeq ($(DEBUG), yes)
    GRAWELL_FLAGS += -O0 -g3 -ggdb
else
    GRAWELL_FLAGS += -O2 -flto -g0
endif

GRAWELL_CXXFLAGS := $(GRAWELL_FLAGS)
GRAWELL_CXXFLAGS += -std=gnu++0x
GRAWELL_CXXFLAGS += -Wall -Wextra -Wshadow
GRAWELL_MENU_CXXFLAGS := $(GRAWELL_CXXFLAGS)
GRAWELL_MENU_CXXFLAGS += $(shell wx-config --unicode=yes --cxxflags)
GRAWELL_CXXFLAGS += -Weffc++ -Wconversion

GRAWELL_SOURCES := $(wildcard src/*.cpp)
GRAWELL_MENU_SOURCES := $(wildcard src-menu/*.cpp)

GRAWELL_OBJS := $(GRAWELL_SOURCES:src/%.cpp=build/objs/%.o)
GRAWELL_MENU_OBJS := $(GRAWELL_MENU_SOURCES:src-menu/%.cpp=build/objs-menu/%.o)


all: dist/grawell

dist/%: build/% | dist
	cp -f $< $@

build/grawell: $(GRAWELL_OBJS) $(GRAWELL_MENU_OBJS) | build
	$(CXX) $(GRAWELL_FLAGS) -o build/grawell \
		$(GRAWELL_OBJS) $(GRAWELL_MENU_OBJS) $(GRAWELL_LIBS)

-include $(GRAWELL_OBJS:.o=.d)
-include $(GRAWELL_MENU_OBJS:.o=.d)

build/objs/%.o: src/%.cpp Makefile | build/objs
	$(CXX) -c -MMD -MP $(GRAWELL_CXXFLAGS) -o $@ $<

build/objs-menu/%.o: src-menu/%.cpp Makefile | build/objs-menu
	$(CXX) -c -MMD -MP $(GRAWELL_MENU_CXXFLAGS) -o $@ $<


dist:
	mkdir dist

build/objs-menu: | build
	mkdir build/objs-menu

build/objs: | build
	mkdir build/objs

build:
	mkdir build


.PHONY: clean

clean:
	rm -rf build dist
