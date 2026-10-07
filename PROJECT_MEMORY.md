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
- [2026-10-06] src/landlord 从 github.com/master-g/Landlord 的 bleeding 分支(b38315e)平铺拷入, 219 条历史留在原仓库; Lua/JS 绑定按用户决定不迁入; 命名统一为 cmg 约定是用户对「子项目不追求统一风格」的明确例外
- [2026-10-07] landlord 改造后的事实: 种子决定一局; Game 逐手用规则校验; 基准(种子 10000–19999)农民 5344/地主 4656/非法 0(默认 AI 为 ai_table); 迁入时的 7652 地主胜是「无人叫分重发未洗的牌」造成的假象(单独恢复该缺陷得 7621)
- [2026-10-07] landlord 的 ai_best_beat 已按用户决定改为: 选出完后剩余手数最少的一手(同分取点数低的), 炸弹与王炸只在别无选择或能出完时才用; 当时基准由 5991/4009 变为 6007/3993
- [2026-10-07] landlord 同一个 AI 自我对局的胜负数看不出强弱, 比较两个 AI 用 bin/landlord 的对打(同牌换边); ai_counted(点数计数搜索、带牌计入手数)对 ai_advanced 为 53.6%:46.4%, ai_moves(枚举合法出牌按剩余手数打分)对 ai_counted 为 57.0%:43.0%, ai_table(再加报单报双与必胜连出)对 ai_moves 为 53.8%:46.2%, 是现在的默认; 叫分仍用 analysis_standard 的手数和旧阈值
- [2026-10-07] 用户决定: 本仓 landlord 保持纯规则逻辑, 不接入神经网络或 DouZero 权重, 不在此训练模型; 要做学习类 AI 另开仓库, 可复用这里的 C 引擎
- [2026-10-07] landlord 的 AI 会对「其余两家合起来的牌」(可达 37 张)调用出牌生成, 所以 move.c 不能假设输入不超过 20 张; 曾因此写越界, Release 下不报错, Debug 的栈保护和 ASan 才报
- [2026-10-07] 在同一个 build/ 里切换 Release/Debug 后紧接着 make landlord-baseline, 可能因时间戳同秒得到用旧对象生成的基线, 表现为随后 make test 基线不符; 重新生成一次即可

## 失败尝试
- [2026-10-07] landlord ai_table 上对打证明无效的规则: 压牌多拆手数时不压(-1.5 个百分点)、队友出牌一律不压(-0.3)、队友报单送小单张与不可压牌加分(无差别); 来源 bin/landlord 对打

<!-- 追加：走不通的路径及原因，每条一行。 -->
- [2026-09-20] 7 张评估里加花色直方图捷径跳过 21 次循环反而更慢(31.9 vs 28.4 ns): 5 张同花只占 3% 的手牌, 直方图开销却在 100% 上付出; 在这个问题上省指令不如换索引结构

## 上次会话
<!-- 整块改写：分支、验证命令及实际结果、停在何处；任务细节只留一行指向证据目录。 -->
- [2026-10-07] main; landlord 新增 ai_table 并设为默认, 基线重生成; make test、make landlord-asan 通过, landlord 零警告, Debug 与 Release 生成的基线一致
  证据: src/landlord/README.md「自检与基线」

## 下次运行
<!-- 整块改写：接下来的任务和优先级，含仍受阻的项。 -->
- [2026-10-07] 待用户决定: 是否推送; 是否删除旧 AI 与 beat.c(现只被旧 AI 用); 叫分是否改用新手数; 原 Landlord 仓库是否归档
  未做: 残局精确搜索(属于搜索, 用户要求保持纯规则, 做之前先问)

