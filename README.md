# 📋 剪贴板历史记录 (Clipboard History)

一个轻量级 Windows 剪贴板历史记录小应用。采用 **Qt 6 + C++17** 实现，UI 风格参考 iOS / Windows 11 现代设计语言。

---

## ✨ 功能特性

| 功能 | 说明 |
|------|------|
| 📝 **文本记录** | 自动记录复制到剪贴板的文本内容 |
| 🖼 **图片记录** | 支持截图、复制图片的记录和预览 |
| 🔍 **实时搜索** | 在历史面板中即时过滤查找条目 |
| 🎨 **现代 UI** | 毛玻璃/Acrylic 背景、圆角卡片、平滑动画 |
| 📌 **系统托盘** | 常驻后台，左键弹出面板，右键菜单操作 |
| ⚡ **一键复制** | 点击历史条目即可重新复制到剪贴板 |
| 🗑 **安全删除** | 右键删除，带确认对话框防止误操作 |
| 🧠 **仅内存存储** | 不写磁盘，关闭即清空，隐私安全 |

---

## 🖥 系统要求

- Windows 10 1809+ 或 Windows 11（64 位）
- 不需要额外安装 Qt 运行库（如静态编译）

---

## 🚀 快速开始

### 方式一：直接运行（推荐）

下载编译好的 `ClipboardHistory.exe`，双击运行即可。

托盘会出现 📋 图标，开始自动监控剪贴板。

### 方式二：从源码构建

#### 1. 安装编译环境

在 **D 盘** 安装 MSYS2（避免占用系统盘）：

```bash
# 下载 MSYS2 安装器
# https://github.com/msys2/msys2-installer/releases
# 安装路径: D:\msys64
```

打开 `D:\msys64\ucrt64.exe`，安装 Qt6 和编译器：

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

#### 3. 静态编译（可选，单文件 exe 无依赖）

```bash
# 先安装 Qt 静态库
pacman -S mingw-w64-ucrt-x86_64-qt6-static

# 静态编译
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF ..
cmake --build .
```

---

## 📖 使用说明

### 基本操作

| 操作 | 方式 |
|------|------|
| 查看历史 | 左键点击托盘图标 |
| 复制条目 | 点击面板中的历史记录 |
| 删除条目 | 右键条目 → 🗑 删除 → 确认 |
| 搜索过滤 | 在面板顶部搜索框输入关键字 |
| 关闭面板 | 点击面板外部 / 按 ESC / 再次点击托盘图标 |
| 清空记录 | 右键托盘 → 🗑 清空所有记录 |
| 退出应用 | 右键托盘 → ✕ 退出 |

### 托盘图标说明

- **左键单击** — 显示/隐藏历史面板
- **右键单击** — 弹出菜单

---

## 🏗 项目结构

```
d:\Vibe_code\
├── CMakeLists.txt              # CMake 构建配置
├── README.md                   # 本文件
├── plan.md                     # 实现计划
├── src/
│   ├── main.cpp                # 应用程序入口
│   ├── ClipboardItem.h         # 单条历史记录数据结构
│   ├── ClipboardManager.h      # 剪贴板监控与历史管理（头文件）
│   ├── ClipboardManager.cpp    # 剪贴板监控与历史管理（实现）
│   ├── HistoryItemWidget.h     # 单条记录 UI 组件（头文件）
│   ├── HistoryItemWidget.cpp   # 单条记录 UI 组件（实现）
│   ├── HistoryPanel.h          # 浮动历史面板（头文件）
│   ├── HistoryPanel.cpp        # 浮动历史面板（实现，含动画）
│   ├── TrayManager.h           # 系统托盘管理（头文件）
│   ├── TrayManager.cpp         # 系统托盘管理（实现）
│   └── resources/
│       ├── app.qrc             # Qt 资源定义
│       └── icons/
│           └── clip.png        # 托盘图标
```

---

## 🛠 技术栈

| 技术 | 说明 |
|------|------|
| **语言** | C++17 |
| **UI 框架** | Qt 6 (Widgets) |
| **构建系统** | CMake + Ninja |
| **编译器** | GCC 13 (MinGW-w64 UCRT) |
| **动画** | QPropertyAnimation + EasingCurve |
| **毛玻璃** | Windows DWM API (Acrylic/Mica) |
| **剪贴板** | QClipboard (系统原生 API) |

---

## 🔒 隐私说明

- 所有历史记录**仅保存在内存中**
- **不会**写入磁盘文件
- **不会**上传到任何网络
- **不会**收集任何用户数据
- 关闭应用后所有记录自动清除

---

## 📄 许可证

MIT License — 可自由使用、修改、分发。

---

## 🎯 设计风格

- **暗色主题** — 深色背景 + 半透明卡片
- **圆角设计** — 12px 窗口圆角 + 8px 卡片圆角
- **毛玻璃背景** — Windows Acrylic 材质
- **平滑动画** — 200ms 淡入 / 150ms 淡出（OutCubic / InCubic 缓动）
- **悬停反馈** — 卡片 hover 高亮
- **微软雅黑** — 中文字体优先
