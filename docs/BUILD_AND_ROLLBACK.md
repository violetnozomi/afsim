# 构建、版本与回退

## 原则

- 插件是独立 Git 仓库。
- AFSIM 主源码不提交、不复制、不打补丁。
- 使用 `WSF_ADD_EXTENSION_PATH` 让 CMake 发现外部插件。
- 开发构建只修改构建目录中的 CMake Cache 和生成文件。

## 接入现有构建目录

```bash
AFSIM_SOURCE=/home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src
AFSIM_BUILD=${AFSIM_SOURCE}/build-ubuntu24
NRM_SOURCE=/home/pyh/afsim/network_resource_manager

cmake -S "${AFSIM_SOURCE}" -B "${AFSIM_BUILD}" \
  -DWSF_ADD_EXTENSION_PATH="${NRM_SOURCE}" \
  -DBUILD_WARLOCK_PLUGIN_NetworkResourceManager=TRUE

cmake --build "${AFSIM_BUILD}" \
  --target wsf_network_resource_manager NetworkResourceManager nrm_framework_types_test

ctest --test-dir "${AFSIM_BUILD}" -R nrm_framework_types_test --output-on-failure
```

上述变量名仅用于说明。执行危险操作时应使用已核对的绝对路径，不使用宽泛目录或通配符。
Linux 开发构建会把 Warlock 插件复制到构建目录的 `warlock_plugins/`；该目录属于生成物，不会
写入 AFSIM 主源码。正式部署仍以 `cmake --install` 的安装结果为准。

## 回退插件代码

查看可用版本：

```bash
git -C /home/pyh/afsim/network_resource_manager tag --list
```

为当前未提交工作建立临时分支后回到稳定版本：

```bash
git -C /home/pyh/afsim/network_resource_manager switch -c backup/wip-YYYYMMDD
git -C /home/pyh/afsim/network_resource_manager add -A
git -C /home/pyh/afsim/network_resource_manager commit -m "backup: preserve work in progress"
git -C /home/pyh/afsim/network_resource_manager switch --detach v0.1.0-framework
```

推荐开发时从稳定标签创建修复分支，不在分离头状态长期开发：

```bash
git -C /home/pyh/afsim/network_resource_manager switch -c fix/example v0.1.0-framework
```

## 从 AFSIM 构建中停用

停用外部扩展只修改构建缓存，不触碰 AFSIM 源文件：

```bash
cmake -S /home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src \
  -B /home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build-ubuntu24 \
  -DWSF_ADD_EXTENSION_PATH= \
  -DBUILD_WARLOCK_PLUGIN_NetworkResourceManager=FALSE
```

已生成的插件库可以保留以便排查。若必须清理，只删除经过核对的两个具体插件产物，不对整个
构建目录执行递归删除。

## 版本策略

- `v0.x.y-framework`：框架和接口里程碑。
- `v0.x.y-demo`：可演示功能里程碑。
- `v1.0.0-acceptance`：验收冻结版本。
- 每次打标签前至少完成配置、两个目标编译和一次加载验证。
- 修改公共数据契约时同步更新 `VERSION` 和 `CHANGELOG.md`。
