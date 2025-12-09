#ifndef VIDEO_TUTORIAL_DIALOG_H
#define VIDEO_TUTORIAL_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>

class VideoTutorialDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VideoTutorialDialog(const QString &videoPath, const QString &text, QWidget *parent = nullptr);
    ~VideoTutorialDialog();

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QString m_videoPath;
    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;
    QAudioOutput *m_audioOutput;
    QLabel *m_textLabel;
};

#endif // VIDEO_TUTORIAL_DIALOG_H
