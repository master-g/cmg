include_directories(.)

add_executable(texas_generate
        bits.c
        generate.c
        main.c
        )

add_executable(texas_test
        answer.c
        bits.c
        generate.c
        test.c
        )

add_executable(texas_eval
        texas_array.c
        texas_eval.c
        texas_utils.c
        texas_main.c
        )
