# 14: 命名统一为 cmg 约定

**What to build:** landlord 的标识符遵循 cmg 的约定：函数与变量用 snake_case，函数名以所属 module 为前缀，类型用 `xxx_t`，宏全大写带 module 前缀，字段不混用驼峰。include guard 统一为一种写法，需要的头文件都带 C++ 的链接声明或都不带。注释与标识符中的拼写错误修正。这是一次全子项目的机械重命名，一次完成。项目记忆里「子项目不追求统一风格」对 landlord 例外，这是所有者的明确决定。

**Blocked by:** 13（const、枚举与结构化牌型）

**Status:** ready-for-agent

- [ ] 没有 PascalCase 的函数名和驼峰的字段名
- [ ] 枚举常量采用同一种命名写法
- [ ] include guard 写法一致
- [ ] 已知拼写错误（如 length、recycle 的误拼）已修正
- [ ] 重命名与行为改动不混在同一个提交里
- [ ] 基线与改动前完全一致
- [ ] landlord 在严格警告选项下零警告
- [ ] `make fmt` 后工作区无改动，`make test` 通过
