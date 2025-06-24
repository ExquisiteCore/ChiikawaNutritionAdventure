#include "loginwindow.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupDatabase();
    applyStyles();
    
    // 设置窗口属性
    setWindowTitle("ちいかわ营养大冒险 - 登录");
    setFixedSize(400, 500);
    
    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

LoginWindow::~LoginWindow()
{
    if (database.isOpen()) {
        database.close();
    }
}

void LoginWindow::setupUI()
{
    // 创建主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    
    // 游戏标题
    titleLabel = new QLabel("ちいかわ营养大冒险", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");
    
    // Logo占位符（可以后续添加游戏logo图片）
    logoLabel = new QLabel("🎮", this);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setObjectName("logoLabel");
    
    // 表单布局
    formLayout = new QVBoxLayout();
    formLayout->setSpacing(15);
    
    // 用户名输入
    usernameLabel = new QLabel("用户名:", this);
    usernameLabel->setObjectName("inputLabel");
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("请输入用户名");
    usernameEdit->setObjectName("inputEdit");
    
    // 密码输入
    passwordLabel = new QLabel("密码:", this);
    passwordLabel->setObjectName("inputLabel");
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setObjectName("inputEdit");
    
    // 按钮布局
    buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    
    loginButton = new QPushButton("登录", this);
    loginButton->setObjectName("primaryButton");
    registerButton = new QPushButton("注册", this);
    registerButton->setObjectName("secondaryButton");
    
    // 添加到表单布局
    formLayout->addWidget(usernameLabel);
    formLayout->addWidget(usernameEdit);
    formLayout->addWidget(passwordLabel);
    formLayout->addWidget(passwordEdit);
    
    // 添加到按钮布局
    buttonLayout->addWidget(registerButton);
    buttonLayout->addWidget(loginButton);
    
    // 添加到主布局
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(logoLabel);
    mainLayout->addStretch();
    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号槽
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    
    // 回车键登录
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::setupDatabase()
{
    // 使用SQLite数据库（无需额外配置）
    database = QSqlDatabase::addDatabase("QSQLITE");
    
    // 使用应用程序目录下的数据库文件
    QString dbPath = QApplication::applicationDirPath() + "/chiikawa_game.db";
    database.setDatabaseName(dbPath);
    
    qDebug() << "数据库文件路径:" << dbPath;
    
    if (!database.open()) {
        qDebug() << "SQLite数据库连接失败:" << database.lastError().text();
        QMessageBox::warning(this, "数据库错误", 
                           "无法创建本地数据库\n" + 
                           database.lastError().text());
    } else {
        qDebug() << "SQLite数据库连接成功";
        
        // 创建用户表（如果不存在）
        QSqlQuery query(database);
        QString createTableSQL = 
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT UNIQUE NOT NULL,"
            "password TEXT NOT NULL,"
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ")";
        
        if (!query.exec(createTableSQL)) {
            qDebug() << "创建用户表失败:" << query.lastError().text();
        } else {
            // 执行数据库初始化脚本（嵌入式SQL）
            QStringList sqlStatements = {
                // 创建食物类别表
                "CREATE TABLE IF NOT EXISTS Categories ("
                "    CategoryID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    CategoryName TEXT NOT NULL,"
                "    Description TEXT,"
                "    ParentCategoryID INTEGER,"
                "    CreatedDate DATETIME DEFAULT CURRENT_TIMESTAMP,"
                "    ModifiedDate DATETIME DEFAULT CURRENT_TIMESTAMP,"
                "    FOREIGN KEY (ParentCategoryID) REFERENCES Categories(CategoryID)"
                ")",
                
                // 创建食物表
                "CREATE TABLE IF NOT EXISTS Foods ("
                "    FoodID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    FoodName TEXT NOT NULL,"
                "    CategoryID INTEGER NOT NULL,"
                "    Description TEXT,"
                "    Calories REAL,"
                "    Protein REAL,"
                "    Fat REAL,"
                "    Carbohydrates REAL,"
                "    Fiber REAL,"
                "    Sugar REAL,"
                "    IsVegetarian INTEGER DEFAULT 0,"
                "    IsVegan INTEGER DEFAULT 0,"
                "    IsGlutenFree INTEGER DEFAULT 0,"
                "    CreatedDate DATETIME DEFAULT CURRENT_TIMESTAMP,"
                "    ModifiedDate DATETIME DEFAULT CURRENT_TIMESTAMP,"
                "    FOREIGN KEY (CategoryID) REFERENCES Categories(CategoryID)"
                ")",
                
                // 创建食物来源表
                "CREATE TABLE IF NOT EXISTS FoodSources ("
                "    SourceID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    FoodID INTEGER NOT NULL,"
                "    SourceType TEXT CHECK (SourceType IN ('Animal', 'Plant', 'Fungus', 'Synthetic')),"
                "    SourceDescription TEXT,"
                "    FOREIGN KEY (FoodID) REFERENCES Foods(FoodID)"
                ")",
                
                // 创建食物产地表
                "CREATE TABLE IF NOT EXISTS FoodOrigins ("
                "    OriginID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    FoodID INTEGER NOT NULL,"
                "    Country TEXT,"
                "    Region TEXT,"
                "    City TEXT,"
                "    FOREIGN KEY (FoodID) REFERENCES Foods(FoodID)"
                ")",
                
                // 创建计量单位表
                "CREATE TABLE IF NOT EXISTS Units ("
                "    UnitID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    UnitName TEXT NOT NULL,"
                "    UnitSymbol TEXT,"
                "    UnitType TEXT CHECK (UnitType IN ('Mass', 'Volume', 'Count', 'Length'))"
                ")",
                
                // 创建食物营养数据表
                "CREATE TABLE IF NOT EXISTS FoodNutrition ("
                "    NutritionID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    FoodID INTEGER NOT NULL,"
                "    UnitID INTEGER NOT NULL,"
                "    Amount REAL NOT NULL,"
                "    Calories REAL,"
                "    Protein REAL,"
                "    Fat REAL,"
                "    Carbohydrates REAL,"
                "    Fiber REAL,"
                "    Sugar REAL,"
                "    Calcium REAL,"
                "    Iron REAL,"
                "    Sodium REAL,"
                "    Potassium REAL,"
                "    VitaminA REAL,"
                "    VitaminC REAL,"
                "    CreatedDate DATETIME DEFAULT CURRENT_TIMESTAMP,"
                "    ModifiedDate DATETIME DEFAULT CURRENT_TIMESTAMP,"
                "    FOREIGN KEY (FoodID) REFERENCES Foods(FoodID),"
                "    FOREIGN KEY (UnitID) REFERENCES Units(UnitID)"
                ")",
                
                // 创建营养知识题库表
                "CREATE TABLE IF NOT EXISTS NutritionQuestions ("
                "    QuestionID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    QuestionText TEXT NOT NULL,"
                "    OptionA TEXT NOT NULL,"
                "    OptionB TEXT NOT NULL,"
                "    OptionC TEXT NOT NULL,"
                "    OptionD TEXT NOT NULL,"
                "    CorrectAnswer TEXT NOT NULL CHECK (CorrectAnswer IN ('A', 'B', 'C', 'D')),"
                "    Explanation TEXT,"
                "    DifficultyLevel INTEGER CHECK (DifficultyLevel BETWEEN 1 AND 3),"
                "    Category TEXT,"
                "    CreatedDate DATETIME DEFAULT CURRENT_TIMESTAMP"
                ")",
                
                // 创建营养知识内容表
                "CREATE TABLE IF NOT EXISTS NutritionKnowledge ("
                "    KnowledgeID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    Title TEXT NOT NULL,"
                "    Content TEXT NOT NULL,"
                "    Category TEXT,"
                "    ImagePath TEXT,"
                "    CreatedDate DATETIME DEFAULT CURRENT_TIMESTAMP"
                ")",
                
                // 创建用户答题记录表
                "CREATE TABLE IF NOT EXISTS UserAnswerRecords ("
                "    RecordID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    Username TEXT NOT NULL,"
                "    QuestionID INTEGER NOT NULL,"
                "    UserAnswer TEXT NOT NULL,"
                "    IsCorrect INTEGER NOT NULL,"
                "    AnswerTime DATETIME DEFAULT CURRENT_TIMESTAMP,"
                "    FOREIGN KEY (QuestionID) REFERENCES NutritionQuestions(QuestionID)"
                ")"
            };
            
            // 执行表创建语句
             for (const QString& statement : sqlStatements) {
                 QSqlQuery scriptQuery(database);
                 if (!scriptQuery.exec(statement)) {
                     qDebug() << "执行SQL语句失败:" << statement;
                     qDebug() << "错误信息:" << scriptQuery.lastError().text();
                 }
             }
             
             // 创建视图
             QStringList viewStatements = {
                 "CREATE VIEW IF NOT EXISTS vw_FoodNutrition AS "
                 "SELECT f.FoodID, f.FoodName, c.CategoryName, fn.Amount, u.UnitName, "
                 "fn.Calories, fn.Protein, fn.Fat, fn.Carbohydrates, fn.Fiber, fn.Sugar, "
                 "fn.Calcium, fn.Iron, fn.Sodium, fn.Potassium, fn.VitaminA, fn.VitaminC "
                 "FROM Foods f "
                 "JOIN Categories c ON f.CategoryID = c.CategoryID "
                 "JOIN FoodNutrition fn ON f.FoodID = fn.FoodID "
                 "JOIN Units u ON fn.UnitID = u.UnitID"
             };
             
             for (const QString& statement : viewStatements) {
                 QSqlQuery scriptQuery(database);
                 if (!scriptQuery.exec(statement)) {
                     qDebug() << "创建视图失败:" << statement;
                     qDebug() << "错误信息:" << scriptQuery.lastError().text();
                 }
             }
             
             // 插入示例数据
             QStringList insertStatements = {
                 // 插入食物类别
                 "INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES ('水果', '多汁且主要甜味的植物果实', NULL)",
                 "INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES ('蔬菜', '可作为食物的草本植物部分', NULL)",
                 "INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES ('谷物', '禾本科植物的种子', NULL)",
                 "INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES ('蛋白质', '富含蛋白质的食物', NULL)",
                 "INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES ('乳制品', '以奶为基础的食品', 4)",
                 "INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES ('肉类', '动物的可食用组织', 4)",
                 "INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES ('豆类', '豆科植物的种子', 4)",
                 "INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES ('坚果和种子', '可食用的坚果和种子', 4)",
                 "INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES ('脂肪和油', '高能量密度的食物', NULL)",
                 "INSERT OR IGNORE INTO Categories (CategoryName, Description, ParentCategoryID) VALUES ('糖类', '甜味的碳水化合物', NULL)",
                 
                 // 插入计量单位
                 "INSERT OR IGNORE INTO Units (UnitName, UnitSymbol, UnitType) VALUES ('克', 'g', 'Mass')",
                 "INSERT OR IGNORE INTO Units (UnitName, UnitSymbol, UnitType) VALUES ('千克', 'kg', 'Mass')",
                 "INSERT OR IGNORE INTO Units (UnitName, UnitSymbol, UnitType) VALUES ('毫升', 'ml', 'Volume')",
                 "INSERT OR IGNORE INTO Units (UnitName, UnitSymbol, UnitType) VALUES ('升', 'L', 'Volume')",
                 "INSERT OR IGNORE INTO Units (UnitName, UnitSymbol, UnitType) VALUES ('个', '', 'Count')",
                 "INSERT OR IGNORE INTO Units (UnitName, UnitSymbol, UnitType) VALUES ('杯', 'c', 'Volume')",
                 "INSERT OR IGNORE INTO Units (UnitName, UnitSymbol, UnitType) VALUES ('汤匙', 'tbsp', 'Volume')",
                 "INSERT OR IGNORE INTO Units (UnitName, UnitSymbol, UnitType) VALUES ('茶匙', 'tsp', 'Volume')"
             };
             
             for (const QString& statement : insertStatements) {
                  QSqlQuery scriptQuery(database);
                  if (!scriptQuery.exec(statement)) {
                      qDebug() << "插入数据失败:" << statement;
                      qDebug() << "错误信息:" << scriptQuery.lastError().text();
                  }
              }
              
              // 插入营养知识内容
              QStringList knowledgeStatements = {
                  "INSERT OR IGNORE INTO NutritionKnowledge (Title, Content, Category) VALUES ('什么是均衡饮食？', '均衡饮食是指摄入适量的各种营养素，包括碳水化合物、蛋白质、脂肪、维生素和矿物质。每天应该摄入多种不同颜色的蔬菜和水果，选择全谷物食品，适量摄入优质蛋白质，限制加工食品和高糖食品的摄入。', '基础营养')",
                  "INSERT OR IGNORE INTO NutritionKnowledge (Title, Content, Category) VALUES ('碳水化合物的重要性', '碳水化合物是人体的主要能量来源，特别是大脑的唯一能量来源。应该选择复合碳水化合物，如全麦面包、糙米、燕麦等，而不是简单糖类。复合碳水化合物能提供持续的能量，并含有丰富的膳食纤维。', '碳水化合物')",
                  "INSERT OR IGNORE INTO NutritionKnowledge (Title, Content, Category) VALUES ('蛋白质的作用', '蛋白质是构成人体组织的重要成分，参与酶和激素的合成，维持免疫功能。优质蛋白质来源包括瘦肉、鱼类、蛋类、豆类和坚果。成人每天应摄入体重（公斤）×0.8-1.2克的蛋白质。', '蛋白质')",
                  "INSERT OR IGNORE INTO NutritionKnowledge (Title, Content, Category) VALUES ('健康脂肪的选择', '脂肪是必需营养素，但要选择健康的脂肪。不饱和脂肪酸（如橄榄油、鱼油、坚果中的脂肪）对心脏健康有益，而应该限制饱和脂肪和反式脂肪的摄入。', '脂肪')",
                  "INSERT OR IGNORE INTO NutritionKnowledge (Title, Content, Category) VALUES ('维生素和矿物质', '维生素和矿物质虽然需要量不大，但对维持正常生理功能至关重要。通过多样化的饮食，特别是多吃蔬菜水果，通常能满足大部分维生素和矿物质的需求。', '维生素矿物质')"
              };
              
              for (const QString& statement : knowledgeStatements) {
                   QSqlQuery scriptQuery(database);
                   if (!scriptQuery.exec(statement)) {
                       qDebug() << "插入营养知识失败:" << statement;
                       qDebug() << "错误信息:" << scriptQuery.lastError().text();
                   }
               }
               
               // 插入营养知识题目
               QStringList questionStatements = {
                   "INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES ('以下哪种食物富含优质蛋白质？', '苹果', '鸡胸肉', '白米饭', '橄榄油', 'B', '鸡胸肉是优质蛋白质的良好来源，每100克含有约31克蛋白质。', 1, '蛋白质')",
                   "INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES ('维生素C主要存在于以下哪种食物中？', '牛奶', '柑橘类水果', '鸡蛋', '牛肉', 'B', '柑橘类水果如橙子、柠檬等富含维生素C。', 1, '维生素')",
                   "INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES ('膳食纤维对人体的主要作用是？', '提供能量', '促进肠道蠕动', '增强免疫力', '帮助钙吸收', 'B', '膳食纤维可以促进肠道蠕动，预防便秘等问题。', 1, '膳食纤维')",
                   "INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES ('以下哪种食物属于全谷物？', '白面包', '燕麦片', '蛋糕', '饼干', 'B', '燕麦片是全谷物食品，保留了谷物的麸皮、胚芽和胚乳。', 1, '全谷物')",
                   "INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES ('铁元素的主要功能是什么？', '维持骨骼健康', '帮助氧气运输', '调节血压', '促进伤口愈合', 'B', '铁是血红蛋白的组成成分，帮助氧气在体内运输。', 1, '矿物质')",
                   "INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES ('以下哪种脂肪对心脏健康有益？', '饱和脂肪', '反式脂肪', '不饱和脂肪', '胆固醇', 'C', '不饱和脂肪如橄榄油、鱼油等对心脏健康有益。', 1, '脂肪')",
                   "INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES ('成人每天建议的饮水量是多少？', '500毫升', '1000毫升', '1500-2000毫升', '3000毫升以上', 'C', '成人每天建议饮水量为1500-2000毫升，具体因个体差异而异。', 1, '水')",
                   "INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES ('以下哪种食物含有丰富的钙？', '菠菜', '牛奶', '西红柿', '米饭', 'B', '牛奶是钙的优质来源，每100毫升约含有120毫克钙。', 1, '矿物质')",
                   "INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES ('以下哪种维生素属于脂溶性维生素？', '维生素C', '维生素B群', '维生素A', '维生素B12', 'C', '维生素A、D、E、K属于脂溶性维生素。', 1, '维生素')",
                   "INSERT OR IGNORE INTO NutritionQuestions (QuestionText, OptionA, OptionB, OptionC, OptionD, CorrectAnswer, Explanation, DifficultyLevel, Category) VALUES ('膳食纤维主要存在于以下哪种食物中？', '肉类', '水果和蔬菜', '乳制品', '油脂', 'B', '水果和蔬菜是膳食纤维的主要来源。', 1, '膳食纤维')"
               };
               
               for (const QString& statement : questionStatements) {
                   QSqlQuery scriptQuery(database);
                   if (!scriptQuery.exec(statement)) {
                       qDebug() << "插入营养题目失败:" << statement;
                       qDebug() << "错误信息:" << scriptQuery.lastError().text();
                   }
               }
               
               // 创建索引
               QStringList indexStatements = {
                   "CREATE INDEX IF NOT EXISTS idx_Foods_CategoryID ON Foods(CategoryID)",
                   "CREATE INDEX IF NOT EXISTS idx_FoodNutrition_FoodID ON FoodNutrition(FoodID)",
                   "CREATE INDEX IF NOT EXISTS idx_Categories_ParentCategoryID ON Categories(ParentCategoryID)",
                   "CREATE INDEX IF NOT EXISTS idx_NutritionQuestions_Category ON NutritionQuestions(Category)",
                   "CREATE INDEX IF NOT EXISTS idx_NutritionQuestions_DifficultyLevel ON NutritionQuestions(DifficultyLevel)",
                   "CREATE INDEX IF NOT EXISTS idx_NutritionKnowledge_Category ON NutritionKnowledge(Category)",
                   "CREATE INDEX IF NOT EXISTS idx_UserAnswerRecords_Username ON UserAnswerRecords(Username)",
                   "CREATE INDEX IF NOT EXISTS idx_UserAnswerRecords_QuestionID ON UserAnswerRecords(QuestionID)"
               };
               
               for (const QString& statement : indexStatements) {
                   QSqlQuery scriptQuery(database);
                   if (!scriptQuery.exec(statement)) {
                       qDebug() << "创建索引失败:" << statement;
                       qDebug() << "错误信息:" << scriptQuery.lastError().text();
                   }
               }
               
               qDebug() << "数据库初始化脚本执行完成";
            
            // 插入默认测试用户
            QSqlQuery insertQuery(database);
            insertQuery.prepare("INSERT OR IGNORE INTO users (username, password) VALUES (?, ?)");
            insertQuery.addBindValue("admin");
            insertQuery.addBindValue("admin123");
            insertQuery.exec();
            
            insertQuery.prepare("INSERT OR IGNORE INTO users (username, password) VALUES (?, ?)");
            insertQuery.addBindValue("test_user");
            insertQuery.addBindValue("test123");
            insertQuery.exec();
            
            qDebug() << "默认用户创建完成";
        }
    }
}

void LoginWindow::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入用户名和密码！");
        return;
    }
    
    if (validateLogin(username, password)) {
        QMessageBox::information(this, "登录成功", "欢迎回来，" + username + "！");
        emit loginSuccessful();
        this->hide();
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码错误！");
        passwordEdit->clear();
        passwordEdit->setFocus();
    }
}

void LoginWindow::onRegisterClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入用户名和密码！");
        return;
    }
    
    if (username.length() < 3) {
        QMessageBox::warning(this, "注册失败", "用户名长度至少3个字符！");
        return;
    }
    
    if (password.length() < 6) {
        QMessageBox::warning(this, "注册失败", "密码长度至少6个字符！");
        return;
    }
    
    if (registerUser(username, password)) {
        QMessageBox::information(this, "注册成功", "注册成功！请使用新账号登录。");
        passwordEdit->clear();
    } else {
        QMessageBox::warning(this, "注册失败", "用户名已存在或注册失败！");
    }
}

bool LoginWindow::validateLogin(const QString &username, const QString &password)
{
    if (!database.isOpen()) {
        return false;
    }
    
    QSqlQuery query(database);
    query.prepare("SELECT password FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        QString storedPassword = query.value(0).toString();
        return storedPassword == password; // 实际项目中应该使用密码哈希
    }
    
    return false;
}

bool LoginWindow::registerUser(const QString &username, const QString &password)
{
    if (!database.isOpen()) {
        return false;
    }
    
    QSqlQuery query(database);
    query.prepare("INSERT INTO users (username, password) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(password); // 实际项目中应该使用密码哈希
    
    return query.exec();
}

void LoginWindow::applyStyles()
{
    setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #e8f4fd, stop:1 #c8e6f5);"
        "    font-family: 'PingFang SC', 'Hiragino Sans GB', 'Arial Unicode MS', 'Helvetica Neue';"
        "}"
        
        "#titleLabel {"
        "    font-size: 24px;"
        "    font-weight: bold;"
        "    color: #2c5aa0;"
        "    margin: 10px 0;"
        "}"
        
        "#logoLabel {"
        "    font-size: 48px;"
        "    margin: 10px 0;"
        "}"
        
        "#inputLabel {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: #333;"
        "    margin-bottom: 5px;"
        "}"
        
        "#inputEdit {"
        "    padding: 12px 15px;"
        "    border: 2px solid #ddd;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    background: white;"
        "    selection-background-color: #4a90e2;"
        "}"
        
        "#inputEdit:focus {"
        "    border-color: #4a90e2;"
        "    outline: none;"
        "}"
        
        "#primaryButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #5cb3f7, stop:1 #4a90e2);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 12px 30px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    min-width: 100px;"
        "}"
        
        "#primaryButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #4a90e2, stop:1 #357abd);"
        "}"
        
        "#primaryButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #357abd, stop:1 #2c5aa0);"
        "}"
        
        "#secondaryButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #f0f0f0, stop:1 #e0e0e0);"
        "    color: #333;"
        "    border: 2px solid #ccc;"
        "    border-radius: 8px;"
        "    padding: 12px 30px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    min-width: 100px;"
        "}"
        
        "#secondaryButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #e0e0e0, stop:1 #d0d0d0);"
        "    border-color: #999;"
        "}"
        
        "#secondaryButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #d0d0d0, stop:1 #c0c0c0);"
        "}"
    );
}