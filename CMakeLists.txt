cmake_minimum_required(VERSION 3.16)
project(farbits CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Configure cache variables
option(DEBUG_LOGGING "Whether or not to enable logging messages" FALSE)
option(DEBUG_PROFILING "Whether or not to profiling code/messages" FALSE)

if(DEBUG_LOGGING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG_LOGGING")
endif()

if(DEBUG_PROFILING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG_PROFILING")
endif()

# TODO - Add compiler warning flags

# You can change the sanitizer if you want to test for certain things
# https://stackoverflow.com/questions/50364533/which-of-the-three-mutually-exclusive-clang-sanitizers-should-i-default-to
# These should only be for DEBUG builds because they drastically lower performance
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")


message(STATUS "Language Global Flags:")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build Type Specific Flags (CMAKE_CXX_FLAGS_RELEASE): ${CMAKE_CXX_FLAGS_RELEASE}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build Type Specific Flags (CMAKE_CXX_FLAGS_DEBUG): ${CMAKE_CXX_FLAGS_DEBUG}")
endif()


include_directories(include)

add_subdirectory(src)
add_subdirectory(test)
