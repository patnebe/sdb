add_library(libsdb sdb_command.cpp sdb_stringutil.cpp sdb_process.cpp sdb_process_state.cpp)
add_library(sdb::libsdb ALIAS libsdb)

set_target_properties(
    libsdb
    PROPERTIES OUTPUT_NAME sdb
)

target_include_directories(
    libsdb
    # PRIVATE include
    # PUBLIC $(CMAKE_SOURCE_DIR)/include
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>

    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/include
)

include(GNUInstallDirs)
install(TARGETS libsdb
    EXPORT sdb-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT sdb-targets
    FILE sdb-config.cmake
    NAMESPACE sdb::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sdb
)
