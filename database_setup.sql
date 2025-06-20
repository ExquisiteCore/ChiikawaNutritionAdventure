-- ちいかわ营养大冒险游戏数据库初始化脚本
-- 请在MySQL中执行此脚本来创建游戏所需的数据库和表

-- 创建数据库
CREATE DATABASE IF NOT EXISTS chiikawa_game 
CHARACTER SET utf8mb4 
COLLATE utf8mb4_unicode_ci;

-- 使用数据库
USE chiikawa_game;

-- 创建用户表
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL COMMENT '用户名',
    password VARCHAR(255) NOT NULL COMMENT '密码（实际项目中应使用哈希）',
    email VARCHAR(100) COMMENT '邮箱',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    last_login TIMESTAMP NULL COMMENT '最后登录时间',
    game_level INT DEFAULT 1 COMMENT '游戏等级',
    total_score INT DEFAULT 0 COMMENT '总分数'
) COMMENT='用户信息表';

-- 创建营养知识表（用于宝典功能）
CREATE TABLE IF NOT EXISTS nutrition_knowledge (
    id INT AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(200) NOT NULL COMMENT '知识标题',
    content TEXT NOT NULL COMMENT '知识内容',
    category VARCHAR(50) NOT NULL COMMENT '分类（如：碳水化合物、蛋白质等）',
    difficulty_level INT DEFAULT 1 COMMENT '难度等级 1-5',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) COMMENT='营养知识表';

-- 创建题库表
CREATE TABLE IF NOT EXISTS question_bank (
    id INT AUTO_INCREMENT PRIMARY KEY,
    question TEXT NOT NULL COMMENT '题目',
    option_a VARCHAR(200) NOT NULL COMMENT '选项A',
    option_b VARCHAR(200) NOT NULL COMMENT '选项B', 
    option_c VARCHAR(200) NOT NULL COMMENT '选项C',
    option_d VARCHAR(200) NOT NULL COMMENT '选项D',
    correct_answer CHAR(1) NOT NULL COMMENT '正确答案 A/B/C/D',
    explanation TEXT COMMENT '答案解释',
    category VARCHAR(50) NOT NULL COMMENT '题目分类',
    difficulty_level INT DEFAULT 1 COMMENT '难度等级 1-5',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) COMMENT='题库表';

-- 创建游戏记录表
CREATE TABLE IF NOT EXISTS game_records (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    game_mode VARCHAR(50) NOT NULL COMMENT '游戏模式（模式一/模式二）',
    score INT NOT NULL COMMENT '得分',
    play_time INT NOT NULL COMMENT '游戏时长（秒）',
    is_victory BOOLEAN NOT NULL COMMENT '是否胜利',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
) COMMENT='游戏记录表';

-- 插入一些示例营养知识
INSERT INTO nutrition_knowledge (title, content, category, difficulty_level) VALUES
('什么是膳食纤维？', 
 '膳食纤维是一种不能被人体消化吸收的碳水化合物，主要存在于植物性食物中。它有助于促进肠道蠕动，预防便秘，还能帮助控制血糖和胆固醇水平。富含膳食纤维的食物包括：全谷物、蔬菜、水果、豆类等。', 
 '膳食纤维', 1),

('糖油混合物的危害', 
 '糖油混合物是指同时含有大量糖分和油脂的食物，如奶茶、炸鸡、蛋糕等。这类食物热量极高，容易导致肥胖、血糖升高、血脂异常等健康问题。长期摄入过多糖油混合物还可能增加糖尿病、心血管疾病的风险。', 
 '营养健康', 2),

('碳水化合物的重要性', 
 '碳水化合物是人体主要的能量来源，分为简单碳水化合物（如糖类）和复杂碳水化合物（如淀粉、纤维）。选择优质的碳水化合物很重要，应多选择全谷物、蔬菜等富含纤维的复杂碳水化合物，少吃精制糖和加工食品。', 
 '碳水化合物', 1);

-- 插入一些示例题目
INSERT INTO question_bank (question, option_a, option_b, option_c, option_d, correct_answer, explanation, category, difficulty_level) VALUES
('以下哪种食物富含膳食纤维？', '白米饭', '苹果', '白面包', '糖果', 'B', '苹果含有丰富的膳食纤维，特别是果皮部分。膳食纤维有助于消化和预防便秘。', '膳食纤维', 1),

('糖油混合物的典型代表是？', '蒸蛋', '水煮蔬菜', '奶茶', '白开水', 'C', '奶茶含有大量糖分和脂肪，是典型的糖油混合物，应该适量饮用。', '营养健康', 1),

('每天推荐的膳食纤维摄入量是？', '10-15克', '25-30克', '50-60克', '100克以上', 'B', '成年人每天推荐摄入25-30克膳食纤维，有助于维持肠道健康。', '膳食纤维', 2),

('以下哪种做法有助于减少糖油混合物的摄入？', '多喝奶茶', '选择烘焙食品', '多吃新鲜水果', '增加油炸食品', 'C', '新鲜水果含有天然糖分和丰富营养，是替代高糖高油食品的健康选择。', '营养健康', 1),

('全谷物相比精制谷物的优势是？', '口感更好', '保留更多营养和纤维', '热量更低', '价格更便宜', 'B', '全谷物保留了谷物的胚芽和麸皮，含有更多的维生素、矿物质和膳食纤维。', '碳水化合物', 2);

-- 创建默认管理员用户（用于测试）
INSERT INTO users (username, password, email, game_level, total_score) VALUES
('admin', 'admin123', 'admin@chiikawa.com', 10, 10000),
('test_user', 'test123', 'test@chiikawa.com', 1, 0);

SELECT '数据库初始化完成！' as message;