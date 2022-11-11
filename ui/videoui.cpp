#include "videoui.h"

VideoUi::VideoUi(QWidget* parent)
    : QWidget(parent)
    , bufferSizeSpinBox(new QSpinBox(this))
    , playButton(new QPushButton("Старт", this))
{
    auto tabLayout = new QFormLayout(this);

    videoWidget = new QWidget(this);
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    videoWidget->setUpdatesEnabled(false);

    tabLayout->addRow(videoWidget);
    tabLayout->addRow(new QLabel("Размер буффера приема (0-без лимита), КБ:", this), bufferSizeSpinBox);
    tabLayout->addRow(playButton);

    connect(playButton, &QPushButton::clicked,
        this, [&] {
            bool play = true;
            if (playButton->text() == QString("Старт")) {
                playButton->setText("Стоп");
            } else {
                playButton->setText("Старт");
                play = false;
            }
            emit setVideoParameters(videoWidget);
            emit startPlay(play);
        });

    bufferSizeSpinBox->setRange(0, std::numeric_limits<int32_t>::max());
    bufferSizeSpinBox->setValue(0);
    connect(bufferSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
        [=](int i) {
            emit setVideoBufferSize(i);
        });
}
