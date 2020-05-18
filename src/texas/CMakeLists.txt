include_directories(.)

file(GLOB SHARED_SRC RELATIVES ${CMAKE_CURRENT_SOURCE_DIR}
        bits.c
        generate.c
        )

add_executable(texas_generate
        ${SHARED_SRC}
        main.c
        )

add_executable(texas_test
        ${SHARED_SRC}
        answer.c
        test.c
        )

add_executable(texas_eval
        texas_array.c
        texas_eval.c
        texas_utils.c
        )
