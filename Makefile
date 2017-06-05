#DEBUG	= -g -O0
DEBUG	= -O3
CC	= g++
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -std=c++11 -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm -ljson-c

SRC=dht.cpp main.cpp utils.cpp

OBJ	=	$(SRC:.cpp=.o)

BINS	=	$(SRC:.cpp=)

all: climbatize

climbatize: dht22.o main.o utils.o
	@echo [link]
	@$(CC) -o $@ dht22.o main.o utils.o $(LDFLAGS) $(LDLIBS)


.cpp.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo "[Clean]"
	@rm -f $(OBJ) *~ core tags $(BINS)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
