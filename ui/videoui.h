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
#include <QSpinBox>
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
    void setVideoParameters(/*const IModem::VideoData& videoData*/ VideoWidget* videoWidget);
    void setVideoBufferSize(const uint64_t& size);

private:
    // debug widget
    VideoWidget*  videoWidget { nullptr };
    QLineEdit*    filePathLineEdit { nullptr };
    QPushButton*  sendButton { nullptr };
    QPushButton*  openButton { nullptr };
    QPushButton*  recvButton { nullptr };
    QProgressBar* sendProgressBar { nullptr };

    QSpinBox* bufferSizeSpinBox { nullptr };

    //    QComboBox*   codecComboBox;
    QPushButton* playButton { nullptr };
    QPushButton* debugButton { nullptr };
    QWidget*     debugWidget { nullptr };

private slots:
    void createDebugUi();
};

#endif // VIDEOUI_H
