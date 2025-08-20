cmake_minimum_required(VERSION 3.19)

project("sdb" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory("src")
add_subdirectory("tools")
add_subdirectory("test")

target_link_libraries(sdb PRIVATE sdb::libsdb)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
