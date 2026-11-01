cmake_minimum_required(VERSION 3.10)
project(gateway C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_definitions(_POSIX_C_SOURCE=200809L _DEFAULT_SOURCE)

find_path(MOSQUITTO_INCLUDE_DIR mosquitto.h)
find_library(MOSQUITTO_LIBRARY mosquitto)

if(NOT MOSQUITTO_INCLUDE_DIR OR NOT MOSQUITTO_LIBRARY)
    message(FATAL_ERROR "libmosquitto-dev not found. On Debian/Ubuntu: apt-get install libmosquitto-dev")
endif()

add_executable(gateway
    src/main.c
    src/config.c
    src/log.c
    src/sensors.c
    src/session.c
    src/offline_queue.c
    src/mqtt_client.c
    src/publisher.c
    src/watchdog.c
    src/diagnostics.c
)

target_include_directories(gateway PRIVATE include ${MOSQUITTO_INCLUDE_DIR})
target_link_libraries(gateway PRIVATE ${MOSQUITTO_LIBRARY} pthread m)
target_compile_options(gateway PRIVATE -Wall -Wextra -g)

option(WITH_ASAN "Build with AddressSanitizer" OFF)
option(WITH_TSAN "Build with ThreadSanitizer" OFF)

if(WITH_ASAN)
    target_compile_options(gateway PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(gateway PRIVATE -fsanitize=address)
endif()

if(WITH_TSAN)
    target_compile_options(gateway PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(gateway PRIVATE -fsanitize=thread)
endif()
