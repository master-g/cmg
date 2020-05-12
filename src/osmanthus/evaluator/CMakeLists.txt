include_directories(.)
include_directories(..)
include_directories(../arraygenerator)
file(GLOB SOURCES RELATIVES ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(osm_eval
        ${SOURCES}
        ../arraygenerator/utils.c
        )
