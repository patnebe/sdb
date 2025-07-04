add_executable(sdb sdb.m.cpp)
target_include_directories(sdb PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(
    sdb PRIVATE sdb::libsdb edit
)
