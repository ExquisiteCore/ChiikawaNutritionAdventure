#ifndef SUGAR_OIL_MAIN_WINDOW_H
#define SUGAR_OIL_MAIN_WINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QStatusBar>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>

#include "sugar_oil_game_scene_new.h"

class SugarOilMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SugarOilMainWindow(QWidget *parent = nullptr);
    virtual ~SugarOilMainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onStartGame();
    void onPauseGame();
    void onResumeGame();
    void onStopGame();
    void onResetGame();
    void onExitGame();
    void onShowHelp();
    void onShowAbout();
    
    // 游戏事件槽
    void onGameStarted();
    void onGamePaused();
    void onGameResumed();
    void onGameOver(int finalScore, int finalLevel);
    void onGameWon(int finalScore, int finalLevel);
    void onScoreChanged(int score);
    void onTimeChanged(int timeLeft);
    void onPlayerStatsChanged(int hp, int maxHp, int level, int exp);
    
private:
    void setupUI();
    void setupMenuBar();
    void setupGameView();
    void setupControlPanel();
    void setupStatusBar();
    void connectSignals();
    
    void updateGameControls();
    void showGameOverDialog(const QString &title, const QString &message, int score, int level);
    
    QPointF mapToScene(const QPoint &viewPos);
    
    // UI组件
    QWidget* mCentralWidget;
    QHBoxLayout* mMainLayout;
    
    // 游戏视图
    QGraphicsView* mGameView;
    SugarOilGameSceneNew* mGameScene;
    
    // 控制面板
    QWidget* mControlPanel;
    QVBoxLayout* mControlLayout;
    
    // 游戏信息显示
    QLabel* mScoreLabel;
    QLabel* mTimeLabel;
    QLabel* mLevelLabel;
    QProgressBar* mHealthBar;
    QProgressBar* mExpBar;
    
    // 控制按钮
    QPushButton* mStartButton;
    QPushButton* mPauseButton;
    QPushButton* mResumeButton;
    QPushButton* mStopButton;
    QPushButton* mResetButton;
    QPushButton* mExitButton;
    
    // 菜单和动作
    QMenuBar* mMenuBar;
    QMenu* mGameMenu;
    QMenu* mHelpMenu;
    QAction* mStartAction;
    QAction* mPauseAction;
    QAction* mResumeAction;
    QAction* mStopAction;
    QAction* mResetAction;
    QAction* mExitAction;
    QAction* mHelpAction;
    QAction* mAboutAction;
    
    // 状态栏
    QStatusBar* mStatusBar;
    QLabel* mStatusLabel;
    
    // 游戏状态
    bool mGameActive;
    
    // 样式常量
    static const int CONTROL_PANEL_WIDTH = 200;
    static const int WINDOW_MIN_WIDTH = 1200;
    static const int WINDOW_MIN_HEIGHT = 800;
};

#endif // SUGAR_OIL_MAIN_WINDOW_H