cmake_minimum_required(VERSION 3.16)

project(SugarOilBattle VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia)

# 启用Qt的MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置源文件
set(SOURCES
    main.cpp
    game_object_base.cpp
    sugar_oil_player.cpp
    bullet_base.cpp
    enemy_base.cpp
    sugar_oil_game_scene_new.cpp
    sugar_oil_main_window.cpp
)

# 设置头文件
set(HEADERS
    game_object_base.h
    sugar_oil_player.h
    bullet_base.h
    enemy_base.h
    sugar_oil_game_scene_new.h
    sugar_oil_main_window.h
)

# 创建可执行文件
add_executable(SugarOilBattle ${SOURCES} ${HEADERS})

# 链接Qt库
target_link_libraries(SugarOilBattle
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
)

# 设置编译器特定选项
if(MSVC)
    target_compile_options(SugarOilBattle PRIVATE /W4)
else()
    target_compile_options(SugarOilBattle PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 设置输出目录
set_target_properties(SugarOilBattle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 复制资源文件（如果有的话）
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# 安装规则
install(TARGETS SugarOilBattle
    RUNTIME DESTINATION bin
)