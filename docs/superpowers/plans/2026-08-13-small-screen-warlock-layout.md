# Warlock小屏响应式布局实施计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 让资源规划页面在小屏VNC中完整可操作、可查看。

**Architecture:** 仅调整`NrmDockWidget`的Qt布局。顶部状态区默认折叠，资源规划四张表改为二级页签并由操作动作切换到对应结果页。

**Tech Stack:** C++14、Qt 5.12、AFSIM Warlock插件。

## Global Constraints

- 不修改AFSIM核心源码。
- 不修改资源规划、推演或并发评估语义。
- 不增加外部依赖。
- 保持中文界面和既有数据对象。

---

### Task 1: 增加响应式布局状态

**Files:**
- Modify: `warlock/source/NrmDockWidget.hpp`
- Modify: `warlock/source/NrmDockWidget.cpp`

- [ ] 增加资源规划二级页签成员。
- [ ] 把顶部指标卡放入默认隐藏容器并增加切换按钮。
- [ ] 缩小Dock建议初始尺寸。

### Task 2: 重组资源规划页面

**Files:**
- Modify: `warlock/source/NrmDockWidget.cpp`

- [ ] 保持操作按钮位于页面顶部。
- [ ] 建立“资源分配、业务需求、校验问题、推演结果”四个页签。
- [ ] 每张表只加入对应页签并取消固定最小高度。
- [ ] 校验和推演完成后切换至对应结果页。

### Task 3: 验证和文档

**Files:**
- Modify: `docs/功能使用手册.md`

- [ ] 编译`NetworkResourceManager`。
- [ ] 运行22项固定回归和25节点场景。
- [ ] 更新小屏操作说明并提交Git。

