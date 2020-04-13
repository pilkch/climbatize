#DEBUG	= -g -O0
DEBUG	= -O3
CC	= g++
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -std=c++11 -std=c++1y -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS  = -ljson-c -lwiringPi -lstdc++fs -lpthread -lm

SRC=dht.cpp main.cpp utils.cpp

OBJ	=	$(SRC:.cpp=.o)

BINS	=	$(SRC:.cpp=)

ifndef PREFIX
PREFIX = /usr/local
endif
USER_HOME_FOLDER = /root

all: climbatize

climbatize: dht22.o main.o utils.o
	@echo [link]
	@$(CC) -o $@ dht22.o main.o utils.o $(LDFLAGS) $(LDLIBS)

.cpp.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONY: install

install: climbatize
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 0755 climbatize $(DESTDIR)$(PREFIX)/bin/
	mkdir -p $(USER_HOME_FOLDER)/.config/climbatize
	install -m 0644 climbatize.json $(USER_HOME_FOLDER)/.config/climbatize/

.PHONY: uninstall

uninstall:
	rm -Rf $(DESTDIR)$(PREFIX)/bin/climbatize $(USER_HOME_FOLDER)/.config/climbatize/

clean:
	@echo "[Clean]"
	@rm -f $(OBJ) *~ core tags $(BINS)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
