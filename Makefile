CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wno-deprecated

INCLUDES := -I. -I/usr/include/SDL -I./third_party/loki-0.1.7/include/loki
LIBS := -L/usr/lib/x86_64-linux-gnu -lSDLmain -lSDL -lGL -lGLU -lSDL_ttf -lSDL_image -lSDL_mixer -lSDL_net -lz -lsfml-audio -llibpng12

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

Release/knv: $(OBJS)
	$(CXX) $(OBJS) $(LIBS) -o Release/knv

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<

