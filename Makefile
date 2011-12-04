# program

VERSION = 0.0.1

# includes and libs
LIBS =

# flags
CFLAGS = -Wall -O3 ${LIBS} -DVERSION=\"${VERSION}\" -I./include -shared

SRC = src/arduino.c 
BIN = libarduino.so

all:
	gcc -o $(BIN) $(SRC) $(CFLAGS)

clean:
	rm -f $(BIN)
	make -C example clean

install:
	cp -f $(BIN) /usr/lib/
	mkdir -p /usr/local/include/arduino/
	cp -f include/* /usr/local/include/arduino/

test:
	make -C example
