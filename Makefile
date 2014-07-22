CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wno-deprecated
LDFLAGS :=

INCLUDES := -I. -I/usr/include/SDL -I./third_party/loki-0.1.7/include/loki
LIBS := -L/usr/lib/x86_64-linux-gnu -L/usr/lib32 -lSDLmain -lSDL -lGL -lGLU -lSDL_ttf -lSDL_image -lSDL_mixer -lSDL_net -lz -lsfml-audio -lpng

SOURCES := $(wildcard Sources/*.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))

.PHONY: all
all: Release/knv

.PHONY: travis-get-deps
travis-get-deps:
	sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test 
	sudo add-apt-repository -y ppa:sonkun/sfml-stable
	sudo apt-get update -qq
	sudo apt-get install --yes gcc-4.8 g++-4.8
	sudo apt-get install --yes libsdl-ttf2.0-dev libsdl-image1.2-dev libsdl-mixer1.2-dev \
		libsdl-net1.2-dev zlib1g-dev libsfml-dev libpng-dev

.PHONY: clean
clean:
	rm -f Sources/*.o Release/knv

.PHONY: 32bit
32bit: M = -m32
32bit: all

Release/knv: $(OBJS)
	$(CXX) $(LDFLAGS) $(M) $(OBJS) $(LIBS) -o Release/knv

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(M) $(INCLUDES) -o $@ -c $<

