# Warlock Chinese Responsive UI Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 中文化 Warlock 可见反馈，并解除表格及态势画布对窗口最小尺寸的约束。

**Architecture:** 新建无 Qt 依赖的 `UiText` 映射组件，以稳定英文编码为输入、中文显示文本为输出；DockWidget 只在渲染时调用该组件。布局层统一采用可交互列宽、滚动条和可收缩尺寸策略，不触碰核心模型和序列化输出。

**Tech Stack:** C++17、Qt/Warlock、CMake、现有自包含 C++ 测试。

## Global Constraints

- 不修改 AFSIM 核心源码。
- 不修改 JSON Schema、资源规划文件格式或英文协议编码。
- 不新增第三方依赖。
- 修改范围仅限网络资源管理器插件、测试和说明文档。

---

### Task 1: 界面中文映射

**Files:**
- Create: `warlock/source/NrmUiText.hpp`
- Create: `warlock/source/NrmUiText.cpp`
- Create: `tests/UiTextTest.cpp`
- Modify: `CMakeLists.txt`
- Modify: `warlock/source/CMakeLists.txt`
- Modify: `warlock/source/NrmDockWidget.cpp`

**Interfaces:**
- Produces: `std::string WkNrm::UiText::TranslateCode(const std::string&)`
- Produces: `std::string WkNrm::UiText::TranslateCodeWithRaw(const std::string&)`
- Produces: `std::string WkNrm::UiText::TranslateListWithRaw(const std::string&)`

- [ ] 编写中文枚举、原因码、字段名及未知值回退测试。
- [ ] 运行测试并确认因映射组件缺失而失败。
- [ ] 实现最小映射组件并接入构建。
- [ ] 将 DockWidget 可见的原始枚举和原因码改为界面映射。
- [ ] 运行映射测试及现有测试。

### Task 2: 响应式布局

**Files:**
- Modify: `warlock/source/NrmDockWidget.cpp`
- Modify: `warlock/source/NrmTacticalView.cpp`

**Interfaces:**
- `CreateTable` 返回可缩小、列宽可交互、支持滚动和长文本工具提示的表格。

- [ ] 增加布局源代码回归检查，验证不存在 `ResizeToContents` 和 `setMinimumSize(620, 480)`。
- [ ] 运行检查并确认当前代码失败。
- [ ] 修改表格尺寸策略、列宽策略和态势画布最小尺寸。
- [ ] 移除需求页四张表的固定最小高度。
- [ ] 运行布局检查和插件构建。

### Task 3: 文档和完整验证

**Files:**
- Modify: `docs/使用与测试指南.md`（若现有使用文档名称不同，则更新对应主使用文档）

**Interfaces:**
- 记录中文显示与英文接口编码的边界，以及表格列宽调整和滚动方式。

- [ ] 更新界面操作说明。
- [ ] 构建全部 NRM 测试目标并运行 CTest。
- [ ] 构建 Warlock 插件。
- [ ] 检查 Git 差异只包含本次范围。
