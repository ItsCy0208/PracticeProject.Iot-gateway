# Day6 学习记录

## 今日目标

完成 `on_message` 的小重构：把 JSON 校验逻辑拆成独立函数。  
理解并修复 C++ 常见编译错误（函数声明顺序 / 作用域）。  
完成一次“编译通过 + 运行验证 + 环境排障”的完整闭环。

## 今日完成内容

### 1) 代码重构：提取校验函数

在 `iot-gateway.cpp` 中将消息处理逻辑分层：
- `on_message` 负责流程编排（取 payload、调用校验、打印结果）
- `on_message_err_temp(...)` 负责解析 JSON 与字段校验（`device` / `temp`）

重构收益：
- `on_message` 更短、更清晰
- 校验逻辑集中，后续更容易测试和复用

### 2) 编译报错定位与修复

报错现象：
- `on_message_err_temp was not declared in this scope`

原因：
- 在 `on_message` 中先调用函数，但函数定义写在后面，且前面没有声明。

修复：
- 在文件上方添加前置声明：  
  `bool on_message_err_temp(const std::string& payload, std::string& device, double& temp);`

结论：
- 这是 C++ 中“声明先于使用”的典型规则问题。

### 3) 构建与运行验证

执行构建：
- `cmake --build build -j`
- 结果：`[100%] Built target iot-gateway`

运行初次失败：
- 输出 `failed to connect broker`

排查步骤：
1. `sudo service mosquitto status` -> 发现 broker 未运行  
2. `sudo service mosquitto start` -> 启动成功  
3. `ss -lntp | grep 1883` -> 确认端口监听  
4. 重新运行网关 -> 出现 `connected` / `listening`

结论：
- 这次问题是运行环境（服务未启动），不是代码逻辑错误。

## 测试与结果

### 用例：网关启动

结果：
- `[gateway] connected to broker`
- `[gateway] listening on iot/test ...`

### 用例：发布合法 JSON（建议复测）

发布：
`mosquitto_pub -h localhost -t iot/test -m '{"device":"day6","temp":26.6}'`

预期：
- `[gateway] topic=iot/test payload=...`
- `[gateway] parsed device=day6 temp=26.6`

## 今日踩坑与解决

### 1) 函数声明顺序

现象：编译器提示函数未声明。  
解决：先写函数声明，再在下方给函数定义。

### 2) 代码问题与环境问题混淆

现象：编译通过但运行连接失败。  
解决：先查服务状态与端口监听，再判断代码是否有问题。

### 3) 验证习惯

改完代码后要固定执行：
- 构建是否通过
- 服务是否运行
- 功能是否按预期输出

## 今日结论

Day6 最大收获是：不仅完成了函数拆分，还学会了把问题分成“编译层”和“运行环境层”分别排查。  
当前网关功能可正常构建和运行，重构后的结构更适合 Day7 做测试化练习。

## Day7 计划（预告）

围绕 `on_message_err_temp(...)` 设计 3-5 组输入用例（合法、缺字段、类型错误、非法 JSON）。  
先做“手工测试表 + 预期输出”，再考虑引入最小单元测试框架。