CXX      ?= g++
CXXFLAGS ?= -std=c++11 -Wall -pedantic -O2
CXXLIBS  ?= -lcurl

NAME     ?= YtCrawler.bin
FILES    ?= main.cpp

all:
	$(CXX) $(CXXFLAGS) $(FILES) -o $(NAME) $(CXXLIBS)
clean:
	rm -rf *o $(NAME)
