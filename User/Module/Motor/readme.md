(由AI生成，不完全代表本人看法，仅供参考(~~懒得写注释~~))
# MotorSts 电机驱动库文档

本库用于驱动 STS 系列总线舵机，提供了相对用户坐标系接口，旨在使使用者更方便地驯服飞特STS3215系列总线舵机。

<!-- 飞特你真的是个大件货！大件货啊！这都是什么牛魔酬宾协议！ -->

## 命名规则

为了防止在控制逻辑中混淆物理位置和编码器原始值，本类采用了 **Hard** (硬) 与 **Soft** (软) 的命名区分：

### 1. Hard (硬值 - 原始值)
*   **定义**：`Hard` 代表从电机直接读取的**原始数据**（Raw Data）。
*   **特点**：未经过任何逻辑处理，直接对应电机内部的寄存器值。
*   **使用场景**：通常用于底层调试、数据记录或计算速度/加速度。

### 2. Soft (软值 - 逻辑值)
*   **定义**：`Soft` 代表经过**逻辑处理后的数据**。
*   **处理逻辑**：
    1.  **方向处理**：根据构造函数中的 `reversed` 参数，自动翻转正负号，使得代码中的正方向始终对应用户期望的机械正方向。
    2.  **零点处理**：根据构造函数中的 `zero_point` (软件零点)，将原始编码器值转换为相对于零点的偏移量。
    3.  **软限位**：所有 `Soft` 值的设定都会经过 `soft_min_pos` 和 `soft_max_pos` 的范围检查。
*   **使用场景**：**这是算法层（如运动学、轨迹规划）应该使用的标准接口。**

### 3. One (归一化值)
*   **定义**：`One` 代表归一化后的浮点数值。
*   **用途**：开合控制？不知道...

---

## 接口速查 

| 函数名                               | 返回/参数类型    | 功能描述                                 | 数据流向        |
|:----------------------------------|:-----------|:-------------------------------------|:------------|
| 位置设定 (Setters)                    |            |                                      |             |
| `SetSoftTargetPos_Ecd(int16_t t)` | `void`     | 设定目标位置 (编码器值)。设置相对于软件零点的目标位置。        | Soft → Hard |
| `SetSoftTargetPos_Rad(float t)`   | `void`     | 设定目标位置 (弧度)。自动转换为编码器值后调用上一个函数。       | Soft → Hard |
| `SetSoftTargetPos_One(float t)`   | `void`     | 设定目标位置 (归一化 0-1)。根据软限位范围映射后设定。       | Soft → Hard |
| `SetSoftTargetVel_Ecd(int16_t t)` | `void`     | 设定目标速度 (编码器值/秒)。已包含方向反转逻辑。           | Soft → Hard |
| `SetSoftTargetVel_Rad(float t)`   | `void`     | 设定目标速度 (弧度/秒)。自动转换单位。                | Soft → Hard |
| 位置获取 (Getters - Soft)             |            |                                      |             |
| `GetSoftTargetPos_Ecd()`          | `int16_t`  | 获取目标位置 (编码器值)。当前逻辑层设定的目标值（相对零点）。     | Hard → Soft |
| `GetSoftTargetPos_Rad()`          | `float`    | 获取目标位置 (弧度)。                         | Hard → Soft |
| `GetSoftTargetPos_One()`          | `float`    | 获取目标位置 (归一化 0-1)。                    | Hard → Soft |
| `GetSoftPos_Ecd()`                | `int16_t`  | 获取当前位置 (编码器值)。当前电机实际物理位置（相对零点）。      | Hard → Soft |
| `GetSoftPos_Rad()`                | `float`    | 获取当前位置 (弧度)。                         | Hard → Soft |
| 速度获取 (Getters - Soft)             |            |                                      |             |
| `GetSoftTargetVel_Ecd()`          | `int16_t`  | 获取目标速度 (编码器值/秒)。                     | Hard → Soft |
| `GetSoftTargetVel_Rad()`          | `float`    | 获取目标速度 (弧度/秒)。                       | Hard → Soft |
| `GetSoftVel_Ecd()`                | `int16_t`  | 获取实际速度 (编码器值/秒)。                     | Hard → Soft |
| `GetSoftVel_Rad()`                | `float`    | 获取实际速度 (弧度/秒)。                       | Hard → Soft |
| 原始数据获取 (Getters - Hard)           |            |                                      |             |
| `GetHardTargetPos_Ecd()`          | `uint16_t` | 获取原始目标位置。直接从寄存器读取的未处理值（绝对坐标）。        | Raw         |
| `GetHardTargetVel_Ecd()`          | `uint16_t` | 获取原始目标速度。发送给电机的原始指令值（已处理方向但未处理零点偏移）。 | Raw         |
| `GetHardPos_Raw()`                | `uint16_t` | 获取原始位置。电机反馈的 Raw 编码器计数。              | Raw         |
| `GetHardVel_Raw()`                | `uint16_t` | 获取原始速度。电机反馈的 Raw 速度计数。               | Raw         |
| 安全检查                              |            |                                      |             |
| `IsSafePos_Ecd(int16_t p)`        | `bool`     | 检查位置安全性 (编码器值)。判断给定的编码器值是否在软限位范围内。   | -           |
| `IsSafePos_Rad(float p)`          | `bool`     | 检查位置安全性 (弧度)。判断给定的弧度值是否在软限位范围内。      | -           |
| 状态查询                              |            |                                      |             |
| `IsReversed()`                    | `bool`     | 查询是否反转。返回构造函数中设置的方向模式。               | -           |
| `GetID()`                         | `uint8_t`  | 获取电机 ID。                             | -           |

> **注意**：所有 `Set` 操作仅修改缓存值，必须调用 `ControlAll()` 才会通过串口发送指令。