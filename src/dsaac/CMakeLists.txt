project(dsaac)

include_directories(.)

file(GLOB SOURCES *.c)

add_executable(${PROJECT_NAME} ${SOURCES})
