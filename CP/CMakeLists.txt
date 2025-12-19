cmake_minimum_required(VERSION 3.10)
project(project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include_directories(include)

add_executable(server 
    src/server/main_server.cpp 
    src/server/server_app.cpp 
    src/game_logic.cpp
)

target_link_libraries(server PRIVATE Threads::Threads)


add_executable(client 
    src/client/main_client.cpp 
    src/client/client_app.cpp 
    src/game_logic.cpp
)

target_link_libraries(client PRIVATE Threads::Threads)