add_executable(sdb sdb.m.cpp)
target_include_directories(sdb PRIVATE ${CMAKE_SOURCE_DIR}/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libedit REQUIRED IMPORTED_TARGET libedit)

target_link_libraries(
    sdb PRIVATE sdb::libsdb edit
)
