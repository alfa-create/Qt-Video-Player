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

class VideoUi : public QWidget {
    Q_OBJECT
public:
    explicit VideoUi(QWidget* parent = nullptr);

signals:
    void startPlay(bool start);
    void setVideoParameters(/*const IModem::VideoData& videoData*/ QWidget* videoWidget);
    void setVideoBufferSize(const uint64_t& size);

private:
    QWidget*   videoWidget { nullptr };
    QLineEdit* filePathLineEdit { nullptr };
    QSpinBox*  bufferSizeSpinBox { nullptr };

    QPushButton* playButton { nullptr };
};

#endif // VIDEOUI_H
