# Mode 2: Sugar Oil Battle CMakeLists.txt

# 设置模式2的源文件
set(MODE2_SOURCES
    sugar_oil_game_window.cpp
    sugar_oil_game_scene.cpp
    usagi_player.cpp
    sugar_oil_enemy.cpp
    game_item.cpp
    creature.cpp
)

# 设置模式2的头文件
set(MODE2_HEADERS
    sugar_oil_config.h
    sugar_oil_game_window.h
    sugar_oil_game_scene.h
    usagi_player.h
    sugar_oil_enemy.h
    game_item.h
    creature.h
)

# 将模式2的文件添加到父项目
set(MODE2_ALL_FILES ${MODE2_SOURCES} ${MODE2_HEADERS})

# 创建模式2的源文件组（用于IDE显示）
source_group("Mode2_SugarOilBattle\\Headers" FILES ${MODE2_HEADERS})
source_group("Mode2_SugarOilBattle\\Sources" FILES ${MODE2_SOURCES})

# 将模式2文件添加到主项目的源文件列表中
set(MODE2_FILES ${MODE2_ALL_FILES} PARENT_SCOPE)