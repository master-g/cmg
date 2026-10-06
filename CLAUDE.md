# cmg — Claude 项目上下文

> 项目指令与常用命令，供 Claude / AI 编码代理按任务需要查阅。
> 跨会话记忆见 [PROJECT_MEMORY.md](./PROJECT_MEMORY.md)。

## 技术栈

<!-- 使用的语言、框架、工具链、版本约束。例: Python 3.12 / FastAPI / uv / Postgres 16 -->

- C (主要) / C++ 标准设为 C++11，实际源码全是 `.c`；无外部依赖，只用 libc、`libm`、Linux 下的 `librt`
- 构建: CMake ≥ 3.16 + Make（macOS 下为 AppleClang，Linux 下为 gcc/clang）
- 格式化: clang-format，配置见根目录 `.clang-format`（LLVM 基础 + 2 空格缩进 + 80 列）
- 产物统一输出到 `bin/`（`EXECUTABLE_OUTPUT_PATH`，已 gitignore）
- 子项目各自独立、互不依赖，仅 `src/libs/tlpi` 作为静态库被 `epoll_examples` 和 `tlpi` 链接
- 子项目各有 README，进入前先读：`src/texas`、`src/dsaac`、`src/osmanthus`、`src/mph`、`src/epoll_examples`、`src/landlord`

## 命令

<!-- 常用脚本、构建命令、测试命令、lint/格式化命令。让代理无需猜测即可运行。 -->

统一入口是根目录 Makefile，`make help` 列出全部 target。

- 安装依赖: 无第三方依赖，只需 cmake ≥ 3.16 + clang/gcc + clang-format
- 构建: `make build`；单个子项目用 `make build TARGET=dsaac`（可选值见 Makefile 顶部注释）
- 档位: `make build BUILD_TYPE=Release`（默认 Debug）；产物落到 `bin/`，cmake 中间件在 `build/`
- 测试: 无测试框架。`make test` 构建并运行 `dsaac`（`src/dsaac/test.c` 的 assert 自检）、`texas_test` 与 `landlord_test`；`make landlord-asan` 在 ASan/UBSan 下跑 landlord 自检
- Lint / 格式化: `make fmt`（对 dsaac / epoll_examples / landlord / medsr / texas / tlpi 跑 `clang-format -i`）
- 清理: `make clean` 只删 `bin/`，`make clean-build` 连 `build/` 一起删
- 生成: `make ename` 重新生成 `src/libs/tlpi/ename.c.inc`；`make landlord-baseline` 重新生成 landlord 的对局基线（只在有意改变对局行为后）
- 重建 texas 查找表（改了生成器才需要，产物已提交）: 流程见 [src/texas/README.md](./src/texas/README.md)

## 代码风格

<!-- 命名约定、格式规则、架构偏好。例: 函数用 snake_case、组件单文件、优先组合而非继承 -->

- 一律跑 `make fmt` 定型：2 空格缩进、80 列、`BreakBeforeBraces: Attach`、指针右靠（`char *p`）、不用 Tab
- 标识符用 snake_case；类型用 `xxx_t`（`list_t`、`camera_t`），函数名以所属模块为前缀（`list_iter_next`、`pdata_u32`），宏全大写带模块前缀（`TEXAS_CARD_RANK`）
- 头文件用 include guard（`LIST_H_` / `MEDSR_RENDERER_H_` / `CMG_TEXAS_EVAL_H`），不用 `#pragma once`
- 文件命名跟随所在子项目：dsaac / texas / libs 用小写，medsr 用 PascalCase，不要跨子项目统一
- 公开 API 的文档注释用 `\brief` / `\param` / `\return` 风格；老文件保留原有版权头
- 内存所有权显式：容器不负责释放 `pdata` 载荷（见 commit `9ea690c`），调试期用 `memtracker.h` 追踪
- 生成器要链接参考实现，所以参考实现不能 include 自己产物的头文件；查表实现单独成文件（见 `texas_eval.c` 与 `texas_eval7.c` 的拆分），否则构建成环

## 禁止文件

<!-- 绝对不能修改的文件清单: 生成物、密钥、锁文件、迁移历史、第三方 vendor 目录等 -->

- `bin/`、`build/`、`cmake-build-debug/`：构建产物
- `src/mph/`：Bob Jenkins 最小完美哈希的第三方移植（public domain），除非在移植上游改动，否则不手改
- `src/medsr/Utils/` 里的 `LodePng.[ch]`、`jsmn.[ch]`、`Gif.[ch]`：第三方单文件库
- `src/libs/tlpi/ename.c.inc`：由 `Build_ename.sh` 生成，改脚本不改产物
- `src/texas/texas_array.c`、`texas_array7.c`、`texas_array7.h`：查找表产物，改生成器不改表；表体有 `clang-format off` 保护，`make fmt` 不会动它们
- `src/landlord/baseline.c.inc`：对局基线，由 `make landlord-baseline` 生成；自检与它不一致时先判断是不是改坏了，不手改
- `src/medsr/suzanne.png`、`suzanne.babylon`：渲染器测试资产

## 审查规则

<!-- PR 审查的标准和流程: 必须通过的检查、谁审、合并条件、提交信息规范 -->

- 个人仓库，直推 `main`，无 CI、无 PR 模板、无 CODEOWNERS
- 提交信息用 Conventional Commits 小写祈使句：`feat: ...`、`refactor: ...`（早期 `update` 是历史遗留，不要沿用）
- 合并前自检：`cmake --build build` 通过（epoll_examples 在 macOS 上必失败，见 PROJECT_MEMORY.md），改过的文件跑过 `make fmt`

## 项目记忆 (回写约定)

跨会话的持久信息记录在 [PROJECT_MEMORY.md](./PROJECT_MEMORY.md)，只写代码、Git 和文档推导不出的内容。

收尾顺序：验证 → 回写记忆 → 提交 → 推送。回写与本次改动进同一个提交；不提交的任务在结束前回写。

回写是提炼，不是记录：

- 「上次会话」「下次运行」整节改写成一块（分支、验证命令与实际结果、停在何处；接下来做什么），旧块直接删，历史由 Git 保存。
- 本次确认的决策追加到「已验证的事实」，走不通的路径及原因追加到「失败尝试」；每条一行，写结论、原因和来源或适用范围。能从 `git log`、代码或计划文档 30 秒内推出来的（commit SHA、进度状态、计划内容）不写，只留一行指向证据目录。
- 同主题已有条目就改写它，不追加；结论被推翻就删旧条。写不进两行的长期知识搬进 docs/ 后只留一行指针。
- 结束前运行 `python3 ~/.claude/skills/bootstrap-claude/scripts/memory.py check PROJECT_MEMORY.md`，不通过不算完成：全文 ≤ 24 KB，单行 ≤ 300 字符，改写节各一块。超限先 `compact` 压改写节，再合并或搬出事实；脚本不自动删追加节条目。

<!-- bootstrap-claude convention v2 -->

## Agent skills

### Issue tracker

spec 与工单是 `.scratch/<feature>/` 下的 Markdown 文件，随仓库提交，不用 GitHub Issues。见 `docs/agents/issue-tracker.md`。

### Triage labels

沿用默认的五个角色名（`needs-triage`、`needs-info`、`ready-for-agent`、`ready-for-human`、`wontfix`），记在工单文件的 `Status:` 行。见 `docs/agents/triage-labels.md`。

### Domain docs

multi-context：根目录 `GLOSSARY-MAP.md` 指向各子项目自己的 `GLOSSARY.md`，ADR 跟随子项目，文件用到时才创建。见 `docs/agents/domain.md`。
