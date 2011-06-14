CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g

CPPFLAGS = -D_GNU_SOURCE

common_objects = deck.o game.o rc4.o hand.o player.o card.o
testdeck_objects = testdeck.o $(common_objects)
testhand_objects = testhand.o $(common_objects)
ircpoker_objects = irc.o command.o $(common_objects)

ircpoker: $(ircpoker_objects)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(ircpoker_objects) -lircclient

testdeck: $(testdeck_objects)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(testdeck_objects)

testhand: $(testhand_objects)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(testhand_objects)

clean:
	rm -f *.o testdeck testhand ircpoker

test: testdeck testhand
	./testdeck && echo ..... OK. || echo ..... FAIL!
	./testhand && echo ..... OK. || echo ..... FAIL!

all: ircpoker testdeck testhand


.PHONY: clean test all

