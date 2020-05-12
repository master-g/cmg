include_directories(.)
file(GLOB SOURCES RELATIVES ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(osm_genhash ${SOURCES})
