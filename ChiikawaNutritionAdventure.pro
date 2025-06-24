QT       += core gui sql multimedia

QT += core widgets multimedia

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    loginwindow.cpp \
    audio_manager.cpp \
    mode1_carbohydrate_battle/carbohydrate_game_window.cpp \
    mode1_carbohydrate_battle/carbohydrate_game_scene.cpp \
    mode1_carbohydrate_battle/game_map.cpp \
    mode1_carbohydrate_battle/player.cpp \
    mode1_carbohydrate_battle/fake_vegetable_boss.cpp \
    mode1_carbohydrate_battle/fiber_sword.cpp \
    mode2_sugar_oil_battle/sugar_oil_game_window.cpp \
    mode2_sugar_oil_battle/sugar_oil_game_scene_new.cpp \
    mode2_sugar_oil_battle/usagi_player.cpp \
    mode2_sugar_oil_battle/sugar_oil_enemy.cpp \
    mode2_sugar_oil_battle/creature.cpp \
    mode2_sugar_oil_battle/game_object_base.cpp \
    mode2_sugar_oil_battle/sugar_oil_player.cpp \
    mode2_sugar_oil_battle/enemy_base.cpp \
    mode2_sugar_oil_battle/bullet_base.cpp \
    mode2_sugar_oil_battle/creature_system.cpp \
    mode2_sugar_oil_battle/item_system.cpp

HEADERS += \
    mainwindow.h \
    loginwindow.h \
    audio_manager.h \
    mode1_carbohydrate_battle/carbohydrate_config.h \
    mode1_carbohydrate_battle/carbohydrate_game_window.h \
    mode1_carbohydrate_battle/carbohydrate_game_scene.h \
    mode1_carbohydrate_battle/game_map.h \
    mode1_carbohydrate_battle/player.h \
    mode1_carbohydrate_battle/fake_vegetable_boss.h \
    mode1_carbohydrate_battle/fiber_sword.h \
    mode2_sugar_oil_battle/sugar_oil_config.h \
    mode2_sugar_oil_battle/sugar_oil_game_window.h \
    mode2_sugar_oil_battle/sugar_oil_game_scene_new.h \
    mode2_sugar_oil_battle/usagi_player.h \
    mode2_sugar_oil_battle/sugar_oil_enemy.h \
    mode2_sugar_oil_battle/creature.h \
    mode2_sugar_oil_battle/game_object_base.h \
    mode2_sugar_oil_battle/sugar_oil_player.h \
    mode2_sugar_oil_battle/creature_system.h \
    mode2_sugar_oil_battle/item_system.h \
    mode2_sugar_oil_battle/enemy_base.h \
    mode2_sugar_oil_battle/bullet_base.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
