# Windows 远程调试

## 架构

```text
Windows VNC Viewer
       │ 127.0.0.1:5901
       ▼
Windows SSH 隧道
       │ 加密连接
       ▼
服务器 127.0.0.1:5901
       │
       ▼
TigerVNC :1 → Openbox → Warlock
```

服务器 VNC 只监听 `127.0.0.1`，没有对局域网或公网开放端口。VNC 层不再重复设置密码，
访问控制完全依赖 SSH 登录和 SSH 隧道。

## Windows 准备

安装一个支持普通 VNC/RFB 的 Viewer，推荐 TigerVNC Viewer。不要让 Viewer 直接连接服务器
IP 的 `5901` 端口。

在 Windows PowerShell 中建立隧道：

```powershell
ssh -N -L 5901:127.0.0.1:5901 pyh@服务器地址
```

也可以把项目中的 `scripts/remote/windows-nrm-tunnel.ps1` 复制到 Windows 后执行：

```powershell
.\windows-nrm-tunnel.ps1 -Server 服务器地址
```

如果 SSH 不是 22 端口：

```powershell
.\windows-nrm-tunnel.ps1 -Server 服务器地址 -SshPort 实际端口
```

该 PowerShell 窗口需要在调试期间保持运行。

然后在 VNC Viewer 中连接：

```text
127.0.0.1:5901
```

如果 Viewer 提示 VNC 本身未加密，可以确认继续，因为整个 VNC 流量已经封装在 SSH 加密
隧道内。

## 服务器服务管理

VNC 桌面和 Warlock 已分别注册为 `nrm-vnc.service` 与 `nrm-warlock.service`，登录服务器后
会自动启动。Warlock 服务依赖 VNC 服务，不需要手工设置 `DISPLAY`。

查看状态：

```bash
systemctl --user status nrm-vnc.service
systemctl --user status nrm-warlock.service
```

启动：

```bash
systemctl --user start nrm-vnc.service
systemctl --user start nrm-warlock.service
```

停止：

```bash
systemctl --user stop nrm-warlock.service
systemctl --user stop nrm-vnc.service
```

查看日志：

```bash
journalctl --user -u nrm-vnc.service -n 100 --no-pager
journalctl --user -u nrm-warlock.service -n 100 --no-pager
```

确认端口只监听本机：

```bash
ss -ltn | grep 5901
```

预期地址必须是 `127.0.0.1:5901` 或 `[::1]:5901`，不能是 `0.0.0.0:5901`。

## 启动 Warlock

普通运行：

```bash
/home/pyh/afsim/network_resource_manager/scripts/remote/run-warlock-remote.sh
```

指定场景：

```bash
/home/pyh/afsim/network_resource_manager/scripts/remote/run-warlock-remote.sh \
  /绝对路径/场景.txt
```

Warlock 中打开：

```text
View → Network Resource Manager
```

如果 Warlock 已由用户服务启动，修改代码并重新构建后用下列命令重新载入：

```bash
systemctl --user restart nrm-warlock.service
```

## GDB 调试

先停止后台 Warlock，再在服务器另一个 SSH 终端执行：

```bash
systemctl --user stop nrm-warlock.service
/home/pyh/afsim/network_resource_manager/scripts/remote/run-warlock-remote.sh --gdb
```

GDB 中常用命令：

```text
break WkNrm::Plugin::Plugin
break WkNrm::SimInterface::SimulationInitializing
break WkNrm::SimInterface::PublishSnapshot
run
bt
continue
```

界面仍显示在 Windows VNC Viewer 中，断点和调用栈在 SSH 终端操作。

## 当前渲染模式

远程启动器默认设置：

```text
LIBGL_ALWAYS_SOFTWARE=1
```

即使用 Mesa 软件 OpenGL，优先保证稳定性。后续需要大规模三维场景性能时，再单独接入
VirtualGL/NVIDIA GPU 加速，不直接改变当前稳定入口。

## AFSIM 2.9 兼容处理

启动器只在用户目录 `/home/pyh/.local/nrm-remote/opt/afsim` 下创建短路径运行布局，不修改
AFSIM 核心源码。每次启动会从现有构建目录刷新 Warlock 可执行文件，并链接既有 WSF、
Warlock 插件和资源。

当前 AFSIM 随附的 Qt 5.12 在新版本 glibc 上使用 `QLockFile` 时存在缓冲区长度兼容问题。
启动器会编译并仅向 Warlock 注入
`scripts/remote/qt512_readlink_compat.c`，限制越界的 `readlink` 长度。该兼容层不改变仿真
算法和插件接口，不影响命令行 `mission`。

## 已验证状态

2026-07-30 已在当前服务器验证：

- VNC 仅监听 `127.0.0.1:5901` 和 `[::1]:5901`。
- Openbox 桌面与 Mesa 软件 OpenGL 可用。
- Warlock 能打开 `test_mission/framework_smoke.txt` 并持续运行。
- WSF 插件 `libwsf_network_resource_manager_ln13m64.so` 已加载。
- Warlock 插件 `libNetworkResourceManager_ln13m64.so` 已加载。

## 回退

停止并禁用两个用户服务：

```bash
systemctl --user disable --now nrm-warlock.service nrm-vnc.service
```

用户目录安装位于：

```text
/home/pyh/.local/nrm-remote
```

早期启动失败产生的 Warlock 临时锁已移动到
`~/.local/share/United States Air Force (USAF)/Warlock/lock-backup-20260730-1723`，没有
删除用户设置。整个方案不需要修改或删除任何 AFSIM 核心源码。
