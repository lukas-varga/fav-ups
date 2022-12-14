CC=g++

all:	clean server

server: server.cpp
	${CC} -o server server.cpp Game.cpp Game.h Player.cpp Player.h Command.cpp Command.h State_Machine.cpp State_Machine.h

clean:
	rm -f server

