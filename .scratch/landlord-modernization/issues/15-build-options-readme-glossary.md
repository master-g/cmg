# 15: 构建选项、README 与 GLOSSARY

**What to build:** 对局日志的开关和花色的显示方式是构建选项，切换它们不需要改源码。每个公共头文件只包含自己需要的标准头，不再有一个把所有标准头带给所有人的公共头。子项目 README 说明 module 划分、如何构建和运行基准与自检、如何更新基线。landlord 有自己的 GLOSSARY.md，收录 spec 中定义的术语，并在根目录的 GLOSSARY-MAP.md 里登记。

**Blocked by:** 14（命名统一为 cmg 约定）

**Status:** ready-for-agent

- [ ] 开启与关闭对局日志、切换花色显示方式都只需构建参数，两种取值下自检都通过
- [ ] 任一公共头文件单独被包含时都能通过编译
- [ ] README 覆盖：module 划分、构建与运行、自检与基线的更新方法
- [ ] landlord 的 GLOSSARY.md 已建立，根目录 GLOSSARY-MAP.md 指向它
- [ ] CLAUDE.md 与 Makefile 中关于 landlord 的说明与现状一致
- [ ] 基线与改动前完全一致
- [ ] landlord 在严格警告选项下零警告
- [ ] `make fmt` 后工作区无改动，`make test` 通过
