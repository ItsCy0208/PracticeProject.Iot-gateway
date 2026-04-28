# Day4 学习记录

## 今日目标

读懂 `iot-gateway.cpp` 中 **main → on_connect → on_message** 的执行关系  
分清 MQTT **主题（topic）** 与 **消息体（payload / `-m`）** 在命令行与代码中的对应关系  
掌握 Bash 下 **`mosquitto_pub` 引号与反引号** 的常见坑，避免 Shell 进入 `>` 续行状态  
巩固 **`mosquitto_sub` / `mosquitto_pub`** 与网关同时订阅时的联调方式  

## 今日完成内容

### 1) 代码阅读：三块一句话模型

- **main（`mosquitto_lib_init` 至 `mosquitto_loop_start`）**  
  初始化库 → 创建客户端 → 注册 `on_connect` / `on_message` → 连接 broker → 启动后台网络循环，由库在适当时机调用回调。

- **on_connect**  
  连接结果由库回调；`rc == 0` 表示成功，此时调用 `mosquitto_subscribe(mosq, …, "iot/test", 0)` 完成订阅。

- **on_message**  
  从 `msg->payload` 得到字符串 → `json::parse` → 校验 `device`（string）、`temp`（number）→ 打印或报错；解析失败走 `catch`。

### 2) MQTT 概念与命令对照

| 概念 | 程序里 | 命令行 |
|------|--------|--------|
| 主题 | `"iot/test"` 传给 `mosquitto_subscribe` | `mosquitto_sub` / `mosquitto_pub` 的 **`-t iot/test`** |
| 消息体 | `msg->payload` | **`mosquitto_pub` 的 `-m '...'`**（不是 `-t`） |

理解：**`mosquitto_sub` 无输出** 通常表示尚无人向该 topic 发布；需另开终端执行 `mosquitto_pub`。

### 3) 多订阅者联调

同时运行：

- 终端 A：`./build/iot-gateway/iot-gateway`  
- 终端 B：`mosquitto_sub -h localhost -t iot/test -v`  

再在终端 C `mosquitto_pub` 到 `iot/test`，**A 与 B 均可收到**同一条消息（各打印各的）。

### 4) Bash 与 `mosquitto_pub` 踩坑复盘

- **双引号 `"..."` 内反引号 `` ` `` 仍会触发命令替换**，易导致 `>` 续行、`mosquitto_pub` 未真正执行完毕。  
- **含反引号或非 JSON 纯文本**：优先用 **单引号** 包住 `-m` 内容，例如 `-m 'what`s up'`。  
- **多行或未闭合引号**：Shell 出现 `>` 时，用 **Ctrl+C** 放弃当前行后重输完整命令。

### 5) 头文件作用（入门备忘）

- `<mosquitto.h>`：MQTT 客户端 API（连接、订阅、循环、回调）。  
- `<nlohmann/json.hpp>`：将 JSON 文本解析为 `json` 对象并校验字段。  
- `<thread>`：本示例中 `std::this_thread::sleep_for`，配合 `mosquitto_loop_start` 使主线程不立即退出。

## 测试与结果（摘录）

### 合法 JSON

发布：

`mosquitto_pub -h localhost -t iot/test -m '{"device":"wtf","temp":27.5}'`

结果：`parsed device=wtf temp=27.5`（程序只校验类型与字段存在，不校验字符串“是否像设备名”）。

### 非 JSON 文本

发布：

`mosquitto_pub -h localhost -t iot/test -m "wcnm"`

预期：`[gateway][error] invalid json: ...`（需网关进程在跑且观察运行网关的终端）。

## 今日踩坑与解决

### 把 JSON 错当成 topic

现象：`mosquitto_sub -t '{"device":...}'` 长期无消息。  
原因：`-t` 只能是**主题名**，不能把 payload 写在 `-t` 里。  
解决：`-t iot/test`，JSON 仅放在 `mosquitto_pub` 的 `-m` 中。

### 误把 Shell 续行当程序卡死

现象：输入含 `` ` `` 的 `-m "what`s up"` 后出现多个 `>`。  
原因：Bash 引号与反引号规则，命令未完整结束。  
解决：单引号包裹 payload 或对反引号转义；`Ctrl+C` 后重敲。

### 在 `/mnt/c/...` 上构建出现 Clock skew

现象：`gmake` 提示 modification time in the future。  
原因：Windows 挂载盘与 WSL 时钟/时间戳差异。  
解决：全量重建可缓解；长期可在 `~/` 下克隆工程再构建以减少干扰。

## 今日结论

已能用自己的话描述 **连接 → 订阅 → 收消息 → 解析 JSON** 的整条链路。  
能正确使用 **`mosquitto_sub` / `mosquitto_pub`**，并区分 **topic 与 payload**。  
理解 **回调由库调用**、**主线程 `sleep` 用于保持进程存活**，而非“轮询收消息”。

## Day5 计划（预告）

对齐 **CMake `target_link_libraries` 与实际使用的库**（例如 nlohmann 与 cjson 是否重复或未使用）。  
（可选）将解析后的合法数据 **POST 到本地 HTTP mock 服务**，并记录请求与响应。  
为 `on_message` 内校验逻辑整理 **小函数 + 单元测试** 的可行切分方式（先写在笔记里再落代码）。
