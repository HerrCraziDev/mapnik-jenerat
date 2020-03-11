CXXARGS=$(shell mapnik-config --includes --defines)

LDARGS=-lmapnik -licuuc -std=c++14


all: test update
	@echo "\e[94;1mautomake: done ($(shell date +%T))\e[0m"

test: test.cpp
	@echo Building...
	g++ $(CXXARGS) test.cpp -o test $(LDARGS) 
	@echo Build ended.

clean:
	rm *.png

clear: clean
	rm test

remake: clear all

update:
	./test