#ifndef SUGAR_OIL_GAME_WINDOW_H
#define SUGAR_OIL_GAME_WINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QProgressBar>
#include <QTimer>
#include <QCloseEvent>
#include <QGraphicsView>
#include "sugar_oil_game_scene_new.h"
#include "sugar_oil_config.h"

class SugarOilGameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SugarOilGameWindow(QWidget *parent = nullptr);
    ~SugarOilGameWindow();
    
    // 游戏控制
    void startNewGame();
    void showGameInstructions();
    
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    
signals:
    void gameWindowClosed();
    
private slots:
    void onGameWon();
    void onGameLost();
    void onGameStateChanged(SugarOilGameState newState);
    void onBackButtonClicked();
    void onRestartButtonClicked();
    void onInstructionsButtonClicked();
    void onTimeChanged(int remainingSeconds);
    void onLivesChanged(int lives);
    void onScoreChanged(int score);
    void onItemCollected(ItemType itemType);
    void onCreatureEncountered(CreatureType creatureType);
    
private:
    void setupUI();
    void setupGameArea();
    void setupControlPanel();
    void updateControlPanel();
    void showGameResult(bool won);
    void updateTimeDisplay(int seconds);
    void updateLivesDisplay(int lives);
    void updateScoreDisplay(int score);
    
    // UI组件
    QVBoxLayout* mainLayout;
    QHBoxLayout* gameAreaLayout;
    QVBoxLayout* controlPanelLayout;
    
    // 游戏区域
    QGraphicsView* gameView;
    SugarOilGameSceneNew* gameScene;
    
    // 控制面板
    QWidget* controlPanel;
    QLabel* titleLabel;
    QLabel* timeLabel;
    QProgressBar* timeProgressBar;
    QLabel* livesLabel;
    QLabel* scoreLabel;
    QLabel* instructionsLabel;
    
    // 按钮
    QPushButton* backButton;
    
    // 音频现在由AudioManager统一管理
    QPushButton* restartButton;
    QPushButton* instructionsButton;
    QPushButton* pauseButton;
    
    // 状态显示
    QLabel* statusLabel;
    QLabel* itemInfoLabel;
    QLabel* creatureInfoLabel;
    
    // 游戏数据
    int currentTime;
    int currentLives;
    int currentScore;
    int finalScore;
    int finalLevel;
    bool gameActive;
};

#endif // SUGAR_OIL_GAME_WINDOW_H