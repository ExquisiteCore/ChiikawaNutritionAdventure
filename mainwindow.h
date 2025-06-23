#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QDialog>
#include <QPixmap>
#include "loginwindow.h"
#include "mode1_carbohydrate_battle/carbohydrate_game_window.h"
#include "mode2_sugar_oil_battle/sugar_oil_game_window.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginSuccessful();
    void onGameIntroClicked();
    void onLevelsClicked();
    void onSettingsClicked();
    void onLogoutClicked();
    void onCarbohydrateBattleClicked();
    void onCarbohydrateGameClosed();
    void onSugarOilBattleClicked();
    void onSugarOilGameClosed();

private:
    void setupGameUI();
    void applyGameStyles();
    
    Ui::MainWindow *ui;
    LoginWindow *loginWindow;
    QWidget *gameWidget;
    QHBoxLayout *gameLayout;
    QVBoxLayout *buttonLayout;
    
    QLabel *welcomeLabel;
    QPushButton *gameIntroButton;
    QPushButton *levelsButton;
    QPushButton *settingsButton;
    QPushButton *logoutButton;
    
    // 游戏窗口
    CarbohydrateGameWindow *carbohydrateGameWindow;
    SugarOilGameWindow *sugarOilGameWindow;
};

#endif // MAINWINDOW_H
