.PHONY: all
all: Exec/KVEngine

.PHONY: clean
clean:
	rm -rf build
	rm -f Exec/KVEngine Exec/Launcher Exec/metadata.json

Exec/KVEngine: install

.PHONY: install
install: build
	cp build/KVEngine Exec
	cp build/Launcher Exec
	cp build/metadata.json Exec

.PHONY: build
build:
	mkdir -p build
	cd build && cmake ../ -DBUILD_TESTS=ON -DBUILD_COVER=ON
	$(MAKE) -C build
