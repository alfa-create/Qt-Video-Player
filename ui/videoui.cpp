#include "videoui.h"

VideoUi::VideoUi(QWidget* parent)
    : QWidget(parent)
    //    , codecComboBox(new QComboBox(this))
    , playButton(new QPushButton("Старт", this))
    , debugButton(new QPushButton("Debug", this))
    , debugWidget(nullptr)
{
    auto tabLayout = new QVBoxLayout(this);

    // ??
    //    auto codecFormLayout = new QFormLayout(nullptr);
    //    codecComboBox->addItem("h264");
    //    codecComboBox->addItem("h265");
    //    codecComboBox->setCurrentIndex(1);
    //    codecFormLayout->addRow(new QLabel("Кодек:", this), codecComboBox);
    //    tabLayout->addLayout(codecFormLayout);

    //    videoWidget = new QWidget(this);
    videoWidget = new VideoWidget(this);
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    tabLayout->addWidget(videoWidget);
    tabLayout->addWidget(playButton);

    tabLayout->addWidget(debugButton);

    connect(debugButton, &QPushButton::clicked,
        this, &VideoUi::createDebugUi);

    connect(playButton, &QPushButton::clicked,
        this, [&] {
            bool play = true;
            if (playButton->text() == QString("Старт")) {
                playButton->setText("Стоп");
            } else {
                playButton->setText("Старт");
                play = false;
            }
            emit setVideoParameters( videoWidget );
            emit startPlay(play);
        });

    connect(videoWidget, &VideoWidget::resized, this, [&] {
        emit setVideoParameters( videoWidget );
    });
}

void VideoUi::setProgress(int progress)
{
    sendProgressBar->show();
    sendProgressBar->setValue(progress);

    if (progress == 100) {
        QMessageBox messageBox;
        messageBox.information(nullptr, "Успех", "Отправка завершена");
        messageBox.setFixedSize(500, 200);
    }
}

void VideoUi::createDebugUi()
{
    debugWidget      = new QWidget(nullptr);
    auto formLayout  = new QFormLayout(debugWidget);
    sendButton       = new QPushButton("Записать на sd", this);
    filePathLineEdit = new QLineEdit(this);
    openButton       = new QPushButton("Открыть", this);
    recvButton       = new QPushButton("Начать излучение", this);
    sendProgressBar  = new QProgressBar(this);
    sendProgressBar->setRange(0, 100);
    sendProgressBar->setValue(0);
    sendProgressBar->hide();

    debugWidget->setAttribute(Qt::WA_DeleteOnClose, true);
    debugWidget->show();

    auto fileLayout = new QHBoxLayout(nullptr);
    fileLayout->addWidget(filePathLineEdit);
    fileLayout->addWidget(openButton);

    formLayout->addRow("Имя файла:", fileLayout);
    formLayout->addRow(sendButton, recvButton);
    formLayout->addRow(sendProgressBar);

    connect(openButton, &QPushButton::clicked, this,
        [&] {
            filePathLineEdit->setText(
                QFileDialog::getOpenFileName(this, "Выбрать файл для записи", "",
                    "All files (*.*);"));

            sendProgressBar->setValue(0);
            sendProgressBar->hide();
        });

    connect(sendButton, &QPushButton::clicked, this,
        [&] {
            if (!filePathLineEdit->text().isEmpty())
                emit sendFile(filePathLineEdit->text());
            sendProgressBar->setValue(0);
        });

    connect(recvButton, &QPushButton::clicked, this,
        [&] {
            if (!filePathLineEdit->text().isEmpty())
                emit recvFile(filePathLineEdit->text());
            sendProgressBar->setValue(0);
            sendProgressBar->hide();
        });
}
