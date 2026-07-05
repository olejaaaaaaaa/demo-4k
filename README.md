# 4K Demo Project

This is a small **demoscene project template** for creating simple real-time graphics demos in C, focused on size-optimized builds (4KB-style experiments).

It is designed for Windows only.

## 💡 What is this?

This project is a lightweight framework for:
- writing small demoscene productions in C
- experimenting with real-time graphics (OpenGL)
- building minimal-size executables (Crinkler-based workflow)
- learning how procedural generation works in demos

---

## ⚙️ Requirements

To build this project you need:

- Windows (x86 / x64)
- **Clang (clang-cl.exe)** installed and available in `PATH`
- Python 3.x (for build automation script)

---

## 🚀 How to build

1. Clone the repository:

```bash
git clone https://github.com/olejaaaaaaaa/retro-4k.git
cd retro-4k
python build.py
./build/demo.exe
```