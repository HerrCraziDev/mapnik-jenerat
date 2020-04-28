CXXARGS=$(shell mapnik-config --includes --defines)

LDARGS=-lmapnik -licuuc -lpqxx -std=c++14


all: build render
	@echo "\e[94;1mautomake: All done ($(shell date +%T))\e[0m"

build: test.cpp
	@echo Building...
	g++ $(CXXARGS) test.cpp -o test $(LDARGS) 
	@echo "\e[94;1mautomake: Build done ($(shell date +%T))\e[0m".

clean:
	rm *.png

clear: clean
	rm test

remake: clear all

render:
	clear && ./test Crozon