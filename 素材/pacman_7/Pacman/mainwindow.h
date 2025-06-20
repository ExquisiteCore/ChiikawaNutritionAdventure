#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMovie>
#include <QPainter>
#include <QLabel>
#include <QPixmap>
#include <QSound>
#include <QTimer>

#include "config.h"
#include "chooselevelscene.h"
#include "rulewindow.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel *Background;
    QMovie *StarBackground;
    ChooseLevelScene* Level_Page;
    RuleWindow* Rule_Page;

    QSound* MainSound;

};
#endif // MAINWINDOW_H
