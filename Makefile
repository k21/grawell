LIBS := -lm -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system
CPPFLAGS := -O2 -flto -ggdb -Wall -Wextra
OBJS := $(addprefix build/objs/,Game.o Main.o Objects.o)
HEADERS := $(addprefix src/,Game.h Objects.h Settings.h Sphere.h Vector.h)

all: dist/grawell

dist/grawell: build/grawell | dist
	cp build/grawell dist/grawell

build/grawell: $(OBJS) | build
	g++ $(CPPFLAGS) $(LIBS) -o build/grawell $(OBJS)

build/objs/%.o: src/%.cpp $(HEADERS) | build/objs
	g++ -c $(CPPFLAGS) -o $@ $<

dist:
	mkdir dist

build/objs: | build
	mkdir build/objs

build:
	mkdir build

.PHONY: clean

clean:
	rm -rf build dist
