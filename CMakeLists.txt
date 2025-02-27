cmake_minimum_required(VERSION 3.25)
project(http_server C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include(FetchContent)

FetchContent_Declare(
        lua
        GIT_REPOSITORY "https://github.com/marovira/lua"
)

FetchContent_MakeAvailable(lua)

add_executable(http_server main.c
        tcp_server.c
        ownedStr.c
        staticFile.c
        md4c/src/md4c-html.c
        md4c/src/md4c.c
        md4c/src/entity.c)

target_link_libraries(http_server PRIVATE lua::lua)
