#include "tutorial_overlay.h"

TutorialOverlay::TutorialOverlay(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_DeleteOnClose);
}

void TutorialOverlay::mousePressEvent(QMouseEvent *event) {
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

void TutorialOverlay::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void TutorialOverlay::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    setFocus();
}
