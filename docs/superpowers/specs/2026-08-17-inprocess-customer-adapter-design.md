# 甲方 AFSIM 同进程适配器设计

## 定位

NRM 是甲方修改版 AFSIM 内部插件，不是独立服务。运行时主链使用 C++ 值对象和直接函数
调用；JSON 仅用于接口规范、测试、回放、文件导入导出和验收证据。

## 两层边界

```text
甲方 AFSIM 私有对象
  -> 甲方专用 ContractInterfaceAdapter（拿到甲方头文件后实现）
  -> NRM 公共强类型对象
  -> CustomerNrmAdapter（本轮实现的同进程状态与服务入口）
  -> ModelServiceFacade / Repository / ResourceSnapshot
```

`CustomerNrmAdapter`不依赖Qt、JSON或AFSIM裸指针。它处理消息幂等、运行切换、按域状态
合并，并把评估、规划和并发需求交给宿主的统一服务入口。`DataContainer`实现宿主接口，
因此Warlock、JSON回放和未来甲方私有适配器可以进入同一业务主链。

`CustomerJsonCodec`只负责JSON与公共强类型对象的双向转换，不再命名为合同运行时适配器。

## 所有权和线程规则

- 接口参数和返回值均为值对象，不把AFSIM或Qt指针交给甲方模块。
- 调用方保留输入所有权；NRM在调用期间复制并发布新快照。
- `DataContainer`入口属于GUI/模型编排线程。甲方若从仿真回调线程调用，必须先沿现有
  SimEvent值对象桥投递，不能跨线程直接操作界面状态。
- 同进程不等于ABI天然兼容；甲方与NRM仍须使用相同AFSIM、编译器、标准库和构建配置。

## 当前可实现与后置部分

本轮实现公共C++入口、参考测试和DataContainer接入。甲方私有类型到公共对象的字段转换
必须等甲方头文件/对象定义到位后，在独立薄适配器中完成，不允许猜测其类布局。

规划建议保持只读。没有潜在链路和动作定义时不生成候选规划，也不构造虚假虚拟拓扑。
