#ifndef TUTORIAL_OVERLAY_H
#define TUTORIAL_OVERLAY_H

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVBoxLayout>

class TutorialOverlay : public QWidget {
    Q_OBJECT
public:
    explicit TutorialOverlay(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;
};

#endif // TUTORIAL_OVERLAY_H
