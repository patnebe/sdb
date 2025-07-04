cmake_minimum_required(VERSION 3.19)

project("sdb" LANGUAGES CXX)

include(CTest)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory("src")
add_subdirectory("tools")

if (BUILD_TESTING)
    find_package(Catch2 CONFIG REQUIRED)
    add_subdirectory("test")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(libedit REQUIRED IMPORTED_TARGET libedit)

target_link_libraries(sdb PRIVATE sdb::libsdb)
