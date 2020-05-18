project(mph)

include_directories(.)

add_executable(${PROJECT_NAME}
        lookupa.c
        perfect.c
        perfhex.c
        recycle.c
        )