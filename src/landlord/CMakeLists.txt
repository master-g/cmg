include_directories(.)

set(LANDLORD_SOURCES
        ai.c
        analysis.c
        beat.c
        card.c
        game.c
        hand.c
        handlist.c
        lmath.c
        player.c
        )

# 基准: 跑 10000 局 AI 对战并打印胜负数
add_executable(landlord ${LANDLORD_SOURCES} main.c)

# 自检: make test 运行
add_executable(landlord_test ${LANDLORD_SOURCES} test.c)

# 严格警告: 应当保持零警告
foreach (target landlord landlord_test)
    target_compile_options(${target} PRIVATE
            -Wall -Wextra -pedantic -Wstrict-prototypes
            -Wmissing-prototypes -Wshadow -Wconversion -Wcast-qual)
endforeach ()
