add_executable(sdb.u.t
    sdb_process.t.cpp
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

target_link_libraries(sdb.u.t
    PRIVATE
    sdb::libsdb
    GTest::gtest_main
    gmock
)

include(GoogleTest)
gtest_discover_tests(sdb.u.t)

add_executable(testbin.tsk testbin.m.cpp)
