#include "video_tutorial_dialog.h"
#include <QScreen>
#include <QApplication>
#include <QStyle>
#include <QFile>
#include <QUrl>

VideoTutorialDialog::VideoTutorialDialog(const QString &videoPath, const QString &text, QWidget *parent)
    : QDialog(parent), m_videoPath(videoPath)
{
    setWindowTitle("操作演示");
    // Dialog flags: Modal window
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

    // Layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Text Label (Top)
    m_textLabel = new QLabel(text, this);
    m_textLabel->setWordWrap(true);
    m_textLabel->setAlignment(Qt::AlignCenter);
    // Use a reasonable font size
    m_textLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 20px; color: #333;");
    layout->addWidget(m_textLabel);

    // Video Area (Bottom)
    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_videoWidget);

    // Player Setup
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setVideoOutput(m_videoWidget);

    // Set Source
    // Note: QMediaPlayer usually requires absolute path or URL.
    // Resource paths (:/...) don't work directly with all backends of QMediaPlayer.
    // We try qrc scheme first. If not supported by backend, we might need a temp file.
    // For now, standard Qt6 ffmpeg backend handles qrc usually or requires copy.
    // Safe approach: if it starts with :, copy to temp.

    QString finalPath = m_videoPath;
    if (finalPath.startsWith(":/")) {
        // Simple hack: Assume it works or copy if needed.
        // QMediaPlayer support for qrc is tricky.
        // Let's try passing "qrc:..." url.
        QString qrcUrl = "qrc" + finalPath.mid(1);
        m_player->setSource(QUrl(qrcUrl));
    } else {
        m_player->setSource(QUrl::fromLocalFile(finalPath));
    }

    m_player->setLoops(QMediaPlayer::Infinite);
    m_audioOutput->setVolume(0.5);
}

VideoTutorialDialog::~VideoTutorialDialog()
{
    if (m_player) {
        m_player->stop();
    }
}

void VideoTutorialDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    // Resize to 80% of parent window if possible
    if (parentWidget()) {
        QSize parentSize = parentWidget()->size();
        resize(parentSize.width() * 0.8, parentSize.height() * 0.8);

        // Center on parent
        move(parentWidget()->geometry().center() - rect().center());
    } else {
        // Fallback to screen size
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QSize screenSize = screen->availableGeometry().size();
            resize(screenSize.width() * 0.8, screenSize.height() * 0.8);
        }
    }

    m_player->play();
}

void VideoTutorialDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    // Layout automatically handles children resizing
}
