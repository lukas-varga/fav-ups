CC=g++ -std=c++17

all:	clean server

server: server.cpp
	${CC} -o server server.cpp Game.cpp Game.h Player.cpp Player.h Command.cpp Command.h State.cpp State.h Parser.cpp Parser.h Card.cpp Card.h Lobby.cpp Lobby.h

clean:
	rm -f server

