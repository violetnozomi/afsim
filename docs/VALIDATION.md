# 0.1.0 验证记录

验证日期：2026-07-30  
AFSIM 基线：2.9.0  
系统：Linux x86_64，GCC 13，Release 构建

## 已通过

1. CMake 通过 `WSF_ADD_EXTENSION_PATH` 发现：
   - `wsf_network_resource_manager`
   - `NetworkResourceManager`
2. 两个共享库目标编译和链接成功。
3. `nrm_framework_types_test` 通过。
4. `mission` 启动信息明确列出 `libwsf_network_resource_manager`，并完成
   `test_mission/framework_smoke.txt`。
5. Warlock 插件导出以下三个要求的入口：
   - `wkf_plugin_registration`
   - `wkf_plugin_create`
   - `wkf_plugin_get_tags`
6. 在 GDB 中，Warlock 的 `wkf::PluginManager::LoadPluginInitialize` 已实际调用
   `WkNrm::Plugin::Plugin` 构造函数，证明插件已经被发现、校验并实例化。

## 基线环境问题

当前开发构建直接以 offscreen 模式启动 Warlock 时，在插件初始化之后发生基线运行环境的
`buffer overflow`。禁用 `NetworkResourceManager` 后使用相同命令仍然复现，因此该异常不是
本插件引入。异常发生前还有缺失 `wkf_plugins` 目录的 `opendir()` 错误。

这不影响已经完成的插件发现、动态加载和实例化验证，但在后续 GUI 联调前应单独修复或补齐
Warlock 开发运行目录。正式验收仍需完成一次有显示环境下的面板打开、场景运行和退出测试。

## 当前结论

框架已经接入 AFSIM 的正式扩展机制，并且没有修改 AFSIM 源文件。WSF 冒烟运行已闭环；
Warlock 已验证到插件实例化，完整 GUI 运行验证受现有 Warlock 开发运行环境问题限制。

