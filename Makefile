testdeck_objects = testdeck.o deck.o game.o rc4.o hand.o player.o
testhand_objects = testhand.o deck.o game.o rc4.o hand.o player.o

testdeck: $(testdeck_objects)
	$(CC) $(CFLAGS) -o $@ $(testdeck_objects)

testhand: $(testhand_objects)
	$(CC) $(CFLAGS) -o $@ $(testhand_objects)

clean:
	rm -f *.o testdeck testhand

test: testdeck testhand
	./testdeck && echo ..... OK. || echo ..... FAIL!
	./testhand && echo ..... OK. || echo ..... FAIL!

all: testdeck testhand


.PHONY: clean test

