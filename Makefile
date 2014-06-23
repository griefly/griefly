CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wno-deprecated

INCLUDES := -I. -I/usr/include/SDL -I./third_party/loki-0.1.7/include/loki
LIBS := -lSDL -lGL -lGLU -lSDL_ttf -lSDL_image -lSDL_mixer -lSDL_net -lz -lsfml-audio

SOURCES := $(wildcard Sources/*.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))

.PHONY: all
all: Release/knv

.PHONY: clean
clean:
	rm -f Sources/*.o Release/knv

Release/knv: $(OBJS)
	$(CXX) $(LIBS) $(OBJS) -o Release/knv

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<

