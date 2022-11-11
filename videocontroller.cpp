#include "videocontroller.h"

extern uint64_t maxQueueSize;

VideoController::VideoController(QObject* parent)
    : QObject { parent }
{
    maxQueueSize = 0;
}

VideoController::~VideoController()
{
    // if play enabled -> clear all threads before delete
    if (window)
        setPlay(false);
}

void VideoController::setVideoParameters(VideoWidget* videoWidget)
{
    //    videoWidget->setUpdatesEnabled(false);

    // fix? ??? no 1 enter
    //    static auto winId = videoWidget->winId();
    //    if (winId != videoWidget->winId()) {
    //        qDebug() << "adsfadsf";
    //        return;
    //    }

    if (!this->videoWidget)
        this->videoWidget = videoWidget;

    qDebug() << this->videoWidget->winId();

    //    if (!window)
    //        initSDL(this->videoWidget->winId());

    // window size for rescale rectangle
    //    if (this->is) {
    //        this->is->height        = videoWidget->height();
    //        this->is->width         = videoWidget->width();
    //        this->is->force_refresh = 1;
    //    }
}

void VideoController::setVideoBufferSize(const uint64_t& size)
{
    maxQueueSize = size * 1024;
}

void VideoController::setPlay(bool play)
{
    qDebug() << av_version_info();

    if (play) {
        videoThread = QThread::create([&] {
            avdevice_register_all();
            avformat_network_init();

            initSDL(this->videoWidget->winId());

            is = stream_open(this->path.data(), this->window, this->renderer);

            if (!is) {
                qDebug() << "error open stream";
                return;
            }
            is->height = videoWidget->height();
            is->width  = videoWidget->width();

            event_loop(is);
        });

        //        connect(videoThread, &QThread::finished, videoThread, &QThread::deleteLater);
        connect(videoThread, &QThread::finished, this, &VideoController::deleteStream);
        connect(this->thread(), &QThread::finished, videoThread, &QThread::deleteLater);

        videoThread->start();
    } else {
        // send event close stream
        streamClose();
    }
}

void VideoController::initSDL(WId winId)
{
    if (!SDL_getenv("SDL_AUDIO_ALSA_SET_BUFFER_SIZE"))
        SDL_setenv("SDL_AUDIO_ALSA_SET_BUFFER_SIZE", "1", 1);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
        throw std::runtime_error("SDL: could not init lib\n");

    SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
    SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

    //    window = SDL_CreateWindow("nullptr", 0, 0, 100, 100, SDL_WINDOW_RESIZABLE);
    window = SDL_CreateWindowFrom((void*)winId);
    if (!(window))
        throw std::runtime_error("SDL: could not create window\n");

    SDL_ShowWindow(window);

    //    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //SDL_RENDERER_ACCELERATED

    if (!renderer) {
        qDebug() << "failed initialize hardware accelerated renderer";
        renderer = SDL_CreateRenderer(window, -1, 0);
    }
    if (!renderer)
        throw std::runtime_error("SDL: could not create renderer\n");
}

void VideoController::streamClose()
{
    SDL_Event event;

    event.type       = SDL_QUIT;
    event.user.data1 = this->is;
    SDL_PushEvent(&event);
}

void VideoController::deleteStream()
{
    delete videoThread;
    videoThread = nullptr;

    is       = nullptr;
    window   = nullptr;
    renderer = nullptr;
}

void logHandle(void* userdata, int category, SDL_LogPriority priority, const char* message)
{
}
