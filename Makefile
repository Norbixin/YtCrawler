CC=g++
CFLAGS=-std=c++11 -Wall -O2
GLFLAGS=-lcurl

NAME=YtCrawler
FILES=YtCrawler.cpp

all:
	$(CC) $(CFLAGS) $(FILES) -o $(NAME) $(GLFLAGS)
clean:
	rm -rf *o $(NAME) YtCrawler
