#ifndef VIDEOUI_H
#define VIDEOUI_H

#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPair>
#include <QProgressBar>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

//#include "imodem.h"
#include "videowidget.h"

class VideoUi : public QWidget {
    Q_OBJECT
public:
    explicit VideoUi(QWidget* parent = nullptr);

public slots:
    void setProgress(int progress);

signals:
    void sendFile(const QString& path);
    void recvFile(const QString& path);

    //    void setCodec(int idx);
    void startPlay(bool start);
    void setVideoParameters(/*const IModem::VideoData& videoData*/ QWidget* videoWidget);

private:
    // debug widget
    VideoWidget*  videoWidget;
    QLineEdit*    filePathLineEdit;
    QPushButton*  sendButton;
    QPushButton*  openButton;
    QPushButton*  recvButton;
    QProgressBar* sendProgressBar;

    //    QComboBox*   codecComboBox;
    QPushButton* playButton;
    QPushButton* debugButton;
    QWidget*     debugWidget;

private slots:
    void createDebugUi();
};

#endif // VIDEOUI_H
