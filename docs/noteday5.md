# Day5 学习记录

## 今日目标

读懂顶层与 `iot-gateway/` 子目录中的 `CMakeLists.txt` 各自职责。  
初步理解 CMake 最常用语句（`project`、`add_subdirectory`、`add_executable`、`target_link_libraries`）的含义。  
完成一次“改代码 -> 重新构建 -> 运行验证”的最小闭环。  
完成一次最小 Git 提交（至少提交 `docs/noteday5.md`）。  

## 今日任务规划

### 1) 先建立“两个 CMakeLists 分工图”

阅读文件：
- 顶层：`CMakeLists.txt`
- 子目录：`iot-gateway/CMakeLists.txt`

在纸上写 4 句话：
- 顶层文件负责什么（总项目入口、包含子目录）。
- 子目录文件负责什么（生成可执行文件、链接库）。
- 可执行文件叫什么（`iot-gateway`）。
- 代码由哪些源文件组成（`iot-gateway.cpp`、`iot-gateway.h`）。

作答：
- 顶层文件负责设置 CMake 最低版本、定义项目名，并通过 `add_subdirectory("iot-gateway")` 纳入子项目。
- 子目录文件负责定义可执行文件（由哪些源文件构成）、设置 C++ 标准、查找线程包并链接所需库。
[批注] 你的理解方向正确；“进入子目录执行 cmake”更准确地说是“将子目录作为构建目标纳入同一构建系统”。

### 2) 依赖对照练习

对照：
- `iot-gateway.cpp` 里的 `#include`
- `target_link_libraries(...)` 里的库名

写一张小表（手写即可）：
- 库/头文件名
- 代码里是否直接使用
- 当前结论（确认在用 / 待确认 / 可能冗余）

建议重点看：
- `mosquitto`（代码中明确使用）
- `Threads::Threads`（线程相关）
- `cjson`、`iniparser`（目前源码里未直接使用，先标注“待确认”）

作答：
- `iot-gateway.h`：项目自定义头文件，目前内容很少，属于项目结构组成部分。
- `<chrono>`：时间相关工具，代码中用于 `std::chrono::seconds(1)`，确认使用。
- `<iostream>`：标准输入输出流，代码中用于日志打印，确认使用。
- `<mosquitto.h>`：MQTT 客户端库头文件，提供连接、订阅、回调等 API，确认使用。
- `<nlohmann/json.hpp>`：JSON 解析库头文件，代码中用于 `json::parse(payload)` 与字段校验，确认使用。
- `<thread>`：线程相关库，代码中使用 `std::this_thread::sleep_for`，确认使用。
[批注] 这里是“头文件使用对照”，不是“target_link_libraries 库对照”。若补充库对照可写：`mosquitto`、`Threads::Threads` 在用；`cjson`、`iniparser` 当前源码未直接使用（待确认/可能冗余）。


### 3) 构建与运行闭环

在 WSL 终端执行：

1. `cmake -S . -B build`  
2. `cmake --build build -j`  
3. `./build/iot-gateway/iot-gateway`  
4. 另开终端发布：
   `mosquitto_pub -h localhost -t iot/test -m '{"device":"day5","temp":26.3}'`

记录观察结果：
- 是否出现 `connected` / `listening`
- 是否正确打印 `parsed device=day5 temp=26.3`

### 4) Git 最小闭环

执行：
- `git status`
- `git add docs/noteday5.md`
- `git commit -m "docs: add day5 learning plan"`

目标：今天先学会“有计划 -> 有记录 -> 有提交”的闭环，不追求一次改很多代码。

## 今日验收标准（满足 3 条即合格）

- 能用自己的话解释顶层与子目录 `CMakeLists.txt` 的分工。  
- 能说明 `cmake -S . -B build` 与 `cmake --build build` 的区别。  
- 成功跑通一次 MQTT 发布到网关解析。  
- 完成一次最小 Git 提交。  

## 常见踩坑与预案

### 1) 看到 `Clock skew detected`

这通常是 `/mnt/c/...` 时间戳差异导致的警告，不一定是构建失败。  
先看是否已 `Built target iot-gateway`，必要时删除 `build` 后重建。

### 2) `mosquitto_sub` 或网关“没反应”

先检查是否有人发布到同一 topic。  
确认主题名完全一致：`iot/test`。  
确认 mosquitto 服务是 `active (running)`。

### 3) Shell 出现 `>` 续行

通常是引号未闭合或反引号导致。  
按 `Ctrl+C` 取消，重新输入完整命令；JSON 建议用单引号包裹。

## 今日输出模板

### 我今天新理解的2个点

1. `cmake -S . -B build` 是“配置/生成构建系统”，`cmake --build build` 是“实际编译链接”。
2. Git 的最小闭环是 `status -> add -> commit`；`push` 只是把本地提交同步到远端，不是每天必须第一步做的动作。
[批注] 第二点建议写成“流程认知”，避免“push 有害”这类绝对化表述。

### 我今天执行过的关键命令

- `cmake -S . -B build`
- `cmake --build build -j`
- `./build/iot-gateway/iot-gateway`
- `mosquitto_pub -h localhost -t iot/test -m '{"device":"day5","temp":26.3}'`
- `git status` / `git add docs/noteday5.md` / `git commit -m "docs:添加Day5学习笔记"`
[批注] 建议把“计划执行”与“已执行”区分开；提交前确保上述 Git 命令至少执行到 `commit`。

### 我今天遇到的 1 个问题与解决

问题：  CmakeLists中find_package(Threads REQUIRED)为什么在target_link_libraries(...)以Threads::Threads出现？
解决：  `find_package(Threads REQUIRED)` 会让 CMake 发现当前平台正确的线程实现；`Threads::Threads` 是 CMake 提供的“导入目标”，使用它可以自动携带正确编译/链接参数（不同平台可能不是同一个库名），比手写 `-lpthread` 更稳妥。
[批注] 你“封装避免错链”的方向是对的，这里把原因补成了更标准的 CMake 解释。

### 明天（Day6）预告

深入理解 `on_message` 的字段校验逻辑，尝试拆出一个“纯校验函数”的草稿。  