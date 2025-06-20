#ifndef CHOOSELEVELSCENE_H
#define CHOOSELEVELSCENE_H

#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QSound>
#include <QSoundEffect>
#include <QPushButton>

#include "config.h"
#include "gamewindow.h"

namespace Ui {
class ChooseLevelScene;
}

class ChooseLevelScene : public QWidget
{
    Q_OBJECT

public:
    explicit ChooseLevelScene(QSound* mainsound,QWidget *parent = nullptr);
    ~ChooseLevelScene();

public slots:
    void ChangeButton(void);

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void closeWindow(void);
    void GameStart(void);
    void GameEnd(void);

private:
    Ui::ChooseLevelScene *ui;
    QLabel *Background;
    QMovie *StarBackground;
    QPushButton* LevelBtn[4];
    GameWindow* Game_w;
    QSound* MainSound;
};

#endif // CHOOSELEVELSCENE_H
