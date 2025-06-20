# ちいかわ营养大冒险 🎮

一个基于Qt的营养教育游戏，帮助玩家学习健康饮食知识。

## 项目简介

这是一个寓教于乐的营养健康游戏，玩家扮演乌萨奇，通过游戏学习营养知识，培养健康的饮食习惯。游戏包含两个主要模式：

- **模式一（碳水化合物之战）**：使用"膳食纤维剑"技能击败"伪蔬菜"BOSS
- **模式二（糖油混合物歼灭战）**：存活300秒，击败高糖油敌人

## 功能特性

### 已完成功能
- ✅ 用户登录注册系统（MySQL数据库）
- ✅ 游戏主界面（四个主要按钮）
- ✅ 游戏简介展示
- ✅ 基础UI设计和样式
- ✅ 数据库结构设计

### 计划功能
- 🔄 关卡选择和游戏模式
- 🔄 游戏设置（鼠标灵敏度、声音、音效）
- 🔄 营养知识宝典
- 🔄 题库问答系统
- 🔄 游戏记录和积分系统
- 🔄 角色和背景切换奖励

## 技术栈

- **开发框架**: Qt 6.x (C++)
- **数据库**: MySQL 8.0+
- **构建工具**: qmake
- **UI设计**: Qt Widgets + CSS样式

## 安装和运行

### 环境要求

1. **Qt开发环境**
   - Qt 6.0 或更高版本
   - Qt Creator IDE（推荐）
   - 支持C++17的编译器

2. **数据库环境**
   - MySQL 8.0 或更高版本
   - MySQL服务正在运行

### 安装步骤

1. **克隆项目**
   ```bash
   git clone <repository-url>
   cd ChiikawaNutritionAdventure
   ```

2. **设置数据库**
   ```bash
   # 登录MySQL
   mysql -u root -p
   
   # 执行数据库初始化脚本
   source database_setup.sql
   ```

3. **配置数据库连接**
   
   编辑 `loginwindow.cpp` 文件中的数据库连接参数：
   ```cpp
   database.setHostName("localhost");     // 数据库主机
   database.setDatabaseName("chiikawa_game"); // 数据库名
   database.setUserName("root");          // 用户名
   database.setPassword("your_password");  // 密码
   database.setPort(3306);                // 端口
   ```

4. **编译和运行**
   
   **方法一：使用Qt Creator**
   - 打开Qt Creator
   - 打开项目文件 `ChiikawaNutritionAdventure.pro`
   - 点击运行按钮
   
   **方法二：命令行编译**
   ```bash
   qmake ChiikawaNutritionAdventure.pro
   make
   ./ChiikawaNutritionAdventure
   ```

## 使用说明

### 登录系统

1. **首次使用**：点击"注册"按钮创建新账户
   - 用户名：至少3个字符
   - 密码：至少6个字符

2. **登录游戏**：输入用户名和密码，点击"登录"

3. **测试账户**（已预置）：
   - 管理员：用户名 `admin`，密码 `admin123`
   - 测试用户：用户名 `test_user`，密码 `test123`

### 游戏界面

登录成功后，主界面包含四个按钮：

- **🎯 游戏简介**：查看游戏背景和玩法说明
- **🎮 关卡选择**：选择游戏模式（开发中）
- **⚙️ 游戏设置**：调整游戏参数（开发中）
- **🚪 退出登录**：返回登录界面

## 项目结构

```
ChiikawaNutritionAdventure/
├── main.cpp                 # 程序入口
├── mainwindow.h/.cpp        # 主窗口类
├── loginwindow.h/.cpp       # 登录窗口类
├── mainwindow.ui            # 主窗口UI文件
├── ChiikawaNutritionAdventure.pro  # 项目配置文件
├── database_setup.sql       # 数据库初始化脚本
├── README.md               # 项目说明文档
├── 设计需求.txt             # 游戏设计需求
└── 素材/                   # 游戏素材目录
    ├── items/              # 道具图片
    ├── roles/              # 角色图片
    ├── ui/                 # UI素材
    ├── bullet/             # 子弹素材
    ├── enemybase/          # 基础敌人
    └── enemybig/           # BOSS敌人
```

## 数据库结构

### 主要数据表

1. **users** - 用户信息表
   - 用户名、密码、邮箱
   - 游戏等级、总分数
   - 创建时间、最后登录时间

2. **nutrition_knowledge** - 营养知识表
   - 知识标题、内容、分类
   - 难度等级

3. **question_bank** - 题库表
   - 题目、选项、正确答案
   - 答案解释、分类、难度

4. **game_records** - 游戏记录表
   - 用户ID、游戏模式、得分
   - 游戏时长、胜负结果

## 开发计划

### 下一步开发重点

1. **关卡系统**
   - 实现两个游戏模式的具体玩法
   - 添加游戏物理引擎和碰撞检测
   - 实现角色移动和攻击系统

2. **营养教育系统**
   - 完善营养知识宝典界面
   - 实现题库问答功能
   - 添加学习进度跟踪

3. **游戏设置**
   - 实现鼠标灵敏度调节
   - 添加音效和背景音乐
   - 实现音量控制

4. **奖励系统**
   - 实现角色切换功能
   - 添加背景图片切换
   - 设计成就系统

## 贡献指南

欢迎提交Issue和Pull Request来改进这个项目！

## 许可证

本项目仅用于学习和教育目的。

## 联系方式

如有问题或建议，请通过以下方式联系：
- 项目Issues页面
- 邮箱：[your-email@example.com]

---

**让我们一起帮助乌萨奇养成健康的饮食习惯吧！** 🥗✨