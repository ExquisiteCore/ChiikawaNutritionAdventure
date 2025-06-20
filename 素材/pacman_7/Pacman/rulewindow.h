#ifndef RULEWINDOW_H
#define RULEWINDOW_H

#include <QWidget>
#include <QPainter>
#include <QSoundEffect>

#include "config.h"

namespace Ui {
class RuleWindow;
}

class RuleWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RuleWindow(QWidget *parent = nullptr);
    ~RuleWindow();

protected:
    void paintEvent(QPaintEvent *event) override;


signals:
    void closeWindow(void);

private:
    Ui::RuleWindow *ui;
};

#endif // RULEWINDOW_H
