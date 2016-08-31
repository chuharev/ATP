# Clipboard char counter

CC=i686-w64-mingw32-gcc
LIBS=-lgdi32
OPT =-municode -Wall -mwindows  -Wl,merge.ld

# BUILD INFO:
OS='"$(shell lsb_release -d -s)"'
COMPILER='"$(CC) $(shell i686-w64-mingw32-gcc -dumpversion)"'
REV = '"$(shell  git rev-parse HEAD | cut -c1-10)..."'
TODAY='"$(shell date +%d.%m.%Y)"'


TARGET= clcc.exe
SRC = clcc.c

all: $(OBJ)
		$(CC) -Os -s -o $(TARGET)  $(SRC)   $(LIBS)  $(OPT) \
		-D REVISION=$(REV) -D OS=$(OS) -D COMPILER=$(COMPILER) -D TODAY=$(TODAY)

.PHONY: clean install

clean:
		rm -f *.o  *.exe

install:
		cp  -f  $(TARGET) ~/Projects


