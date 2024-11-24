# slog
```
Wrapping of spdlog
```

# why
```
spdlog is excellent, but not quite what I need for my business scenarios
```

# requirement
- Log output by module
- Real-time updating of log levels
- 
# test
## 封装后接口性能
```
./slog_test_slog 
Starting single-threaded logging test...
[2024-11-24 14:26:18.573] [info] Single-threaded Test - Average time per log: 0.006342 ms
[2024-11-24 14:26:18.573] [info] Single-threaded Test - 95% time per log: 0.010672 ms
Starting multi-threaded logging test...
[2024-11-24 14:26:49.335] [info] Multi-threaded Test - Average time per log: 0.029846 ms
[2024-11-24 14:26:49.335] [info] Multi-threaded Test - 95% time per log: 0.012408 ms
Starting single-threaded logging test with define...
[2024-11-24 14:26:56.380] [info] Single-threaded-define Test - Average time per log: 0.006984 ms
[2024-11-24 14:26:56.380] [info] Single-threaded-define Test - 95% time per log: 0.011545 ms
```

## 原始接口性能
```
./slog_test_spdlog 
2024-11-24 14:21:13.595 [info] Starting single-threaded logging test...
2024-11-24 14:21:14.121 [info] Single-threaded Test - Average time per log: 0.000463 ms
2024-11-24 14:21:14.121 [info] Single-threaded Test - 95% time per log: 0.001073 ms
2024-11-24 14:21:14.122 [info] Starting multi-threaded logging test...
2024-11-24 14:21:19.152 [info] Multi-threaded Test - Average time per log: 0.004770 ms
2024-11-24 14:21:19.152 [info] Multi-threaded Test - 95% time per log: 0.018229 ms
2024-11-24 14:21:19.155 [info] Performance test completed.
```