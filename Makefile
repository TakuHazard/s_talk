

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L 

all: build

build:
	gcc -pthread $(CFLAGS) network.c keyboardProducer.c udpConsumer.c udpProducer.c ShutdownManager.c screenConsumer.c list.c -o s-talk

clean:
	rm ./s-talk
