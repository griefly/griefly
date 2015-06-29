.PHONY: all
all: Exec/KVEngine

.PHONY: travis-get-deps
travis-get-deps:
	sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test 
	sudo add-apt-repository -y ppa:sonkun/sfml-stable
	sudo add-apt-repository -y ppa:ubuntu-sdk-team/ppa
	sudo apt-get update -qq
	sudo apt-get install --yes gcc-4.8 g++-4.8
	sudo apt-get install --yes libsdl-ttf2.0-dev libsdl-image1.2-dev libsdl-mixer1.2-dev \
		libsdl-net1.2-dev zlib1g-dev libsfml-dev libpng-dev
	sudo apt-get install qtbase5-dev

.PHONY: clean
clean:
	rm -f build

Exec/KVEngine:
	mkdir -p build
	cd build && cmake ../ && make
	cp build/KVEngine Exec
