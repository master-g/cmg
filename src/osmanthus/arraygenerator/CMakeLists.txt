include_directories(.)
include_directories(..)

file(GLOB SOURCES RELATIVES ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(osm_genarray
        ${SOURCES}
        main.c
        ../evaluator/evalarrays.c
        ../evaluator/evaluator.c
        )
