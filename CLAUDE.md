# CLAUDE.md

本项目的完整开发规则位于AGENTS.md，必须先完整阅读并执行。

每次开始工作还必须读取：

1. docs/ai/PROJECT_MEMORY.md
2. docs/ai/CURRENT_MILESTONE.md
3. docs/ai/DECISIONS.md
4. docs/ai/SESSION_HANDOFF.md
5. docs/NEXT_DEVELOPMENT_INSTRUCTIONS.md

开始时运行scripts/ai_guard.sh status，结束前运行scripts/ai_guard.sh static和受影响测试。

一次只完成CURRENT_MILESTONE中的一个任务。未经用户明确要求，不运行参数扫描、多seed实验、新算法比较或长时间批量仿真；测试失败时修复实现，不通过调参改变验收结果。

不得修改AFSIM核心、删除现有改动、猜测甲方接口、实现导航算法或把模拟结果标为最终验收。
