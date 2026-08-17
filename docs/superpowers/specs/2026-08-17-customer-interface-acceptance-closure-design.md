# 甲方进程内接口验收闭环设计

## 目标

在不修改 AFSIM 核心、不改动既有评估/规划算法、不引入 HTTP、TCP、WebSocket 或消息队列的前提下，补齐 NRM 作为甲方 AFSIM 进程内插件的接口边界与可执行验收证据。

## 设计决定

1. `EndpointSnapshot`、`LinkSnapshot` 增加明确的 `networkId`。旧 `networkName` 保留兼容，甲方资源解码、引用校验和网络统计统一使用 `networkId`。
2. `NavigationSample` 增加明确的 `platformId`。导航状态按 `platformId` upsert；旧 `platformName` 仅作兼容回退和显示。
3. 资源、导航、环境共用 `CustomerIngestionState` 和 `CustomerSnapshotAssembler`，同一 `runId` 增量合并，新运行清空旧状态，旧时刻、重复消息和已结束运行拒绝进入状态。
4. 环境上下文保留三态 `INFORMATION_ONLY`、`ALREADY_INCLUDED`、`CANDIDATE_ADJUSTMENT`。前两态不重复施加影响并输出不同证据，第三态才允许参数化候选修正。
5. `CustomerJsonCodec` 增加可替换运行时校验层。默认实现继续使用现有无第三方依赖的严格约束校验；所有 JSON 入口先经过这一层，再执行领域解码。
6. `network_plan.configVersion` 只由宿主当前配置绑定，JSON 解码值不可信也不进入内部计划。
7. 新增 `DataContainer::LoadCustomerJson` 端到端测试，覆盖资源、导航、环境、评估、规划及新快照使旧派生结果失效。

## 非目标

- 不实现任何外部传输协议。
- 不猜测甲方私有 C++ 类型或二进制 ABI。
- 不实现推荐自动转规划、虚拟快照 What-if 或自动控制链路。
- 不重构成熟的核心算法。

## 验收准则

- 两个同类型网络各自拥有端点和链路，跨网络引用必须拒绝。
- 同一运行内后到的资源更新不得抹除导航与环境；两个平台导航互不覆盖。
- 三种环境模式均可解析、保存并产生可区分的应用证据。
- 甲方评估请求从 JSON 入口到 Facade，再编码为响应 JSON；UI 与甲方接口共享同一 Facade。
- 规划配置版本等于宿主活动配置版本。
- 新快照到达后旧评估、能力、规划验证/推演和需求匹配结果全部失效。
- 新增及既有测试、完整 CTest、WSF/Warlock 构建和部署契约均通过。
