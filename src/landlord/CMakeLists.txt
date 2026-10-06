include_directories(.)

# -DLANDLORD_STRICT=ON 打开严格警告
option(LANDLORD_STRICT "build landlord with strict warnings" OFF)

set(LANDLORD_SOURCES
        advanced_ai.c
        card.c
        deck.c
        game.c
        hand.c
        handlist.c
        lmath.c
        memtracker.c
        player.c
        ruiko_algorithm.c
        standard_ai.c
        )

# 基准: 跑 10000 局 AI 对战并打印胜负数
add_executable(landlord ${LANDLORD_SOURCES} main.c)

# 自检: make test 运行
add_executable(landlord_test ${LANDLORD_SOURCES} test.c)

if (LANDLORD_STRICT)
    foreach (target landlord landlord_test)
        target_compile_options(${target} PRIVATE
                -Wall -Wextra -pedantic -Wstrict-prototypes
                -Wmissing-prototypes -Wshadow -Wconversion -Wcast-qual)
    endforeach ()
endif ()
