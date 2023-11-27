project(epoll_eventfd)

include_directories(../libs)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} m)
target_link_libraries(${PROJECT_NAME} tlpi)
