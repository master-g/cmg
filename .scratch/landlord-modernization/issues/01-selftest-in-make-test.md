# 01: landlord 自检接入 make test

**What to build:** 运行 `make test` 时会构建并运行 landlord 的自检程序。自检包含一张「一组牌 → 牌型」的用例表（先收录当前就能正确识别的牌型），以及「若干个种子的对局都能打完并产生胜者」的检查。原来的入口程序改为独立的基准目标，继续跑 10000 局并打印胜负数，不再承担测试职责。同时提供用严格警告选项构建、以及用 ASan/UBSan 构建并运行自检的方式。沿用仓库现有做法：assert 自检的可执行程序，不引入测试框架，参照 dsaac 与 texas 的测试目标。

**Blocked by:** None (can start immediately)

**Status:** done

- [x] `make test` 会运行 landlord 自检，失败时返回非零
- [x] 自检含牌型用例表，覆盖单张、对子、三张、三带一、三带二、炸弹、王炸、顺子、连对、飞机、四带二各至少一例
- [x] 自检含整局检查：一小段种子范围内每局都能结束并有胜者
- [x] 自检在几秒内跑完
- [x] 基准程序是单独的构建目标，行为与原入口一致
- [x] 有文档化的方式用严格警告选项构建 landlord，以及在 ASan/UBSan 下运行自检
- [x] Makefile 顶部的目标列表与 CLAUDE.md 的测试说明同步更新
- [x] `make fmt` 后工作区无改动，`make test` 通过
