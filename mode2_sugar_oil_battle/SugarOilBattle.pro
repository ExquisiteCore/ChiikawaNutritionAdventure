QT += core widgets multimedia

CONFIG += c++17

TARGET = SugarOilBattle
TEMPLATE = app

# 源文件
SOURCES += \
    main.cpp \
    game_object_base.cpp \
    sugar_oil_player.cpp \
    bullet_base.cpp \
    enemy_base.cpp \
    sugar_oil_game_scene_new.cpp \
    sugar_oil_main_window.cpp

# 头文件
HEADERS += \
    game_object_base.h \
    sugar_oil_player.h \
    bullet_base.h \
    enemy_base.h \
    sugar_oil_game_scene_new.h \
    sugar_oil_main_window.h

# 编译器标志
CXXFLAGS += -Wall -Wextra

# 输出目录
DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = moc
UI_DIR = ui
RCC_DIR = rcc

# 资源文件（如果有的话）
# RESOURCES += resources.qrc

# 图标（可选）
# RC_ICONS = icon.ico
# ICON = icon.icns

# 版本信息
VERSION = 1.0.0

# 应用程序信息
QMAKE_TARGET_COMPANY = "游戏开发团队"
QMAKE_TARGET_PRODUCT = "吃糖吃油大冒险 - 模式2"
QMAKE_TARGET_DESCRIPTION = "一个基于Qt的射击生存游戏"
QMAKE_TARGET_COPYRIGHT = "Copyright 2024"