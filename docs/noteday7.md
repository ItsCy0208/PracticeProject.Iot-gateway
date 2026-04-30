# Day7 学习记录

## 今日目标

围绕 `on_message` 的 JSON 处理流程设计并执行测试用例。  
验证“合法输入 / 缺字段 / 非法 JSON / 类型错误”四类场景。  
巩固“先判断语法是否合法，再判断业务字段是否有效”的排错顺序。

## 今日完成内容

### 1) 环境与构建确认

- 执行 `cmake --build build -j`，构建成功。  
- 启动网关：`./build/iot-gateway/iot-gateway`。  
- 网关正常输出：
  - `[gateway] connected to broker`
  - `[gateway] listening on iot/test ...`

### 2) Day7 用例执行

在另一个终端使用 `mosquitto_pub` 向 `iot/test` 发布消息，观察网关日志。

#### 用例1：合法 JSON（通过）

发布：
`mosquitto_pub -h localhost -t iot/test -m '{"device":"sensor1","temp":27.5}'`

结果：
- `[gateway] topic=iot/test payload={"device":"sensor1","temp":27.5}`
- `[gateway] parsed device=sensor1 temp=27.5`

结论：解析与字段校验均通过。

#### 用例2：缺少 device（按预期报错）

发布：
`mosquitto_pub -h localhost -t iot/test -m '{"temp":27.5}'`

结果：
- `[gateway][error] missing/invalid field: device`

结论：字段存在性校验生效。

#### 用例3：缺少 temp（按预期报错）

发布：
`mosquitto_pub -h localhost -t iot/test -m '{"device":"sensor1"}'`

结果：
- `[gateway][error] missing/invalid field: temp`

结论：字段存在性校验生效。

#### 用例4：非法 JSON（按预期报错）

发布：
`mosquitto_pub -h localhost -t iot/test -m '{"device":sensor1,"temp":27.5}'`

结果：
- `[gateway][error] invalid json: ...parse_error...`

结论：JSON 语法错误会先被解析器拦截，进入 `catch`。

#### 用例5：我最初想测 temp 类型错误，但命令写成了非法 JSON

发布（本次执行）：
`mosquitto_pub -h localhost -t iot/test -m '{"device":sensor1,"temp":"27.5"}'`

结果：
- `[gateway][error] invalid json: ...parse_error...`

说明：
- 该输入的 `device` 没有引号，所以首先是“语法错误”，不是“类型错误”。

若要正确测试 “temp 是字符串” 的类型错误，应发布：
`mosquitto_pub -h localhost -t iot/test -m '{"device":"sensor1","temp":"27.5"}'`

预期：
- `[gateway][error] missing/invalid field: temp`

## 今日踩坑与解决

### 1) `failed to connect broker` 并不一定是代码错

之前遇到启动失败，后续确认是 `mosquitto` 服务未运行。  
排查顺序应固定为：服务状态 -> 端口监听 -> 程序日志。

### 2) “类型错误”与“语法错误”容易混淆

如果 JSON 语法本身不合法，程序会先在 `json::parse` 报错，根本走不到字段类型判断。  
要测试类型错误，必须先保证 JSON 语法正确。

## 今日结论

已完成 Day7 核心目标：  
- 能系统化设计并执行 JSON 处理测试用例。  
- 能解释不同错误路径：`parse_error`（语法层） vs `missing/invalid field`（业务校验层）。  
- 对网关消息处理逻辑理解更深入，可进入下一步“最小测试化”阶段。

## Day8 计划（预告）

把当前手工用例整理为固定测试清单（输入、预期、实际）。  
尝试将校验函数继续命名优化（如 `parse_and_validate_payload`）并保持行为不变。  
准备引入最小单元测试框架前的代码切分方案。
