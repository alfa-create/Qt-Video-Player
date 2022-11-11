#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , packetMutex(new QMutex())
{
    ui->setupUi(this);

    auto groupBox = new QGroupBox(this);
    auto layout   = new QHBoxLayout(groupBox);
    auto widget   = new VideoUi(groupBox);

    layout->addWidget(widget);
    this->setCentralWidget(groupBox);

    auto controller = new VideoController(nullptr);

    connect(widget, &VideoUi::startPlay, controller, &VideoController::setPlay);
    connect(widget, &VideoUi::setVideoParameters, controller, &VideoController::setVideoParameters);
    connect(widget, &VideoUi::setVideoBufferSize, controller, &VideoController::setVideoBufferSize);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPlay(bool play)
{
}

void MainWindow::setVideoParameters(QWidget* videoWidget)
{
    videoWidget->resize(QSize(704, 576));
    this->widgetId = videoWidget->winId();
    auto size      = videoWidget->size();

    this->presentRect.h = size.height();
    this->presentRect.w = size.width();
}
