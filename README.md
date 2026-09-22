# WebSocketNotifier 🚀

[![Build Status](https://github.com/yourusername/WebSocketNotifier/actions/workflows/build.yml/badge.svg)](https://github.com/yourusername/WebSocketNotifier/actions)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

**WebSocketNotifier** is a lightweight, production‑ready C++ WebSocket server that broadcasts JSON‑encoded notifications to all connected clients. 
It demonstrates modern C++ (C++17), robust error handling, logging, configuration management, and unit testing.

---

## Features

- ✅ **Asynchronous WebSocket server** built on Boost.Beast/Boost.Asio 
- ✅ **JSON configuration** (`config.json`) parsed with *nlohmann/json* 
- ✅ **Structured logging** via *spdlog* (console + rotating file) 
- ✅ **Graceful shutdown** handling SIGINT/SIGTERM 
- ✅ **Thread‑safe client management** (broadcast to all connections) 
- ✅ **Unit tests** with GoogleTest 

---

## Prerequisites

| Tool | Version |
|------|---------|
| CMake | >= 3.14 |
| C++ compiler | GCC 9+, Clang 10+, MSVC 2019 |
| Boost | >= 1.70 (Asio + Beast) |
| spdlog | >= 1.8 |
| nlohmann_json | >= 3.9 |
| GoogleTest | (optional, for tests) |

---

## Installation