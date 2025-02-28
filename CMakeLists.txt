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

FetchContent_Declare(
        MD4C
        GIT_REPOSITORY "https://github.com/mity/md4c"
)

FetchContent_MakeAvailable(MD4C)

include_directories(${MD4C_SOURCE_DIR}/src)
add_executable(http_server source/main.c
        source/tcp_server.c
        source/ownedStr.c
        source/staticFile.c
        ${MD4C_SOURCE_DIR}/src/md4c.c
        ${MD4C_SOURCE_DIR}/src/entity.c
        ${MD4C_SOURCE_DIR}/src/md4c-html.c)

target_link_libraries(http_server PRIVATE lua::lua)
target_link_libraries(http_server)
