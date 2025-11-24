# Simple Discrete Event Simulator (SDES)

一个用 C 语言实现的简单离散事件模拟器，主要用于模拟网络数据包传输场景。

## 项目结构

```
.
├── src/                    # 源代码文件
│   ├── main.c             # 主程序入口
│   ├── event.c            # 事件结构实现
│   ├── event_scheduler.c  # 事件调度器（优先队列）
│   ├── event_loop.c       # 事件循环处理
│   ├── network_sim.c      # 网络模拟逻辑
│   └── packet.c           # 数据包结构实现
├── include/               # 头文件
├── build/                 # 编译输出目录
└── makefile              # 构建配置
```

## 编译项目

### 前置要求
- GCC 编译器
- Make 工具

### 编译步骤

```bash
# 编译项目
make

# 清理编译文件
make clean

# 重新编译
make clean && make
```

编译成功后，可执行文件位于 `build/sdes`

## 如何测试

本项目提供了多种测试模式，可以通过命令行参数选择：

### 1. 查看帮助信息

```bash
./build/sdes help
# 或
./build/sdes -h
# 或
./build/sdes --help
```

### 2. 基础事件调度器测试

测试事件调度器的基本功能，验证事件是否按时间顺序执行：

```bash
./build/sdes test
```

**预期结果：**
- 显示三个事件按时间顺序（10, 20, 30）执行
- 验证优先队列正确工作

### 3. Demo1 - 固定间隔网络模拟（默认模式）

模拟固定时间间隔发送数据包的场景：

```bash
./build/sdes demo1
# 或直接运行（默认为 demo1）
./build/sdes
```

**模拟参数：**
- 发送间隔：每 100 时间单位
- 模拟时长：2000 时间单位（约 20 个数据包）
- 网络延迟：10-50 时间单位（随机）
- 数据包大小：512 字节

**预期输出：**
- 显示发送者发送数据包的时间
- 显示网络转发数据包的延迟
- 显示接收者收到数据包的时间和延迟
- 最后输出统计信息（发送/转发/接收的数据包数量、字节数、平均延迟等）

### 4. Demo2 - 指数分布网络模拟

模拟泊松过程（指数分布）的数据包到达：

```bash
./build/sdes demo2
```

**模拟参数：**
- 发送模式：指数分布（λ = 0.01）
- 模拟时长：2000 时间单位
- 网络延迟：10-50 时间单位（随机）
- 数据包大小：512 字节

**特点：**
- 数据包到达时间间隔不固定
- 符合泊松过程特性
- 更贴近真实网络流量模式

### 5. Demo3 - 长时间运行模拟

测试模拟器在大量事件下的性能：

```bash
./build/sdes demo3
```

**模拟参数：**
- 发送间隔：每 10 时间单位
- 模拟时长：10000 时间单位（约 1000 个数据包）
- 网络延迟：5-20 时间单位（随机）
- 数据包大小：1024 字节

**用途：**
- 性能测试
- 验证大规模事件处理能力
- 检测内存泄漏

## 输出说明

### 事件执行输出
```
[Sender] Packet 0 sent at time 100 (size: 512 bytes)
[Network] Forwarding packet 0 with delay 35
[Receiver] Packet 0 received at time 135 (latency: 35)
```

### 统计信息输出
```
=== Simulation Statistics ===
Sender Statistics:
  Total packets sent: 20
  Total bytes sent: 10240
  
Network Statistics:
  Total packets forwarded: 20
  Total bytes forwarded: 10240
  
Receiver Statistics:
  Total packets received: 20
  Total bytes received: 10240
  Average latency: 29.5 time units
  Last packet latency: 32 time units
```

## 验证测试

### 1. 功能正确性验证
- ✅ 事件按时间顺序执行
- ✅ 数据包 ID 连续且唯一
- ✅ 接收数据包数 = 发送数据包数
- ✅ 延迟在指定范围内（min_delay 到 max_delay）

### 2. 性能测试
运行 demo3 并观察：
- 执行时间是否合理
- 内存使用是否稳定
- 无崩溃或段错误

### 3. 内存检查（可选）
如果安装了 valgrind：

```bash
valgrind --leak-check=full ./build/sdes demo1
```

应该没有内存泄漏报告。

## 调试模式

项目已在 makefile 中启用 `-g` 调试标志，可以使用 GDB 进行调试：

```bash
gdb ./build/sdes

# 在 GDB 中
(gdb) run demo1
(gdb) break main
(gdb) break sender_task
(gdb) continue
```

## 作者备注

> I think these functions are easy to read. C'est facile XD
> —— packet.c 作者留言 😄
> (哈哈，AI这都给我录上了)

## 许可证

本项目仅供学习使用。

---

# Simple Discrete Event Simulator (SDES)

A simple discrete event simulator implemented in C, primarily designed for simulating network packet transmission scenarios.

## Project Structure

```
.
├── src/                    # Source code files
│   ├── main.c             # Main program entry
│   ├── event.c            # Event structure implementation
│   ├── event_scheduler.c  # Event scheduler (priority queue)
│   ├── event_loop.c       # Event loop processing
│   ├── network_sim.c      # Network simulation logic
│   └── packet.c           # Packet structure implementation
├── include/               # Header files
├── build/                 # Build output directory
└── makefile              # Build configuration
```

## Building the Project

### Prerequisites
- GCC compiler
- Make tool

### Build Steps

```bash
# Build the project
make

# Clean build files
make clean

# Rebuild
make clean && make
```

After successful compilation, the executable is located at `build/sdes`

## How to Test

This project provides multiple test modes that can be selected via command-line arguments:

### 1. View Help Information

```bash
./build/sdes help
# or
./build/sdes -h
# or
./build/sdes --help
```

### 2. Basic Event Scheduler Test

Test the basic functionality of the event scheduler and verify events execute in time order:

```bash
./build/sdes test
```

**Expected Results:**
- Display three events executing in time order (10, 20, 30)
- Verify priority queue works correctly

### 3. Demo1 - Fixed Interval Network Simulation (Default Mode)

Simulate packet transmission at fixed time intervals:

```bash
./build/sdes demo1
# or run directly (defaults to demo1)
./build/sdes
```

**Simulation Parameters:**
- Send interval: Every 100 time units
- Simulation duration: 2000 time units (~20 packets)
- Network delay: 10-50 time units (random)
- Packet size: 512 bytes

**Expected Output:**
- Display sender packet transmission times
- Display network forwarding delays
- Display receiver packet arrival times and latencies
- Final statistics (packet count, bytes, average latency, etc.)

### 4. Demo2 - Exponential Distribution Network Simulation

Simulate Poisson process (exponential distribution) packet arrivals:

```bash
./build/sdes demo2
```

**Simulation Parameters:**
- Send mode: Exponential distribution (λ = 0.01)
- Simulation duration: 2000 time units
- Network delay: 10-50 time units (random)
- Packet size: 512 bytes

**Features:**
- Variable packet arrival intervals
- Follows Poisson process characteristics
- More realistic network traffic patterns

### 5. Demo3 - Long Running Simulation

Test simulator performance with large numbers of events:

```bash
./build/sdes demo3
```

**Simulation Parameters:**
- Send interval: Every 10 time units
- Simulation duration: 10000 time units (~1000 packets)
- Network delay: 5-20 time units (random)
- Packet size: 1024 bytes

**Purpose:**
- Performance testing
- Verify large-scale event handling capabilities
- Detect memory leaks

## Output Explanation

### Event Execution Output
```
[Sender] Packet 0 sent at time 100 (size: 512 bytes)
[Network] Forwarding packet 0 with delay 35
[Receiver] Packet 0 received at time 135 (latency: 35)
```

### Statistics Output
```
=== Simulation Statistics ===
Sender Statistics:
  Total packets sent: 20
  Total bytes sent: 10240
  
Network Statistics:
  Total packets forwarded: 20
  Total bytes forwarded: 10240
  
Receiver Statistics:
  Total packets received: 20
  Total bytes received: 10240
  Average latency: 29.5 time units
  Last packet latency: 32 time units
```

## Validation Testing

### 1. Functional Correctness Verification
- ✅ Events execute in time order
- ✅ Packet IDs are sequential and unique
- ✅ Packets received = Packets sent
- ✅ Delays within specified range (min_delay to max_delay)

### 2. Performance Testing
Run demo3 and observe:
- Execution time is reasonable
- Memory usage is stable
- No crashes or segmentation faults

### 3. Memory Check (Optional)
If valgrind is installed:

```bash
valgrind --leak-check=full ./build/sdes demo1
```

Should report no memory leaks.

## Debug Mode

The project has `-g` debug flag enabled in the makefile for GDB debugging:

```bash
gdb ./build/sdes

# In GDB
(gdb) run demo1
(gdb) break main
(gdb) break sender_task
(gdb) continue
```

## License

This project is for educational purposes only.

