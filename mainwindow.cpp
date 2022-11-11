#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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
