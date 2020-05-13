include_directories(.)
file(GLOB SOURCES RELATIVES ${CMAKE_CURRENT_SOURCE_DIR} *.c)
add_executable(texas_generate ${SOURCES})
