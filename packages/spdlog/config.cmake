if(NOT TARGET spdlog)
    set(spdlog_VERSION 1.15.1)
    set(spdlog_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/packages/spdlog/include")
    set(spdlog_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/packages/spdlog/lib")

    add_library(spdlog STATIC IMPORTED)
    set_target_properties(spdlog PROPERTIES
        IMPORTED_LOCATION "${spdlog_LIBRARY_DIR}/spdlogd.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${spdlog_INCLUDE_DIRS}"
    )
endif()
