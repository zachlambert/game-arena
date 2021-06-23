.PHONY: build
build:
	mkdir -p build
	cmake -E chdir build cmake -DCMAKE_BUILD_TYPE=Debug ..
	cmake --build build

.PHONY: run
run:
	build/game
