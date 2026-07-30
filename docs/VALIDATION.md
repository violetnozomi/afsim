# 0.1.2 验证记录

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

## 远程 GUI 验证

已建立仅监听服务器回环地址的 TigerVNC `:1` 桌面，并通过 Mesa llvmpipe 提供软件
OpenGL。Warlock 已打开 `test_mission/framework_smoke.txt`，主窗口在 VNC 桌面持续运行。
运行进程的内存映射确认同时加载：

- `warlock_plugins/libNetworkResourceManager_ln13m64.so`
- `wsf_plugins/libwsf_network_resource_manager_ln13m64.so`

AFSIM 2.9 随附的 Qt 5.12 在当前 glibc 下使用 `QLockFile` 时会因 fortified `readlink`
长度不一致而中止。远程启动器通过短路径运行布局和仅注入 Warlock 的兼容层规避该问题，
没有修改 AFSIM 核心源码。

## 当前结论

框架已经接入 AFSIM 的正式扩展机制，并且没有修改 AFSIM 源文件。WSF 冒烟运行与 Warlock
远程 GUI 启动均已闭环。当前面板仍是框架状态页，网络资源采集、评估和完整业务界面属于
后续版本。
