.PHONY: all
all: Exec/KVEngine

.PHONY: travis-get-deps
travis-get-deps:
	sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
	sudo add-apt-repository -y ppa:beineri/opt-qt542
	sudo add-apt-repository -y ppa:sonkun/sfml-stable
	sudo add-apt-repository -y ppa:ubuntu-sdk-team/ppa
	sudo add-apt-repository -y ppa:kalakris/cmake
	sudo apt-get update -qq
	sudo apt-get install --yes gcc-4.9 g++-4.9
	sudo apt-get install --yes libsdl-ttf2.0-dev libsdl-image1.2-dev libsdl-mixer1.2-dev \
		libsdl-net1.2-dev zlib1g-dev libsfml-dev libpng-dev lcov
	sudo apt-get install qtbase5-dev
	sudo apt-get install cmake
	gem install coveralls-lcov

.PHONY: clean
clean:
	rm -rf build
	rm -f Exec/KVEngine Exec/Launcher Exec/metadata.json

.PHONY: partial-build
partial-build: build/Makefile
	$(MAKE) -C build

build/Makefile:
	mkdir -p build
	cd build && cmake ../ -DBUILD_TESTS=ON -DBUILD_COVER=ON

Exec/KVEngine: install

.PHONY: install
install: partial-build
	cp build/KVEngine Exec
	cp build/Launcher Exec
	cp build/metadata.json Exec
