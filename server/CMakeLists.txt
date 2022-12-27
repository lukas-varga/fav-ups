cmake_minimum_required(VERSION 3.24)
project(server)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")

add_executable(server server.cpp
        Game.cpp Game.h
        Player.cpp Player.h
        Command.cpp Command.h
        State.cpp State.h
        Help.cpp Help.h
        Card.cpp Card.h
        Lobby.cpp Lobby.h)
