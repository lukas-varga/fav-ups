cmake_minimum_required(VERSION 3.24)
project(server)

set(CMAKE_CXX_STANDARD 17)

add_executable(server server.cpp Game.cpp Game.h Player.cpp Player.h Cmd.cpp Cmd.h State.cpp Help.cpp Help.h)
