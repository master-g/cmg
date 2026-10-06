# 项目记忆 — cmg

> 跨会话的持久信息，只记代码、Git 和文档推导不出的内容。回写约定见 [CLAUDE.md](./CLAUDE.md)。
> 门槛：全文 ≤ 24 KB，单行 ≤ 300 字符；改写节各只留一块。

## 已验证的事实

<!-- 追加：确认过的决策和约束，每条一行：结论 + 原因 + 来源。同主题改写既有条目，不追加第二条。 -->
- [2026-09-20] epoll_examples 只在 Linux 可构建（依赖 sys/epoll.h 和 librt），macOS 上 cmake --build 到此必然失败，其余全部目标可过；不要按平台条件化，这是个 Linux 学习子项目
- [2026-09-20] 仓库是学习性质的子项目集合（dsaac/landlord/medsr/mph/osmanthus/texas/tlpi/epoll_examples），各自独立演进，不追求统一风格或跨子项目复用；osmanthus 的三段式工作流见 src/osmanthus/README.md
- [2026-09-20] 根 CMakeLists 不设 CMAKE_BUILD_TYPE，默认构建是 -O0，优化档位才暴露的 bug 不会出现在日常构建里（texas_generate 的栈越界就这样藏了下来）；排查内存问题要显式 -DCMAKE_BUILD_TYPE=Release 或加 -fsanitize
- [2026-09-20] src/texas 的 5/7 张评估内核已穷举验证：全部 2598960 手牌型分布与理论值吻合、7462 个 value 无空洞、7hand 对拍 20 万样本 0 误；改表或改哈希后用 texas_test 复验
- [2026-09-20] texas 7 张评估有两条路径: texas_eval_7hand 是 21 次 5 张取最小的参考实现(29.5 ns), texas_eval_7hand_fast 走完美哈希表(2.5 ns, 11.9x); 两者全量 C(52,7)=133784560 手对拍零差异, 改任一侧都要用 texas_test 复验
- [2026-10-06] src/landlord 是从 github.com/master-g/Landlord 的 bleeding 分支(b38315e)平铺拷入的，219 条历史留在原仓库未并入；binding/ 不进构建(缺 Lua 头文件和从未提交的 mt19937.h)，memtracker 与 dsaac 的不同且不合并

## 失败尝试

<!-- 追加：走不通的路径及原因，每条一行。 -->
- [2026-09-20] 7 张评估里加花色直方图捷径跳过 21 次循环反而更慢(31.9 vs 28.4 ns): 5 张同花只占 3% 的手牌, 直方图开销却在 100% 上付出; 在这个问题上省指令不如换索引结构

## 上次会话
<!-- 整块改写：分支、验证命令及实际结果、停在何处；任务细节只留一行指向证据目录。 -->
- [2026-10-06] main(与 origin/master 同一提交)；make build TARGET=landlord 通过(仅既有 -Wunused-value 警告), ./bin/landlord 跑完 10000 局且无泄漏, make test 通过
  本次: 把 Landlord 迁入 src/landlord 并接入 CMake/Makefile, 再按根 .clang-format 统一格式; 已提交未推送

## 下次运行
<!-- 整块改写：接下来的任务和优先级，含仍受阻的项。 -->
- [2026-10-06] 待用户决定: 推送到哪个分支(本地在 main, 约定写的是 master)、原 Landlord 仓库是否归档; src/landlord 的 TODO 见其 README
