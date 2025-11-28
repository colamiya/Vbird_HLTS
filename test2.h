#ifndef TEST2_H
#define TEST2_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QFile>
#include <QMessageBox>
#include <QTextEdit>
#include <QDialog>
#include <QScrollArea>

struct Question {
    QString text;
    QStringList options;
    int correctIndex;
    int userSelection = -1;
};

class Test2 : public QWidget {
    Q_OBJECT
public:
    explicit Test2(QWidget *parent = nullptr);

signals:
    void levelCompleted();
    void levelCancelled();
    void logMessage(QString msg);

private:
    QList<Question> questions;
    int currentQuestionIndex = 0;
    int quizScore = 0;

    QLabel *questionLabel;
    QWidget *optionsContainer;
    QButtonGroup *optionGroup;
    QPushButton *optionButtons[4];
    QPushButton *optionImages[4];
    QPushButton *nextQBtn;
    QLabel *scoreLabel;

    void loadQuestion();
    void handleOptionSelect(int index);
    void handleNextOrSubmit();
    void showQuizSummary();
    void showImagePreview(QString imagePath, const QString &title);
};

#endif // TEST2_H
