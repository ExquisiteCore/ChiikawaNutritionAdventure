#ifndef CARBOHYDRATE_GAME_WINDOW_H
#define CARBOHYDRATE_GAME_WINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QKeyEvent>
#include "carbohydrate_game_scene.h"
#include "carbohydrate_config.h"

class CarbohydrateGameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CarbohydrateGameWindow(QWidget *parent = nullptr);
    ~CarbohydrateGameWindow();
    
    // 游戏控制
    void startNewGame();
    void showGameInstructions();
    
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    
signals:
    void gameWindowClosed();
    
private slots:
    void onGameWon();
    void onGameLost();
    void onGameStateChanged(GameState newState);
    void onBackButtonClicked();
    void onRestartButtonClicked();
    void onInstructionsButtonClicked();
    
private:
    void setupUI();
    void setupGameArea();
    void setupControlPanel();
    void updateControlPanel();
    void showGameResult(bool won);
    
    // UI组件
    QVBoxLayout* mainLayout;
    QHBoxLayout* gameLayout;
    QVBoxLayout* controlLayout;
    
    // 游戏区域
    CarbohydrateGameScene* gameScene;
    CarbohydrateGameView* gameView;
    
    // 控制面板
    QLabel* titleLabel;
    QLabel* statusLabel;
    QPushButton* backButton;
    QPushButton* restartButton;
    QPushButton* instructionsButton;
    
    // 游戏状态
    bool gameInProgress;
};

#endif // CARBOHYDRATE_GAME_WINDOW_H