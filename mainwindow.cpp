#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , packetMutex(new QMutex())
{
    ui->setupUi(this);

    //    avformat_network_init();
    //    pFormatCtx = avformat_alloc_context();

    //    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    //    setVideoParameters(ui->widget);

    //    setPlay(true);

    //    auto widget = new VideoUi(this);
    //    this->setCentralWidget(widget);

    auto controller = new VideoController(this);
    controller->setVideoParameters(ui->widget);
    controller->setPlay(true);

    //    connect(widget, &VideoUi::startPlay, controller, &VideoController::setPlay);
    //    connect(widget, &VideoUi::setVideoParameters, controller, &VideoController::setVideoParameters);
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
