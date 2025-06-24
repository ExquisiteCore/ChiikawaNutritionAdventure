#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QPixmap>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QFile>
#include <QTextStream>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
QT_END_NAMESPACE

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    void setupUI();
    void setupDatabase();
    bool validateLogin(const QString &username, const QString &password);
    bool registerUser(const QString &username, const QString &password);
    void applyStyles();

    QVBoxLayout *mainLayout;
    QVBoxLayout *formLayout;
    QHBoxLayout *buttonLayout;
    
    QLabel *titleLabel;
    QLabel *logoLabel;
    QLabel *usernameLabel;
    QLabel *passwordLabel;
    
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    
    QPushButton *loginButton;
    QPushButton *registerButton;
    
    QSqlDatabase database;

signals:
    void loginSuccessful();
};

#endif // LOGINWINDOW_H