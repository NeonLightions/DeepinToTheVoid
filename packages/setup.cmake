set(packages_DIR ${CMAKE_SOURCE_DIR}/packages)

include(${packages_DIR}/pdcurses/config.cmake)
include(${packages_DIR}/spdlog/config.cmake)

set(LIBS winmm pdcurses spdlog)
set(INCLUDE_DIRS ${pdcurses_INCLUDE_DIRS} ${spdlog_INCLUDE_DIRS})