cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

project(urcl_interpreter)

file(GLOB SOURCE_FILES "src/*.c" "src/*.h")

set(CMAKE_C_FLAGS "-g -Wall -Wextra -Werror=format-security -Wno-cast-function-type -pedantic -std=c99")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
