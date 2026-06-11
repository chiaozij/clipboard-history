# 📋 Clipboard History — Windows 剪贴板历史记录

轻量级 Windows 剪贴板历史记录小应用。**Qt 6 + C++17** 实现，UI 参考 iOS / Windows 11 现代设计，支持暗色/浅色双主题。

<p align="center">
  <img src="https://img.shields.io/badge/platform-Windows%2010%2B-blue" alt="Platform">
  <img src="https://img.shields.io/badge/language-C%2B%2B17-%23f34b7d" alt="Language">
  <img src="https://img.shields.io/badge/framework-Qt%206-%2341CD52" alt="Qt">
</p>

---

## ✨ 功能

| 功能 | 说明 |
|------|------|
| 📝 文本记录 | 自动记录复制到剪贴板的文本（纯文本优先，不保留 HTML 标签） |
| 🖼 图片记录 | 支持截图、复制图片的记录和缩略图预览 |
| 🔍 实时搜索 | 面板顶部搜索框，输入即时过滤 |
| 🎨 双主题 | 暗色主题 + Win11 浅色 Mica 透明主题，一键切换 |
| 📌 系统托盘 | 常驻后台，左键弹出面板，右键菜单操作 |
| ⚡ 一键复制 | 点击历史条目即可重新复制到剪贴板 |
| 🗑 安全删除 | 右键删除需确认，防止误操作 |
| 🧠 仅内存 | 不写磁盘，关闭即清空，隐私安全 |
| 🎬 平滑动画 | 面板淡入淡出，OutCubic / InCubic 缓动 |

---

## 🚀 安装使用

### 方式一：下载 Release（推荐）

从 [Releases](https://github.com/chiaozij/clipboard-history/releases) 下载最新 `ClipboardHistory-v*.zip`，解压后双击 `ClipboardHistory.exe` 即可运行。

> 无需安装任何运行时，Windows 10/11 64 位解压即用。

### 方式二：从源码编译

#### 1. 安装编译环境

在 **D 盘** 安装 MSYS2（不占用系统盘）：

从 [msys2.org](https://www.msys2.org/) 下载安装器，安装到 `D:\MSYS2`。

打开 `D:\MSYS2\ucrt64.exe`，执行：

```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-qt6-base \
          mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-cmake \
          mingw-w64-ucrt-x86_64-ninja
```

#### 2. 编译

```bash
cd /d/Vibe_code
mkdir build && cd build
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

编译产物：`build/ClipboardHistory.exe`

#### 3. 部署（打包依赖）

```bash
windeployqt build/ClipboardHistory.exe
cp /d/MSYS2/ucrt64/bin/libgcc_s_seh-1.dll build/
cp /d/MSYS2/ucrt64/bin/libstdc++-6.dll build/
cp /d/MSYS2/ucrt64/bin/libwinpthread-1.dll build/
```

---

## 🎮 使用说明

### 基本操作

| 操作 | 方式 |
|------|------|
| 查看历史 | 左键点击托盘图标 📋 |
| 复制条目 | 点击面板中的记录 |
| 删除条目 | 右键条目 → 🗑 删除 → 确认 |
| 搜索过滤 | 面板顶部搜索框输入关键字 |
| 关闭面板 | 点击面板外部 / 按 ESC / 再次点托盘图标 |
| 切换主题 | 右键托盘 → 🎨 切换主题 |
| 清空记录 | 右键托盘 → 🗑 清空所有记录 |
| 退出应用 | 右键托盘 → ✕ 退出 |

### 主题

- **暗色主题**（默认）：深灰半透明背景，亮色文字，适合暗光环境
- **浅色主题**（Win11 风格）：浅灰 Mica 透明背景，深色文字，类似 Win11 设置面板

切换主题后面板、菜单、对话框全部跟随变色。

---

## 🏗 项目结构

```
src/
├── main.cpp                # 应用入口
├── ThemeManager.h/cpp      # 主题管理器（暗色/浅色双主题）
├── ClipboardItem.h/cpp     # 单条记录数据结构
├── ClipboardManager.h/cpp  # 剪贴板监控 + 历史管理
├── HistoryItemWidget.h/cpp # 单条记录卡片 UI
├── HistoryPanel.h/cpp      # 浮动面板（动画 + 毛玻璃）
├── TrayManager.h/cpp       # 系统托盘管理
└── resources/
    ├── app.qrc             # Qt 资源定义
    └── icons/clip.png      # 托盘图标
CMakeLists.txt              # CMake 构建配置
```

---

## 🛠 技术栈

| 技术 | 说明 |
|------|------|
| 语言 | C++17 |
| UI 框架 | Qt 6 (Widgets) |
| 构建 | CMake + Ninja |
| 编译器 | GCC (MinGW-w64 UCRT) |
| 动画 | QPropertyAnimation |
| 毛玻璃 | Windows DWM API (Mica/Acrylic) |

---

## 🔒 隐私

- 所有历史记录**仅在内存中**
- **不写入**磁盘文件
- **不联网**、不上传数据
- 关闭应用后全部清除

---

## 📄 License

MIT — 自由使用、修改、分发。
