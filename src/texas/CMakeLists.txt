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
        texas_array.c
        texas_array7.c
        texas_eval.c
        texas_eval7.c
        texas_utils.c
        test.c
        )

# 生成 texas_array7.c/h, 因此不能依赖它们
add_executable(texas_generate7
        texas_array.c
        texas_eval.c
        texas_gen7.c
        )

add_executable(texas_all
        texas_array.c
        texas_array7.c
        texas_eval.c
        texas_eval7.c
        texas_utils.c
        texas_main.c
        )

add_executable(texas_eval
        texas_array.c
        texas_array7.c
        texas_eval.c
        texas_eval7.c
        texas_utils.c
        eval_main.c
)