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
    database.setDatabaseName("chiikawa_game.db");
    
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