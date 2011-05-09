common_objects = deck.o game.o rc4.o hand.o player.o
testdeck_objects = testdeck.o $(common_objects)
testhand_objects = testhand.o $(common_objects)
ircpoker_objects = irc.o command.o $(common_objects)

ircpoker: $(ircpoker_objects)
	$(CC) $(CFLAGS) -o $@ $(ircpoker_objects) -lircclient

testdeck: $(testdeck_objects)
	$(CC) $(CFLAGS) -o $@ $(testdeck_objects)

testhand: $(testhand_objects)
	$(CC) $(CFLAGS) -o $@ $(testhand_objects)

clean:
	rm -f *.o testdeck testhand

test: testdeck testhand
	./testdeck && echo ..... OK. || echo ..... FAIL!
	./testhand && echo ..... OK. || echo ..... FAIL!

all: ircpoker


.PHONY: clean test

