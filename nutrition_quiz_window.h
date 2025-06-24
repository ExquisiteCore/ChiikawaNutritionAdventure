#ifndef NUTRITION_QUIZ_WINDOW_H
#define NUTRITION_QUIZ_WINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QScrollArea>
#include <QTextEdit>
#include <QProgressBar>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QStackedWidget>
#include <QFrame>

struct QuizQuestion {
    int questionId;
    QString questionText;
    QString optionA;
    QString optionB;
    QString optionC;
    QString optionD;
    QString correctAnswer;
    QString explanation;
    int difficultyLevel;
    QString category;
};

struct KnowledgeItem {
    int knowledgeId;
    QString title;
    QString content;
    QString category;
    QString imagePath;
};

class NutritionQuizWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NutritionQuizWindow(QWidget *parent = nullptr);
    ~NutritionQuizWindow();
    
    void startQuiz();
    void showKnowledge();

private slots:
    void onKnowledgeButtonClicked();
    void onNextStepClicked();
    void onAnswerSelected();
    void onSubmitAnswerClicked();
    void onNextQuestionClicked();
    void onFinishQuizClicked();
    void onRetryClicked();
    void onBackToMenuClicked();

private:
    void setupUI();
    void setupKnowledgeUI();
    void setupQuizUI();
    void applyStyles();
    
    // 数据库相关
    bool loadKnowledgeFromDatabase();
    bool loadQuestionsFromDatabase();
    
    // 知识宝典相关
    void displayKnowledge();
    void updateKnowledgeDisplay(int index);
    
    // 答题相关
    void displayCurrentQuestion();
    void checkAnswer();
    void showQuestionResult(bool isCorrect);
    void showFinalResult();
    void resetQuiz();
    
    // 动画效果
    void fadeInWidget(QWidget* widget);
    void fadeOutWidget(QWidget* widget);
    
    // UI组件
    QStackedWidget* stackedWidget;
    
    // 主界面
    QWidget* mainWidget;
    QVBoxLayout* mainLayout;
    QLabel* titleLabel;
    QLabel* messageLabel;
    QPushButton* knowledgeButton;
    
    // 知识宝典界面
    QWidget* knowledgeWidget;
    QVBoxLayout* knowledgeLayout;
    QLabel* knowledgeTitleLabel;
    QScrollArea* knowledgeScrollArea;
    QTextEdit* knowledgeContentEdit;
    QHBoxLayout* knowledgeNavLayout;
    QPushButton* prevKnowledgeButton;
    QPushButton* nextKnowledgeButton;
    QPushButton* nextStepButton;
    QProgressBar* knowledgeProgressBar;
    
    // 答题界面
    QWidget* quizWidget;
    QVBoxLayout* quizLayout;
    QLabel* quizTitleLabel;
    QLabel* questionLabel;
    QButtonGroup* answerButtonGroup;
    QRadioButton* optionAButton;
    QRadioButton* optionBButton;
    QRadioButton* optionCButton;
    QRadioButton* optionDButton;
    QPushButton* submitAnswerButton;
    QPushButton* nextQuestionButton;
    QLabel* resultLabel;
    QTextEdit* explanationEdit;
    QProgressBar* quizProgressBar;
    QLabel* scoreLabel;
    
    // 结果界面
    QWidget* resultWidget;
    QVBoxLayout* resultLayout;
    QLabel* finalResultLabel;
    QLabel* finalScoreLabel;
    QPushButton* retryButton;
    QPushButton* backToMenuButton;
    
    // 数据
    QList<KnowledgeItem> knowledgeItems;
    QList<QuizQuestion> questions;
    int currentKnowledgeIndex;
    int currentQuestionIndex;
    int correctAnswers;
    int totalQuestions;
    QString currentUserAnswer;
    
    // 数据库
    QSqlDatabase database;
    
signals:
    void quizCompleted();
    void backToMenu();
};

#endif // NUTRITION_QUIZ_WINDOW_H