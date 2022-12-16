CC=g++

all:	clean server

server: server.cpp
	${CC} -o server server.cpp Game.cpp Game.h Player.cpp Player.h Cmd.cpp Cmd.h State.cpp Help.cpp Help.h

clean:
	rm -f server

