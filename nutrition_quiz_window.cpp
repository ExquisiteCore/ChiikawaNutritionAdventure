#include "nutrition_quiz_window.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QRandomGenerator>
#include <QFont>
#include <QGraphicsDropShadowEffect>

NutritionQuizWindow::NutritionQuizWindow(QWidget *parent)
    : QWidget(parent)
    , currentKnowledgeIndex(0)
    , currentQuestionIndex(0)
    , correctAnswers(0)
    , totalQuestions(5)
{
    setupUI();
    applyStyles();
    
    // 设置窗口属性
    setWindowTitle("ちいかわ营养大冒险 - 营养知识宝典");
    setFixedSize(800, 600);
    
    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // 连接数据库
    database = QSqlDatabase::database();
    if (!database.isOpen()) {
        qDebug() << "数据库未连接，尝试重新连接...";
        database = QSqlDatabase::addDatabase("QSQLITE", "quiz_connection");
        QString dbPath = QApplication::applicationDirPath() + "/chiikawa_game.db";
        database.setDatabaseName(dbPath);
        if (!database.open()) {
            qDebug() << "答题界面数据库连接失败:" << database.lastError().text();
        }
    }
}

NutritionQuizWindow::~NutritionQuizWindow()
{
    if (database.isOpen() && database.connectionName() == "quiz_connection") {
        database.close();
    }
}

void NutritionQuizWindow::setupUI()
{
    stackedWidget = new QStackedWidget(this);
    
    setupKnowledgeUI();
    setupQuizUI();
    
    // 主布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(stackedWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // 默认显示主界面
    stackedWidget->setCurrentWidget(mainWidget);
}

void NutritionQuizWindow::setupKnowledgeUI()
{
    // 主界面
    mainWidget = new QWidget();
    mainLayout = new QVBoxLayout(mainWidget);
    
    titleLabel = new QLabel("营养知识宝典");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");
    
    messageLabel = new QLabel("很遗憾，游戏失败了！\n不过没关系，让我们一起学习营养知识吧！");
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setObjectName("messageLabel");
    
    knowledgeButton = new QPushButton("📖 打开宝典");
    knowledgeButton->setObjectName("knowledgeButton");
    connect(knowledgeButton, &QPushButton::clicked, this, &NutritionQuizWindow::onKnowledgeButtonClicked);
    
    mainLayout->addStretch();
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(30);
    mainLayout->addWidget(messageLabel);
    mainLayout->addSpacing(50);
    mainLayout->addWidget(knowledgeButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    
    stackedWidget->addWidget(mainWidget);
    
    // 知识宝典界面
    knowledgeWidget = new QWidget();
    knowledgeLayout = new QVBoxLayout(knowledgeWidget);
    
    knowledgeTitleLabel = new QLabel("营养知识宝典");
    knowledgeTitleLabel->setAlignment(Qt::AlignCenter);
    knowledgeTitleLabel->setObjectName("knowledgeTitleLabel");
    
    knowledgeScrollArea = new QScrollArea();
    knowledgeContentEdit = new QTextEdit();
    knowledgeContentEdit->setReadOnly(true);
    knowledgeContentEdit->setObjectName("knowledgeContentEdit");
    knowledgeScrollArea->setWidget(knowledgeContentEdit);
    knowledgeScrollArea->setWidgetResizable(true);
    
    knowledgeProgressBar = new QProgressBar();
    knowledgeProgressBar->setObjectName("knowledgeProgressBar");
    
    knowledgeNavLayout = new QHBoxLayout();
    prevKnowledgeButton = new QPushButton("⬅️ 上一页");
    nextKnowledgeButton = new QPushButton("下一页 ➡️");
    nextStepButton = new QPushButton("📝 开始答题");
    
    prevKnowledgeButton->setObjectName("navButton");
    nextKnowledgeButton->setObjectName("navButton");
    nextStepButton->setObjectName("nextStepButton");
    
    connect(prevKnowledgeButton, &QPushButton::clicked, [this]() {
        if (currentKnowledgeIndex > 0) {
            currentKnowledgeIndex--;
            updateKnowledgeDisplay(currentKnowledgeIndex);
        }
    });
    
    connect(nextKnowledgeButton, &QPushButton::clicked, [this]() {
        if (currentKnowledgeIndex < knowledgeItems.size() - 1) {
            currentKnowledgeIndex++;
            updateKnowledgeDisplay(currentKnowledgeIndex);
        }
    });
    
    connect(nextStepButton, &QPushButton::clicked, this, &NutritionQuizWindow::onNextStepClicked);
    
    knowledgeNavLayout->addWidget(prevKnowledgeButton);
    knowledgeNavLayout->addStretch();
    knowledgeNavLayout->addWidget(nextStepButton);
    knowledgeNavLayout->addStretch();
    knowledgeNavLayout->addWidget(nextKnowledgeButton);
    
    knowledgeLayout->addWidget(knowledgeTitleLabel);
    knowledgeLayout->addWidget(knowledgeProgressBar);
    knowledgeLayout->addSpacing(20);
    knowledgeLayout->addWidget(knowledgeScrollArea);
    knowledgeLayout->addSpacing(20);
    knowledgeLayout->addLayout(knowledgeNavLayout);
    
    stackedWidget->addWidget(knowledgeWidget);
}

void NutritionQuizWindow::setupQuizUI()
{
    // 答题界面
    quizWidget = new QWidget();
    quizLayout = new QVBoxLayout(quizWidget);
    
    quizTitleLabel = new QLabel("营养知识问答");
    quizTitleLabel->setAlignment(Qt::AlignCenter);
    quizTitleLabel->setObjectName("quizTitleLabel");
    
    quizProgressBar = new QProgressBar();
    quizProgressBar->setObjectName("quizProgressBar");
    
    scoreLabel = new QLabel("得分: 0/0");
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setObjectName("scoreLabel");
    
    questionLabel = new QLabel();
    questionLabel->setWordWrap(true);
    questionLabel->setObjectName("questionLabel");
    
    // 选项按钮
    answerButtonGroup = new QButtonGroup(this);
    optionAButton = new QRadioButton();
    optionBButton = new QRadioButton();
    optionCButton = new QRadioButton();
    optionDButton = new QRadioButton();
    
    optionAButton->setObjectName("optionButton");
    optionBButton->setObjectName("optionButton");
    optionCButton->setObjectName("optionButton");
    optionDButton->setObjectName("optionButton");
    
    answerButtonGroup->addButton(optionAButton, 0);
    answerButtonGroup->addButton(optionBButton, 1);
    answerButtonGroup->addButton(optionCButton, 2);
    answerButtonGroup->addButton(optionDButton, 3);
    
    connect(answerButtonGroup, &QButtonGroup::buttonClicked,
            this, [this](QAbstractButton*) { onAnswerSelected(); });
    
    submitAnswerButton = new QPushButton("提交答案");
    submitAnswerButton->setObjectName("submitButton");
    submitAnswerButton->setEnabled(false);
    connect(submitAnswerButton, &QPushButton::clicked, this, &NutritionQuizWindow::onSubmitAnswerClicked);
    
    resultLabel = new QLabel();
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setObjectName("resultLabel");
    resultLabel->hide();
    
    explanationEdit = new QTextEdit();
    explanationEdit->setReadOnly(true);
    explanationEdit->setObjectName("explanationEdit");
    explanationEdit->setMaximumHeight(100);
    explanationEdit->hide();
    
    nextQuestionButton = new QPushButton("下一题");
    nextQuestionButton->setObjectName("nextQuestionButton");
    nextQuestionButton->hide();
    connect(nextQuestionButton, &QPushButton::clicked, this, &NutritionQuizWindow::onNextQuestionClicked);
    
    quizLayout->addWidget(quizTitleLabel);
    quizLayout->addWidget(quizProgressBar);
    quizLayout->addWidget(scoreLabel);
    quizLayout->addSpacing(20);
    quizLayout->addWidget(questionLabel);
    quizLayout->addSpacing(15);
    quizLayout->addWidget(optionAButton);
    quizLayout->addWidget(optionBButton);
    quizLayout->addWidget(optionCButton);
    quizLayout->addWidget(optionDButton);
    quizLayout->addSpacing(20);
    quizLayout->addWidget(submitAnswerButton, 0, Qt::AlignCenter);
    quizLayout->addWidget(resultLabel);
    quizLayout->addWidget(explanationEdit);
    quizLayout->addWidget(nextQuestionButton, 0, Qt::AlignCenter);
    quizLayout->addStretch();
    
    stackedWidget->addWidget(quizWidget);
    
    // 结果界面
    resultWidget = new QWidget();
    resultLayout = new QVBoxLayout(resultWidget);
    
    finalResultLabel = new QLabel();
    finalResultLabel->setAlignment(Qt::AlignCenter);
    finalResultLabel->setObjectName("finalResultLabel");
    
    finalScoreLabel = new QLabel();
    finalScoreLabel->setAlignment(Qt::AlignCenter);
    finalScoreLabel->setObjectName("finalScoreLabel");
    
    QHBoxLayout* finalButtonLayout = new QHBoxLayout();
    retryButton = new QPushButton("🔄 重新答题");
    backToMenuButton = new QPushButton("🏠 返回主菜单");
    
    retryButton->setObjectName("finalButton");
    backToMenuButton->setObjectName("finalButton");
    
    connect(retryButton, &QPushButton::clicked, this, &NutritionQuizWindow::onRetryClicked);
    connect(backToMenuButton, &QPushButton::clicked, this, &NutritionQuizWindow::onBackToMenuClicked);
    
    finalButtonLayout->addWidget(retryButton);
    finalButtonLayout->addSpacing(20);
    finalButtonLayout->addWidget(backToMenuButton);
    
    resultLayout->addStretch();
    resultLayout->addWidget(finalResultLabel);
    resultLayout->addSpacing(30);
    resultLayout->addWidget(finalScoreLabel);
    resultLayout->addSpacing(50);
    resultLayout->addLayout(finalButtonLayout);
    resultLayout->addStretch();
    
    stackedWidget->addWidget(resultWidget);
}

void NutritionQuizWindow::applyStyles()
{
    setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #e8f4fd, stop:1 #b3d9ff);"
        "    font-family: 'Microsoft YaHei', 'SimHei', sans-serif;"
        "}"
        
        "#titleLabel, #knowledgeTitleLabel, #quizTitleLabel {"
        "    font-size: 28px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    margin: 20px;"
        "}"
        
        "#messageLabel {"
        "    font-size: 16px;"
        "    color: #34495e;"
        "    line-height: 1.6;"
        "}"
        
        "#knowledgeButton, #nextStepButton {"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    color: white;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #3498db, stop:1 #2980b9);"
        "    border: none;"
        "    border-radius: 25px;"
        "    padding: 15px 30px;"
        "    min-width: 200px;"
        "}"
        
        "#knowledgeButton:hover, #nextStepButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #2980b9, stop:1 #1f5f8b);"
        "}"
        
        "#navButton {"
        "    font-size: 14px;"
        "    color: white;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #95a5a6, stop:1 #7f8c8d);"
        "    border: none;"
        "    border-radius: 20px;"
        "    padding: 10px 20px;"
        "    min-width: 100px;"
        "}"
        
        "#navButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #7f8c8d, stop:1 #6c7b7d);"
        "}"
        
        "#knowledgeContentEdit, #explanationEdit {"
        "    background: white;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 10px;"
        "    padding: 15px;"
        "    font-size: 14px;"
        "    line-height: 1.6;"
        "}"
        
        "#questionLabel {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    background: white;"
        "    border: 2px solid #3498db;"
        "    border-radius: 10px;"
        "    padding: 15px;"
        "    margin: 10px 0;"
        "}"
        
        "#optionButton {"
        "    font-size: 14px;"
        "    color: #2c3e50;"
        "    background: white;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    padding: 10px 15px;"
        "    margin: 5px 0;"
        "}"
        
        "#optionButton:checked {"
        "    border-color: #3498db;"
        "    background: #ebf3fd;"
        "}"
        
        "#submitButton, #nextQuestionButton {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: white;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #27ae60, stop:1 #229954);"
        "    border: none;"
        "    border-radius: 20px;"
        "    padding: 12px 25px;"
        "    min-width: 120px;"
        "}"
        
        "#submitButton:hover, #nextQuestionButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #229954, stop:1 #1e7e34);"
        "}"
        
        "#submitButton:disabled {"
        "    background: #bdc3c7;"
        "    color: #7f8c8d;"
        "}"
        
        "#resultLabel {"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    padding: 10px;"
        "    border-radius: 8px;"
        "    margin: 10px 0;"
        "}"
        
        "#scoreLabel {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "}"
        
        "#finalResultLabel {"
        "    font-size: 24px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "}"
        
        "#finalScoreLabel {"
        "    font-size: 20px;"
        "    color: #34495e;"
        "}"
        
        "#finalButton {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: white;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #e74c3c, stop:1 #c0392b);"
        "    border: none;"
        "    border-radius: 20px;"
        "    padding: 12px 25px;"
        "    min-width: 150px;"
        "}"
        
        "#finalButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #c0392b, stop:1 #a93226);"
        "}"
        
        "QProgressBar {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    background: white;"
        "    text-align: center;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "}"
        
        "QProgressBar::chunk {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #3498db, stop:1 #2980b9);"
        "    border-radius: 6px;"
        "}"
    );
}

void NutritionQuizWindow::startQuiz()
{
    // 加载知识内容和题目
    if (!loadKnowledgeFromDatabase() || !loadQuestionsFromDatabase()) {
        QMessageBox::warning(this, "错误", "无法加载营养知识数据，请检查数据库连接。");
        return;
    }
    
    // 重置状态
    currentKnowledgeIndex = 0;
    currentQuestionIndex = 0;
    correctAnswers = 0;
    
    // 显示主界面
    stackedWidget->setCurrentWidget(mainWidget);
    show();
}

void NutritionQuizWindow::showKnowledge()
{
    displayKnowledge();
    stackedWidget->setCurrentWidget(knowledgeWidget);
}

bool NutritionQuizWindow::loadKnowledgeFromDatabase()
{
    knowledgeItems.clear();
    
    if (!database.isOpen()) {
        qDebug() << "数据库未连接";
        return false;
    }
    
    QSqlQuery query(database);
    query.prepare("SELECT KnowledgeID, Title, Content, Category, ImagePath FROM NutritionKnowledge ORDER BY KnowledgeID");
    
    if (!query.exec()) {
        qDebug() << "加载营养知识失败:" << query.lastError().text();
        return false;
    }
    
    while (query.next()) {
        KnowledgeItem item;
        item.knowledgeId = query.value(0).toInt();
        item.title = query.value(1).toString();
        item.content = query.value(2).toString();
        item.category = query.value(3).toString();
        item.imagePath = query.value(4).toString();
        knowledgeItems.append(item);
    }
    
    qDebug() << "加载了" << knowledgeItems.size() << "条营养知识";
    return !knowledgeItems.isEmpty();
}

bool NutritionQuizWindow::loadQuestionsFromDatabase()
{
    questions.clear();
    
    if (!database.isOpen()) {
        qDebug() << "数据库未连接";
        return false;
    }
    
    // 随机选择5道题目
    QSqlQuery query(database);
    query.prepare("SELECT QuestionID, QuestionText, OptionA, OptionB, OptionC, OptionD, "
                  "CorrectAnswer, Explanation, DifficultyLevel, Category "
                  "FROM NutritionQuestions ORDER BY RANDOM() LIMIT ?");
    query.addBindValue(totalQuestions);
    
    if (!query.exec()) {
        qDebug() << "加载营养题目失败:" << query.lastError().text();
        return false;
    }
    
    while (query.next()) {
        QuizQuestion question;
        question.questionId = query.value(0).toInt();
        question.questionText = query.value(1).toString();
        question.optionA = query.value(2).toString();
        question.optionB = query.value(3).toString();
        question.optionC = query.value(4).toString();
        question.optionD = query.value(5).toString();
        question.correctAnswer = query.value(6).toString();
        question.explanation = query.value(7).toString();
        question.difficultyLevel = query.value(8).toInt();
        question.category = query.value(9).toString();
        questions.append(question);
    }
    
    qDebug() << "加载了" << questions.size() << "道题目";
    return questions.size() == totalQuestions;
}

void NutritionQuizWindow::onKnowledgeButtonClicked()
{
    showKnowledge();
}

void NutritionQuizWindow::displayKnowledge()
{
    if (knowledgeItems.isEmpty()) {
        return;
    }
    
    updateKnowledgeDisplay(currentKnowledgeIndex);
}

void NutritionQuizWindow::updateKnowledgeDisplay(int index)
{
    if (index < 0 || index >= knowledgeItems.size()) {
        return;
    }
    
    const KnowledgeItem& item = knowledgeItems[index];
    
    QString content = QString("<h2 style='color: #2c3e50; text-align: center;'>%1</h2>")
                     .arg(item.title);
    
    if (!item.category.isEmpty()) {
        content += QString("<p style='color: #7f8c8d; text-align: center; font-style: italic;'>类别: %1</p>")
                  .arg(item.category);
    }
    
    content += QString("<div style='line-height: 1.8; font-size: 14px; color: #2c3e50;'>%1</div>")
              .arg(item.content);
    
    knowledgeContentEdit->setHtml(content);
    
    // 更新进度条
    knowledgeProgressBar->setMaximum(knowledgeItems.size());
    knowledgeProgressBar->setValue(index + 1);
    knowledgeProgressBar->setFormat(QString("第 %1 页，共 %2 页")
                                   .arg(index + 1).arg(knowledgeItems.size()));
    
    // 更新按钮状态
    prevKnowledgeButton->setEnabled(index > 0);
    nextKnowledgeButton->setEnabled(index < knowledgeItems.size() - 1);
}

void NutritionQuizWindow::onNextStepClicked()
{
    // 切换到答题界面
    currentQuestionIndex = 0;
    correctAnswers = 0;
    displayCurrentQuestion();
    stackedWidget->setCurrentWidget(quizWidget);
}

void NutritionQuizWindow::displayCurrentQuestion()
{
    if (currentQuestionIndex >= questions.size()) {
        showFinalResult();
        return;
    }
    
    const QuizQuestion& question = questions[currentQuestionIndex];
    
    questionLabel->setText(QString("第 %1 题: %2")
                          .arg(currentQuestionIndex + 1)
                          .arg(question.questionText));
    
    optionAButton->setText(QString("A. %1").arg(question.optionA));
    optionBButton->setText(QString("B. %1").arg(question.optionB));
    optionCButton->setText(QString("C. %1").arg(question.optionC));
    optionDButton->setText(QString("D. %1").arg(question.optionD));
    
    // 重置选择状态
    answerButtonGroup->setExclusive(false);
    optionAButton->setChecked(false);
    optionBButton->setChecked(false);
    optionCButton->setChecked(false);
    optionDButton->setChecked(false);
    answerButtonGroup->setExclusive(true);
    
    submitAnswerButton->setEnabled(false);
    submitAnswerButton->show();
    resultLabel->hide();
    explanationEdit->hide();
    nextQuestionButton->hide();
    
    // 更新进度条和得分
    quizProgressBar->setMaximum(totalQuestions);
    quizProgressBar->setValue(currentQuestionIndex + 1);
    quizProgressBar->setFormat(QString("第 %1 题，共 %2 题")
                              .arg(currentQuestionIndex + 1).arg(totalQuestions));
    
    scoreLabel->setText(QString("得分: %1/%2")
                       .arg(correctAnswers).arg(currentQuestionIndex));
}

void NutritionQuizWindow::onAnswerSelected()
{
    submitAnswerButton->setEnabled(true);
    
    // 获取选择的答案
    int selectedId = answerButtonGroup->checkedId();
    switch (selectedId) {
    case 0: currentUserAnswer = "A"; break;
    case 1: currentUserAnswer = "B"; break;
    case 2: currentUserAnswer = "C"; break;
    case 3: currentUserAnswer = "D"; break;
    default: currentUserAnswer = ""; break;
    }
}

void NutritionQuizWindow::onSubmitAnswerClicked()
{
    if (currentUserAnswer.isEmpty()) {
        return;
    }
    
    checkAnswer();
}

void NutritionQuizWindow::checkAnswer()
{
    const QuizQuestion& question = questions[currentQuestionIndex];
    bool isCorrect = (currentUserAnswer == question.correctAnswer);
    
    if (isCorrect) {
        correctAnswers++;
    }
    
    showQuestionResult(isCorrect);
    
    // 记录答题结果到数据库（可选）
    // recordAnswerToDatabase(question.questionId, currentUserAnswer, isCorrect);
}

void NutritionQuizWindow::showQuestionResult(bool isCorrect)
{
    const QuizQuestion& question = questions[currentQuestionIndex];
    
    submitAnswerButton->hide();
    
    if (isCorrect) {
        resultLabel->setText("🎉 回答正确！");
        resultLabel->setStyleSheet("#resultLabel { background: #d5f4e6; color: #27ae60; }");
    } else {
        resultLabel->setText(QString("❌ 回答错误！正确答案是: %1").arg(question.correctAnswer));
        resultLabel->setStyleSheet("#resultLabel { background: #fadbd8; color: #e74c3c; }");
    }
    
    resultLabel->show();
    
    // 显示解释
    if (!question.explanation.isEmpty()) {
        explanationEdit->setPlainText(question.explanation);
        explanationEdit->show();
    }
    
    // 更新得分显示
    scoreLabel->setText(QString("得分: %1/%2")
                       .arg(correctAnswers).arg(currentQuestionIndex + 1));
    
    // 显示下一题按钮
    if (currentQuestionIndex < totalQuestions - 1) {
        nextQuestionButton->setText("下一题");
    } else {
        nextQuestionButton->setText("查看结果");
    }
    nextQuestionButton->show();
}

void NutritionQuizWindow::onNextQuestionClicked()
{
    currentQuestionIndex++;
    
    if (currentQuestionIndex < totalQuestions) {
        displayCurrentQuestion();
    } else {
        showFinalResult();
    }
}

void NutritionQuizWindow::showFinalResult()
{
    double percentage = (double)correctAnswers / totalQuestions * 100;
    
    QString resultText;
    if (percentage >= 80) {
        resultText = "🎉 优秀！🎉\n你对营养知识掌握得很好！";
    } else if (percentage >= 60) {
        resultText = "👍 良好！👍\n继续加油，营养知识很重要！";
    } else {
        resultText = "💪 加油！💪\n多学习营养知识，健康生活！";
    }
    
    finalResultLabel->setText(resultText);
    finalScoreLabel->setText(QString("最终得分: %1/%2 (%.1f%%)")
                            .arg(correctAnswers).arg(totalQuestions).arg(percentage));
    
    stackedWidget->setCurrentWidget(resultWidget);
}

void NutritionQuizWindow::onRetryClicked()
{
    // 重新开始答题
    if (loadQuestionsFromDatabase()) {
        resetQuiz();
        displayCurrentQuestion();
        stackedWidget->setCurrentWidget(quizWidget);
    } else {
        QMessageBox::warning(this, "错误", "无法重新加载题目，请检查数据库连接。");
    }
}

void NutritionQuizWindow::onBackToMenuClicked()
{
    emit backToMenu();
    close();
}

void NutritionQuizWindow::resetQuiz()
{
    currentQuestionIndex = 0;
    correctAnswers = 0;
    currentUserAnswer.clear();
}

void NutritionQuizWindow::onFinishQuizClicked()
{
    emit quizCompleted();
    close();
}