

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

all: build

build:
	gcc -pthread $(CFLAGS) network.c keyboardProducer.c udpConsumer.c udpProducer.c screenConsumer.c list.o -o s-talk

clean:
	rm ./s-talk
