#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QGridLayout>
#include <QListWidget>
#include <QRadioButton>
#include <QButtonGroup>
#include <QMap>
#include <QVector>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDrag>
#include <QTimer>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialog>
#include <QScrollArea>
#include <functional>

// --- Data Structures ---

struct StudentInfo {
    QString name;
    int age;
    QString gender;
    QString className;
    QString duration;
};

struct Task {
    int targetFloor;
    QMap<QString, int> requiredItems; // Type -> Count
    bool isEmergency;
    bool isCompleted;
};

struct Inventory {
    QMap<QString, int> currentItems; // Type -> Count
    int cartCapacity = 10;
};

enum class GameScene {
    Entrance,
    StaffHallway,
    Office,
    Warehouse,
    ElevatorHall,
    ElevatorInside,
    FloorCorridor,
    LinenRoom
};

struct GameState {
    GameScene currentScene;
    int currentFloor; // 0 for G, 6, 7
    QList<Task> tasks;
    Inventory inventory;
    bool hasClockedIn;
    bool hasReceivedTask; // New: Track if task has been claimed
    // Track state of dirty bag per floor to persist state
    // Key: Floor number, Value: isPresent
    QMap<int, bool> dirtyBagState; 
    QList<QString> logs;
};

// --- Helper Classes ---

class DraggableListWidget : public QListWidget {
    Q_OBJECT
public:
    DraggableListWidget(QWidget *parent = nullptr) : QListWidget(parent) {
        setDragEnabled(true);
    }

protected:
    void startDrag(Qt::DropActions supportedActions) override {
        QListWidgetItem *item = currentItem();
        if (item) {
            QMimeData *mimeData = new QMimeData;
            // Use the UserRole data where we stored the item type key
            mimeData->setText(item->data(Qt::UserRole).toString());

            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            
            // Generate a simple pixmap for dragging
            QPixmap pixmap(100, 30);
            pixmap.fill(Qt::lightGray);
            QPainter painter(&pixmap);
            painter.drawText(pixmap.rect(), Qt::AlignCenter, item->text().split(" ").first());
            drag->setPixmap(pixmap);

            drag->exec(Qt::MoveAction);
        }
    }
};

class DropLabel : public QLabel {
    Q_OBJECT
public:
    DropLabel(const QString &text, QWidget *parent = nullptr) : QLabel(text, parent) {
        setAcceptDrops(true);
        setAlignment(Qt::AlignCenter);
        // Modernized Drop Zone Style
        setStyleSheet(
            "border: 2px dashed #95a5a6;"
            "border-radius: 8px;"
            "background-color: #ecf0f1;"
            "color: #7f8c8d;"
            "font-weight: bold;"
            "font-size: 14px;"
        );
    }

    // Signal to notify the main window of a drop
    // We pass the mime data text (item name)
    std::function<void(QString)> onDropCallback;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasText()) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override {
        if (event->mimeData()->hasText()) {
            QString text = event->mimeData()->text();
            if (onDropCallback) {
                onDropCallback(text);
            }
            event->acceptProposedAction();
        }
    }
};

// --- MainWindow ---

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // --- Utils ---
    QPixmap generatePlaceholder(QString text, QColor color, QSize size);
    void logAction(QString action);
    void setupUI();
    void setupStyle(); // New method for applying styles

    // --- Module 1: Start Page ---
    QWidget *createStartPage();
    QLineEdit *nameInput;
    QSpinBox *ageInput;
    QComboBox *genderInput;
    QLineEdit *classInput;
    QLineEdit *durationInput;
    StudentInfo student;
    void onStartTraining();

    // --- Module 2: Slideshow ---
    QWidget *createSlideshowPage();
    QLabel *slideImageLabel;
    int currentSlideIndex = 0;
    const int totalSlides = 10;
    QWidget *slideshowContainer;
    QWidget *slideshowSummaryWidget; // Grid view
    void updateSlide();
    void finishSlideshow();

    // --- Module 3: Quiz ---
    QWidget *createQuizPage();
    struct Question {
        QString text;
        QStringList options;
        int correctIndex;
    };
    QList<Question> questions;
    int currentQuestionIndex = 0;
    int quizScore = 0;
    QLabel *questionLabel;

    // New Quiz UI elements
    QWidget *optionsContainer;
    QRadioButton *optionRadios[4];
    QPushButton *optionImages[4]; // Clickable thumbnails
    QButtonGroup *optionGroup;

    QLabel *scoreLabel;
    void loadQuestion();
    void checkAnswerAndNext(); // Combined logic
    void showImagePreview(QString imagePath); // Lightbox

    // --- Module 4: RPG Simulation ---
    QWidget *createRPGPage();
    
    // RPG UI Components
    QWidget *rpgCenterPanel;
    QLabel *locationLabel;
    QLabel *cartStatusLabel;
    QListWidget *taskListWidget;
    DraggableListWidget *inventoryListWidget; // Changed to custom class
    
    // Game State
    GameState gameState;
    
    // Scene Management
    void goToScene(GameScene scene);
    void renderScene(); // Clears center panel and draws elements based on currentScene
    
    // Logic Handlers
    void handleClockIn();
    void handleGetTask();
    void handleWarehouseItemClick(QString itemName);
    void handleLoadCart();
    void handleElevatorButton(int floor);
    void handleLinenDrop(QString itemName);
    void checkEmergencyTask();
    void handleClockOut();
    void updateRPGStatusLabels();
    void refreshInventoryList();
    void refreshTaskList();

    // Specific Scene Rendering Helpers
    void renderEntrance();
    void renderStaffHallway();
    void renderOffice();
    void renderWarehouse();
    void renderElevatorHall();
    void renderElevatorInside();
    void renderFloorCorridor();
    void renderLinenRoom();

    QStackedWidget *mainStack;
};

#endif // MAINWINDOW_H
