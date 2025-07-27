cmake_minimum_required(VERSION 3.19)

project("sdb" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory("src")
add_subdirectory("tools")

if (BUILD_TESTING)
    add_subdirectory("test")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(libedit REQUIRED IMPORTED_TARGET libedit)

target_link_libraries(sdb PRIVATE sdb::libsdb)
