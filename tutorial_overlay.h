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
    explicit TutorialOverlay(QWidget *parent = nullptr) : QWidget(parent) {
        setFocusPolicy(Qt::StrongFocus);
        setAttribute(Qt::WA_DeleteOnClose);
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        // Right click closes the overlay
        if (event->button() == Qt::RightButton) {
            close();
            return;
        }

        // Left click
        if (event->button() == Qt::LeftButton) {
            // childAt returns the visible child widget at the position.
            // If it returns nullptr, we clicked on the overlay itself.
            // If it returns a widget, it means we clicked on the content box or its children.
            if (childAt(event->pos()) == nullptr) {
                close();
            }
            // If clicked on content, do nothing (absorb the click essentially)
        }

        QWidget::mousePressEvent(event);
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Escape) {
            close();
        } else {
            QWidget::keyPressEvent(event);
        }
    }

    // Ensure we grab focus when shown
    void showEvent(QShowEvent *event) override {
        QWidget::showEvent(event);
        setFocus();
    }
};

#endif // TUTORIAL_OVERLAY_H
