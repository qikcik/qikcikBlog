cmake_minimum_required(VERSION 3.25)
project(http_server C)

set(CMAKE_C_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        lua
        GIT_REPOSITORY "https://github.com/marovira/lua"
)

FetchContent_MakeAvailable(lua)

add_executable(http_server main.c
        http_server.c
        ownedStr.c)
target_link_libraries(http_server PRIVATE lua::lua)
