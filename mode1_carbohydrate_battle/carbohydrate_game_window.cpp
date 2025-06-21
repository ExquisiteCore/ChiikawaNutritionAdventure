#include "carbohydrate_game_window.h"
#include <QApplication>
#include <QCloseEvent>
#include <QFont>
#include <QSpacerItem>
#include <QDebug>

CarbohydrateGameWindow::CarbohydrateGameWindow(QWidget *parent)
    : QWidget(parent)
    , gameScene(nullptr)
    , gameView(nullptr)
    , gameInProgress(false)
{
    setWindowTitle("碳水化合物之战 - 膳食纤维剑击败伪蔬菜BOSS");
    setFixedSize(GAME_WINDOW_WIDTH + 200, GAME_WINDOW_HEIGHT);
    
    setupUI();
    startNewGame();
}

CarbohydrateGameWindow::~CarbohydrateGameWindow()
{
}

void CarbohydrateGameWindow::setupUI()
{
    // 创建主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // 创建游戏区域布局
    gameLayout = new QHBoxLayout();
    gameLayout->setSpacing(10);
    
    setupGameArea();
    setupControlPanel();
    
    // 添加到主布局
    mainLayout->addLayout(gameLayout);
    
    // 设置样式
    setStyleSheet(
        "QWidget { background-color: #2b2b2b; }"
        "QLabel { color: white; }"
        "QPushButton {"
        "    background-color: #4a4a4a;"
        "    color: white;"
        "    border: 2px solid #6a6a6a;"
        "    border-radius: 5px;"
        "    padding: 8px 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #5a5a5a;"
        "    border-color: #7a7a7a;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3a3a3a;"
        "}"
    );
}

void CarbohydrateGameWindow::setupGameArea()
{
    // 创建游戏场景和视图
    gameScene = new CarbohydrateGameScene(this);
    gameView = new CarbohydrateGameView(gameScene, this);
    
    // 连接游戏信号
    connect(gameScene, &CarbohydrateGameScene::gameWon, this, &CarbohydrateGameWindow::onGameWon);
    connect(gameScene, &CarbohydrateGameScene::gameLost, this, &CarbohydrateGameWindow::onGameLost);
    connect(gameScene, &CarbohydrateGameScene::gameStateChanged, this, &CarbohydrateGameWindow::onGameStateChanged);
    
    // 添加到布局
    gameLayout->addWidget(gameView);
}

void CarbohydrateGameWindow::setupControlPanel()
{
    // 创建控制面板布局
    controlLayout = new QVBoxLayout();
    controlLayout->setSpacing(15);
    
    // 标题
    titleLabel = new QLabel("碳水化合物之战");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #ffcc00; margin-bottom: 10px;");
    controlLayout->addWidget(titleLabel);
    
    // 状态标签
    statusLabel = new QLabel("准备开始游戏");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("color: #cccccc; font-size: 14px; margin-bottom: 20px;");
    controlLayout->addWidget(statusLabel);
    
    // 添加弹性空间
    controlLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    
    // 控制按钮
    restartButton = new QPushButton("重新开始");
    connect(restartButton, &QPushButton::clicked, this, &CarbohydrateGameWindow::onRestartButtonClicked);
    controlLayout->addWidget(restartButton);
    
    instructionsButton = new QPushButton("游戏说明");
    connect(instructionsButton, &QPushButton::clicked, this, &CarbohydrateGameWindow::onInstructionsButtonClicked);
    controlLayout->addWidget(instructionsButton);
    
    backButton = new QPushButton("返回主菜单");
    connect(backButton, &QPushButton::clicked, this, &CarbohydrateGameWindow::onBackButtonClicked);
    controlLayout->addWidget(backButton);
    
    // 添加弹性空间
    controlLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    
    // 游戏说明
    QLabel* instructionLabel = new QLabel(
        "游戏目标:\n"
        "使用膳食纤维剑击败伪蔬菜BOSS\n\n"
        "操作方法:\n"
        "• WASD 或方向键移动\n"
        "• 空格键发射膳食纤维剑\n"
        "• 每次攻击消耗10点纤维值\n"
        "• 避免被BOSS抓住\n\n"
        "获胜条件:\n"
        "将BOSS血量降至0"
    );
    instructionLabel->setStyleSheet(
        "color: #aaaaaa; "
        "font-size: 12px; "
        "background-color: rgba(0, 0, 0, 100); "
        "border: 1px solid #555; "
        "border-radius: 5px; "
        "padding: 10px;"
    );
    instructionLabel->setWordWrap(true);
    controlLayout->addWidget(instructionLabel);
    
    // 设置控制面板固定宽度
    QWidget* controlWidget = new QWidget();
    controlWidget->setLayout(controlLayout);
    controlWidget->setFixedWidth(180);
    
    gameLayout->addWidget(controlWidget);
}

void CarbohydrateGameWindow::startNewGame()
{
    if (gameScene) {
        gameInProgress = true;
        updateControlPanel();
        
        // 设置焦点到游戏视图
        if (gameView) {
            gameView->setFocus();
        }
    }
}

void CarbohydrateGameWindow::showGameInstructions()
{
    QMessageBox::information(this, "游戏说明",
        "<h3>碳水化合物之战</h3>"
        "<p><b>游戏背景：</b><br>"
        "在营养世界中，伪蔬菜BOSS正在传播错误的营养信息！"
        "你需要使用膳食纤维剑来击败它，保护真正的营养知识。</p>"
        
        "<p><b>游戏目标：</b><br>"
        "使用膳食纤维剑攻击伪蔬菜BOSS，将其血量降至0。</p>"
        
        "<p><b>操作方法：</b><br>"
        "• <b>移动：</b> WASD键 或 方向键<br>"
        "• <b>攻击：</b> 空格键发射膳食纤维剑<br>"
        "• <b>暂停：</b> 点击游戏内的暂停按钮</p>"
        
        "<p><b>游戏机制：</b><br>"
        "• 初始膳食纤维值：100点<br>"
        "• 每次攻击消耗：10点纤维值<br>"
        "• 膳食纤维剑伤害：25点<br>"
        "• BOSS总血量：100点<br>"
        "• 避免被BOSS抓住，否则游戏失败</p>"
        
        "<p><b>获胜条件：</b><br>"
        "成功将BOSS血量降至0即可获胜！</p>"
    );
}

void CarbohydrateGameWindow::updateControlPanel()
{
    if (!gameScene) {
        return;
    }
    
    GameState state = gameScene->getCurrentState();
    
    switch (state) {
    case GAME_READY:
        statusLabel->setText("准备开始游戏\n按任意键开始");
        break;
    case GAME_RUNNING:
        statusLabel->setText("游戏进行中\n击败伪蔬菜BOSS!");
        break;
    case GAME_PAUSED:
        statusLabel->setText("游戏已暂停\n点击继续按钮恢复");
        break;
    case GAME_WIN:
        statusLabel->setText("恭喜获胜！\n成功击败了BOSS");
        gameInProgress = false;
        break;
    case GAME_LOSE:
        statusLabel->setText("游戏失败\n被BOSS抓住了");
        gameInProgress = false;
        break;
    }
}

void CarbohydrateGameWindow::showGameResult(bool won)
{
    QString title = won ? "游戏胜利!" : "游戏失败!";
    QString message;
    
    if (won) {
        message = "<h3>恭喜！</h3>"
                 "<p>你成功击败了伪蔬菜BOSS！</p>"
                 "<p>膳食纤维剑发挥了强大的作用，"
                 "保护了营养世界的和平。</p>"
                 "<p>真正的蔬菜富含膳食纤维，"
                 "是健康饮食的重要组成部分！</p>";
    } else {
        message = "<h3>很遗憾...</h3>"
                 "<p>你被伪蔬菜BOSS抓住了！</p>"
                 "<p>不要灰心，多了解膳食纤维的知识，"
                 "下次一定能够成功击败BOSS。</p>"
                 "<p>记住：真正的蔬菜富含膳食纤维，"
                 "有助于消化和健康！</p>";
    }
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void CarbohydrateGameWindow::keyPressEvent(QKeyEvent *event)
{
    if (gameView) {
        // 直接转发键盘事件到游戏场景
        if (gameScene) {
            gameScene->handleKeyPress(event);
        }
    }
    QWidget::keyPressEvent(event);
}

void CarbohydrateGameWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (gameView) {
        // 直接转发键盘事件到游戏场景
        if (gameScene) {
            gameScene->handleKeyRelease(event);
        }
    }
    QWidget::keyReleaseEvent(event);
}

void CarbohydrateGameWindow::closeEvent(QCloseEvent *event)
{
    emit gameWindowClosed();
    event->accept();
}

void CarbohydrateGameWindow::onGameWon()
{
    showGameResult(true);
    updateControlPanel();
}

void CarbohydrateGameWindow::onGameLost()
{
    showGameResult(false);
    updateControlPanel();
}

void CarbohydrateGameWindow::onGameStateChanged(GameState newState)
{
    Q_UNUSED(newState);
    updateControlPanel();
}

void CarbohydrateGameWindow::onBackButtonClicked()
{
    emit gameWindowClosed();
    close();
}

void CarbohydrateGameWindow::onRestartButtonClicked()
{
    // 重新创建游戏场景
    if (gameScene) {
        gameLayout->removeWidget(gameView);
        delete gameView;
        delete gameScene;
    }
    
    setupGameArea();
    startNewGame();
}

void CarbohydrateGameWindow::onInstructionsButtonClicked()
{
    showGameInstructions();
}