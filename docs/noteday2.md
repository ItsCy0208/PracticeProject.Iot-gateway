# Day2 学习记录
## 今日目标
熟悉 Linux 常用命令并能在 WSL 中独立操作
完成 MQTT 本地发布/订阅联调
巩固 CMake 构建流程与 Git 基本操作

## 今日完成内容

### 1) Linux/WSL 基础命令实操
使用了 pwd、ls、cd、mkdir -p、cat、less
理解了家目录 ~ 对应 /home/caoyue
解决了权限问题：/aaa/... 需要 sudo，个人练习目录应使用 ~/...
理解了 cp 的源和目标顺序，以及目录复制要用 -r

### 2) MQTT 本地联调
确认 mosquitto 服务状态为 active (running)

订阅命令：
mosquitto_sub -h localhost -t "iot/test" -v

发布命令：
mosquitto_pub -h localhost -t iot/test -m hello

成功收到消息：
iot/test hello

排错点：
topic 名必须完全一致（iot/test 与 iot/tes 不同）
wsl -d Ubuntu 应在 PowerShell 中执行，不在 Ubuntu shell 内执行

### 3) CMake 构建认知巩固
cmake -S . -B build：生成构建系统（配置阶段）
cmake --build build -j：执行编译链接（构建阶段）
能区分“生成构建文件”和“实际编译”的职责

### 4) Git 状态认知
理解 git log 只展示历史，不会自动生成提交
知道 git commit 才会创建提交
学会通过 git status 判断当前是否有未提交改动

## 今日踩坑与解决

### 权限错误
现象：mkdir -p /aaa/zhuzhu 报 Permission denied
原因：在根目录创建文件夹需要管理员权限
解决：改用 mkdir -p ~/aaa/zhuzhu 或加 sudo

### topic 拼写错误
现象：发布后订阅端无输出
原因：订阅了 iot/tes，发布到 iot/test
解决：统一 topic 为 iot/test

### 命令执行环境混淆
现象：在 Ubuntu 内执行 wsl -d Ubuntu 报命令不存在
原因：wsl 是 Windows 命令
解决：在 PowerShell 执行 wsl -d Ubuntu

## 今日结论
已具备在 WSL 中完成 IoT 项目基础开发的操作能力
MQTT 本地消息链路稳定可复现
可进入 Day3（模块化整理 + README 文档化）

## Day3 计划（预告）
整理项目目录结构与 README 运行步骤
规范化启动流程（先 broker，再 gateway，再 pub）
准备 JSON 解析与字段校验功能实现