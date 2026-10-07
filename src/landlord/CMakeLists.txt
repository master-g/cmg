include_directories(.)

set(LANDLORD_SOURCES
        ai.c
        analysis.c
        beat.c
        card.c
        game.c
        hand.c
        hand_list.c
        lmath.c
        move.c
        player.c
        )

# 基准: 跑 10000 局 AI 对战并打印胜负数
add_executable(landlord ${LANDLORD_SOURCES} main.c)

# 自检: make test 运行
add_executable(landlord_test ${LANDLORD_SOURCES} test.c)

# 构建选项 (cmake -D<选项>=ON, 经 make 传入用 CMAKE_ARGS):
#   LANDLORD_LOG          打印每一局的每一手牌
#   LANDLORD_ASCII_SUITS  花色写作 c d h s, 不用 ♣♦♥♠
option(LANDLORD_LOG "landlord: print every move of every game" OFF)
option(LANDLORD_ASCII_SUITS "landlord: write suits as c d h s" OFF)

# 严格警告: 应当保持零警告
foreach (target landlord landlord_test)
    target_compile_options(${target} PRIVATE
            -Wall -Wextra -pedantic -Wstrict-prototypes
            -Wmissing-prototypes -Wshadow -Wconversion -Wcast-qual)
    if (LANDLORD_LOG)
        target_compile_definitions(${target} PRIVATE LANDLORD_PRINT_LOG)
    endif ()
    if (LANDLORD_ASCII_SUITS)
        target_compile_definitions(${target} PRIVATE LANDLORD_ASCII_SUITS)
    endif ()
endforeach ()
