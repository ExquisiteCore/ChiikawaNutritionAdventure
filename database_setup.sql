-- ちいかわ营养大冒险 - SQLite数据库初始化脚本
-- 基于原始SQL Server设计，转换为SQLite兼容语法

-- 创建食物类别表
CREATE TABLE IF NOT EXISTS Categories (
    CategoryID INTEGER PRIMARY KEY AUTOINCREMENT,
    CategoryName TEXT NOT NULL,
    Description TEXT,
    ParentCategoryID INTEGER,
    CreatedDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    ModifiedDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (ParentCategoryID) REFERENCES Categories(CategoryID)
);

-- 创建食物表
CREATE TABLE IF NOT EXISTS Foods (
    FoodID INTEGER PRIMARY KEY AUTOINCREMENT,
    FoodName TEXT NOT NULL,
    CategoryID INTEGER NOT NULL,
    Description TEXT,
    Calories REAL,
    Protein REAL,
    Fat REAL,
    Carbohydrates REAL,
    Fiber REAL,
    Sugar REAL,
    IsVegetarian INTEGER DEFAULT 0,
    IsVegan INTEGER DEFAULT 0,
    IsGlutenFree INTEGER DEFAULT 0,
    CreatedDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    ModifiedDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (CategoryID) REFERENCES Categories(CategoryID)
);

-- 创建食物来源表
CREATE TABLE IF NOT EXISTS FoodSources (
    SourceID INTEGER PRIMARY KEY AUTOINCREMENT,
    FoodID INTEGER NOT NULL,
    SourceType TEXT CHECK (SourceType IN ('Animal', 'Plant', 'Fungus', 'Synthetic')),
    SourceDescription TEXT,
    FOREIGN KEY (FoodID) REFERENCES Foods(FoodID)
);

-- 创建食物产地表
CREATE TABLE IF NOT EXISTS FoodOrigins (
    OriginID INTEGER PRIMARY KEY AUTOINCREMENT,
    FoodID INTEGER NOT NULL,
    Country TEXT,
    Region TEXT,
    City TEXT,
    FOREIGN KEY (FoodID) REFERENCES Foods(FoodID)
);

-- 创建计量单位表
CREATE TABLE IF NOT EXISTS Units (
    UnitID INTEGER PRIMARY KEY AUTOINCREMENT,
    UnitName TEXT NOT NULL,
    UnitSymbol TEXT,
    UnitType TEXT CHECK (UnitType IN ('Mass', 'Volume', 'Count', 'Length'))
);

-- 创建食物营养数据表
CREATE TABLE IF NOT EXISTS FoodNutrition (
    NutritionID INTEGER PRIMARY KEY AUTOINCREMENT,
    FoodID INTEGER NOT NULL,
    UnitID INTEGER NOT NULL,
    Amount REAL NOT NULL,
    Calories REAL,
    Protein REAL,
    Fat REAL,
    Carbohydrates REAL,
    Fiber REAL,
    Sugar REAL,
    Calcium REAL,
    Iron REAL,
    Sodium REAL,
    Potassium REAL,
    VitaminA REAL,
    VitaminC REAL,
    CreatedDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    ModifiedDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (FoodID) REFERENCES Foods(FoodID),
    FOREIGN KEY (UnitID) REFERENCES Units(UnitID)
);

-- 创建营养知识题库表
CREATE TABLE IF NOT EXISTS NutritionQuestions (
    QuestionID INTEGER PRIMARY KEY AUTOINCREMENT,
    QuestionText TEXT NOT NULL,
    OptionA TEXT NOT NULL,
    OptionB TEXT NOT NULL,
    OptionC TEXT NOT NULL,
    OptionD TEXT NOT NULL,
    CorrectAnswer TEXT NOT NULL CHECK (CorrectAnswer IN ('A', 'B', 'C', 'D')),
    Explanation TEXT,
    DifficultyLevel INTEGER CHECK (DifficultyLevel BETWEEN 1 AND 3),
    Category TEXT,
    CreatedDate DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 创建营养知识内容表（用于宝典功能）
CREATE TABLE IF NOT EXISTS NutritionKnowledge (
    KnowledgeID INTEGER PRIMARY KEY AUTOINCREMENT,
    Title TEXT NOT NULL,
    Content TEXT NOT NULL,
    Category TEXT,
    ImagePath TEXT,
    CreatedDate DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 创建用户答题记录表
CREATE TABLE IF NOT EXISTS UserAnswerRecords (
    RecordID INTEGER PRIMARY KEY AUTOINCREMENT,
    Username TEXT NOT NULL,
    QuestionID INTEGER NOT NULL,
    UserAnswer TEXT NOT NULL,
    IsCorrect INTEGER NOT NULL,
    AnswerTime DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (QuestionID) REFERENCES NutritionQuestions(QuestionID)
);

-- 创建视图：获取食物及其营养信息
CREATE VIEW IF NOT EXISTS vw_FoodNutrition AS
SELECT 
    f.FoodID,
    f.FoodName,
    c.CategoryName,
    fn.Amount,
    u.UnitName,
    fn.Calories,
    fn.Protein,
    fn.Fat,
    fn.Carbohydrates,
    fn.Fiber,
    fn.Sugar,
    fn.Calcium,
    fn.Iron,
    fn.Sodium,
    fn.Potassium,
    fn.VitaminA,
    fn.VitaminC
FROM 
    Foods f
JOIN 
    Categories c ON f.CategoryID = c.CategoryID
JOIN 
    FoodNutrition fn ON f.FoodID = fn.FoodID
JOIN 
    Units u ON fn.UnitID = u.UnitID;

-- 插入示例数据：食物类别
INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES
('水果', '多汁且主要甜味的植物果实', NULL),
('蔬菜', '可作为食物的草本植物部分', NULL),
('谷物', '禾本科植物的种子', NULL),
('蛋白质', '富含蛋白质的食物', NULL),
('乳制品', '以奶为基础的食品', 4),
('肉类', '动物的可食用组织', 4),
('豆类', '豆科植物的种子', 4),
('坚果和种子', '可食用的坚果和种子', 4),
('脂肪和油', '高能量密度的食物', NULL),
('糖类', '甜味的碳水化合物', NULL);

-- 插入示例数据：计量单位
INSERT OR IGNORE INTO Units (UnitName, UnitSymbol, UnitType) VALUES
('克', 'g', 'Mass'),
('千克', 'kg', 'Mass'),
('毫升', 'ml', 'Volume'),
('升', 'L', 'Volume'),
('个', '', 'Count'),
('杯', 'c', 'Volume'),
('汤匙', 'tbsp', 'Volume'),
('茶匙', 'tsp', 'Volume');

-- 插入营养知识内容（宝典内容）
INSERT OR IGNORE INTO NutritionKnowledge (Title, Content, Category) VALUES
('什么是均衡饮食？', '均衡饮食是指摄入适量的各种营养素，包括碳水化合物、蛋白质、脂肪、维生素和矿物质。每天应该摄入多种不同颜色的蔬菜和水果，选择全谷物食品，适量摄入优质蛋白质，限制加工食品和高糖食品的摄入。', '基础营养'),
('碳水化合物的重要性', '碳水化合物是人体的主要能量来源，特别是大脑的唯一能量来源。应该选择复合碳水化合物，如全麦面包、糙米、燕麦等，而不是简单糖类。复合碳水化合物能提供持续的能量，并含有丰富的膳食纤维。', '碳水化合物'),
('蛋白质的作用', '蛋白质是构成人体组织的重要成分，参与酶和激素的合成，维持免疫功能。优质蛋白质来源包括瘦肉、鱼类、蛋类、豆类和坚果。成人每天应摄入体重（公斤）×0.8-1.2克的蛋白质。', '蛋白质'),
('健康脂肪的选择', '脂肪是必需营养素，但要选择健康的脂肪。不饱和脂肪酸（如橄榄油、鱼油、坚果中的脂肪）对心脏健康有益，而应该限制饱和脂肪和反式脂肪的摄入。', '脂肪'),
('维生素和矿物质', '维生素和矿物质虽然需要量不大，但对维持正常生理功能至关重要。通过多样化的饮食，特别是多吃蔬菜水果，通常能满足大部分维生素和矿物质的需求。', '维生素矿物质'),
('膳食纤维的益处', '膳食纤维有助于维持肠道健康，预防便秘，控制血糖和胆固醇水平。富含膳食纤维的食物包括全谷物、蔬菜、水果和豆类。成人每天应摄入25-35克膳食纤维。', '膳食纤维'),
('水的重要性', '水是人体最重要的营养素，参与所有生理过程。成人每天应饮用1500-2000毫升的水，运动时需要增加水分摄入。除了白开水，也可以通过汤类、水果等获得水分。', '水分'),
('如何阅读营养标签', '营养标签提供了食品的营养信息。重点关注热量、蛋白质、脂肪、碳水化合物、钠和糖的含量。选择低钠、低糖、高纤维的食品。注意份量大小，避免过量摄入。', '营养标签'),
('健康烹饪方法', '选择健康的烹饪方法可以保留食物的营养价值。推荐蒸、煮、烤、炖等方法，减少油炸和高温烹饪。使用香料和香草代替过多的盐和糖来调味。', '烹饪方法'),
('运动与营养', '运动前后的营养补充很重要。运动前1-2小时可以吃一些容易消化的碳水化合物，运动后30分钟内补充蛋白质和碳水化合物有助于肌肉恢复和糖原补充。', '运动营养');

-- 插入营养知识选择题
INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES
('以下哪种食物富含优质蛋白质？', '苹果', '鸡胸肉', '白米饭', '橄榄油', 'B', '鸡胸肉是优质蛋白质的良好来源，每100克含有约31克蛋白质。', 1, '蛋白质'),
('维生素C主要存在于以下哪种食物中？', '牛奶', '柑橘类水果', '鸡蛋', '牛肉', 'B', '柑橘类水果如橙子、柠檬等富含维生素C。', 1, '维生素'),
('膳食纤维对人体的主要作用是？', '提供能量', '促进肠道蠕动', '增强免疫力', '帮助钙吸收', 'B', '膳食纤维可以促进肠道蠕动，预防便秘等问题。', 1, '膳食纤维'),
('以下哪种食物属于全谷物？', '白面包', '燕麦片', '蛋糕', '饼干', 'B', '燕麦片是全谷物食品，保留了谷物的麸皮、胚芽和胚乳。', 1, '全谷物'),
('铁元素的主要功能是什么？', '维持骨骼健康', '帮助氧气运输', '调节血压', '促进伤口愈合', 'B', '铁是血红蛋白的组成成分，帮助氧气在体内运输。', 1, '矿物质'),
('以下哪种脂肪对心脏健康有益？', '饱和脂肪', '反式脂肪', '不饱和脂肪', '胆固醇', 'C', '不饱和脂肪如橄榄油、鱼油等对心脏健康有益。', 1, '脂肪'),
('成人每天建议的饮水量是多少？', '500毫升', '1000毫升', '1500-2000毫升', '3000毫升以上', 'C', '成人每天建议饮水量为1500-2000毫升，具体因个体差异而异。', 1, '水'),
('以下哪种食物含有丰富的钙？', '菠菜', '牛奶', '西红柿', '米饭', 'B', '牛奶是钙的优质来源，每100毫升约含有120毫克钙。', 1, '矿物质'),
('以下哪种维生素属于脂溶性维生素？', '维生素C', '维生素B群', '维生素A', '维生素B12', 'C', '维生素A、D、E、K属于脂溶性维生素。', 1, '维生素'),
('膳食纤维主要存在于以下哪种食物中？', '肉类', '水果和蔬菜', '乳制品', '油脂', 'B', '水果和蔬菜是膳食纤维的主要来源。', 1, '膳食纤维'),
('以下哪种食物的升糖指数（GI）较高？', '全麦面包', '白米饭', '燕麦', '藜麦', 'B', '白米饭的升糖指数较高，食用后血糖上升较快。', 2, '碳水化合物'),
('Omega-3脂肪酸主要存在于以下哪种食物中？', '橄榄油', '鱼油', '椰子油', '花生油', 'B', '鱼油中富含Omega-3脂肪酸，如EPA和DHA。', 2, '脂肪'),
('以下哪种食物富含益生菌？', '酸奶', '橙汁', '巧克力', '薯片', 'A', '酸奶中含有益生菌，有助于维持肠道健康。', 2, '益生菌'),
('以下哪种矿物质对神经系统功能至关重要？', '钠', '钾', '镁', '以上都是', 'D', '钠、钾、镁等矿物质对神经系统功能都非常重要。', 2, '矿物质'),
('以下哪种食物含有天然的抗氧化剂？', '蓝莓', '黄油', '白面包', '香肠', 'A', '蓝莓中含有丰富的抗氧化剂，如花青素。', 2, '抗氧化剂'),
('以下哪种维生素有助于铁的吸收？', '维生素A', '维生素C', '维生素D', '维生素E', 'B', '维生素C可以促进非血红素铁的吸收。', 2, '维生素'),
('以下哪种食物属于低血糖指数（低GI）食物？', '白面包', '西瓜', '全麦饼干', '蜂蜜', 'C', '全麦饼干的血糖指数较低，适合控制血糖的人群。', 2, '碳水化合物'),
('以下哪种食物富含维生素B12？', '菠菜', '鸡蛋', '苹果', '杏仁', 'B', '维生素B12主要存在于动物性食物中，如鸡蛋、肉类、奶制品等。', 2, '维生素'),
('以下哪种食物的蛋白质含量最高？', '黑豆', '鸡胸肉', '豆腐', '燕麦', 'B', '鸡胸肉的蛋白质含量约为31%，高于黑豆、豆腐和燕麦。', 2, '蛋白质'),
('以下哪种营养素是人体最主要的能量来源？', '蛋白质', '碳水化合物', '脂肪', '维生素', 'B', '碳水化合物是人体最主要的能量来源，尤其是大脑的唯一能量来源。', 2, '碳水化合物');

-- 创建索引以提高查询性能
CREATE INDEX IF NOT EXISTS idx_Foods_CategoryID ON Foods(CategoryID);
CREATE INDEX IF NOT EXISTS idx_FoodNutrition_FoodID ON FoodNutrition(FoodID);
CREATE INDEX IF NOT EXISTS idx_Categories_ParentCategoryID ON Categories(ParentCategoryID);
CREATE INDEX IF NOT EXISTS idx_NutritionQuestions_Category ON NutritionQuestions(Category);
CREATE INDEX IF NOT EXISTS idx_NutritionQuestions_DifficultyLevel ON NutritionQuestions(DifficultyLevel);
CREATE INDEX IF NOT EXISTS idx_NutritionKnowledge_Category ON NutritionKnowledge(Category);
CREATE INDEX IF NOT EXISTS idx_UserAnswerRecords_Username ON UserAnswerRecords(Username);
CREATE INDEX IF NOT EXISTS idx_UserAnswerRecords_QuestionID ON UserAnswerRecords(QuestionID);