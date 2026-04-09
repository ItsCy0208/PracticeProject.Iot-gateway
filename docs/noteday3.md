# Day3 学习记录
## 今日目标
统一网关日志顺序与格式
完成 MQTT payload 的 JSON 解析与字段校验
验证异常输入时程序可继续稳定运行

## 今日完成内容

### 1) 网关日志规范化
调整连接日志顺序为：
- [gateway] connected to broker
- [gateway] listening on iot/test ...

统一消息日志格式为：
- [gateway] topic=... payload=...

### 2) JSON 解析与字段校验
在 `iot-gateway/iot-gateway.cpp` 中新增：
- `#include <nlohmann/json.hpp>`
- `using json = nlohmann::json;`

在 `on_message` 中实现：
- `json::parse(payload)` 解析
- 校验 `device` 必须为 string
- 校验 `temp` 必须为 number
- 异常捕获并输出错误日志

### 3) 依赖与构建
安装依赖：
`sudo apt install -y nlohmann-json3-dev`

构建命令：
`cmake --build build -j`

## 测试与结果

### 用例1：正常 JSON
发布：
`mosquitto_pub -h localhost -t iot/test -m '{"device":"dev01","temp":25.1}'`

结果：
- [gateway] topic=iot/test payload={"device":"dev01","temp":25.1}
- [gateway] parsed device=dev01 temp=25.1

### 用例2：缺少 temp
发布：
`mosquitto_pub -h localhost -t iot/test -m '{"device":"dev01"}'`

结果：
- [gateway][error] missing/invalid field: temp

### 用例3：非法 JSON
发布：
`mosquitto_pub -h localhost -t iot/test -m '{"device":'`

结果：
- [gateway][error] invalid json: ...

## 今日踩坑与解决

### 旧输出与新代码不一致
现象：终端仍显示旧日志顺序
原因：运行到旧构建产物或源码未保存
解决：确认目标文件后重编译并重新运行

### 命令参数细节错误
现象：`mosquitto_pub` 报 unknown option
原因：`-m` 与消息内容之间缺少空格
解决：使用 `-m '{"device":"dev01","temp":25.1}'`

## 今日结论
Day3 核心功能已完成：
- 网关可稳定订阅并接收消息
- 可解析合法 JSON 并提取业务字段
- 对缺字段和非法 JSON 均能正确报错且不崩溃

## Day4 计划（预告）
把解析后的数据转发到本地 HTTP mock 服务
补充 README 中 Day3 测试截图与输出示例
准备下一个功能提交
