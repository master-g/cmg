project(medsr)

include_directories(.)
include_directories(./Math)
include_directories(./Profile)
include_directories(./Renderer)
include_directories(./Utils)

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c Math/*.c Profile/*.c Renderer/*.c Utils/*.c)

add_executable(${PROJECT_NAME} ${SOURCES})