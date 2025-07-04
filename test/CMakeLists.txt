add_executable(tests
    tests.cpp
)

find_package(Catch2 REQUIRED)

target_link_libraries(tests
    PRIVATE
    sdb::libsdb
    Catch2::Catch2
)
