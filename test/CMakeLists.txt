add_executable(tests
    tests.cpp
)

target_link_libraries(tests
    PRIVATE
    sdb::libsdb
)

include(CTest)
add_test(NAME sdb_tests COMMAND tests)
