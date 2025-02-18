if(NOT TARGET pdcurses)
    set(pdcurses_VERSION 3.9)
    set(pdcurses_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/packages/pdcurses/include")
    set(pdcurses_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/packages/pdcurses/lib")

    add_library(pdcurses STATIC IMPORTED)
    set_target_properties(pdcurses PROPERTIES
        IMPORTED_LOCATION "${pdcurses_LIBRARY_DIR}/pdcurses.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${pdcurses_INCLUDE_DIRS}"
    )
endif()
