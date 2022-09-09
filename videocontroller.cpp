#include "videocontroller.h"

VideoController::VideoController(QObject* parent)
    : QObject { parent }
{
    avdevice_register_all();
    avformat_network_init();
}

void VideoController::setVideoParameters(QWidget* videoWidget)
{
    if (!window)
        initSDL(videoWidget->winId());

    if (is) {
        is->height = videoWidget->height();
        is->width  = videoWidget->width();
    }

    // TODO: present rectangle
}

void VideoController::setPlay(bool play)
{
    qDebug() << av_version_info();

    if (play) {

        auto thread = QThread::create([&] {
            //            streamOpen();
            //            init_sdl(this->window, this->renderer);
            //            eventLoop();
            is = stream_open(path.data(), this->window, this->renderer);

            if (!is)
                int a = 1234;
            event_loop(is);
        });
        thread->start();

        //        streamOpen();

        //        eventLoop();
        // open stream + init ffmpeg part
        // event loop
        // start this in new thread?

    } else {
        // close stream + delete ffmpeg struct's
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

    if (!(window = SDL_CreateWindowFrom((void*)winId)))
        throw std::runtime_error("SDL: could not create window\n");

    SDL_ShowWindow(window);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        qDebug() << "failed initialize hardware accelerated renderer";
        renderer = SDL_CreateRenderer(window, -1, 0);
    }
    if (!renderer)
        throw std::runtime_error("SDL: could not create renderer\n");
}

void VideoController::streamOpen()
{
    //    this->is = stream_open(path.data(), nullptr;

    //    try {
    //        is = new VideoState;
    //        memset((void*)is, 0, sizeof(VideoState));
    //        if (!is)
    //            throw std::runtime_error("streamOpen: could not create is\n");

    //        is->last_video_stream = is->video_stream = -1;
    //        is->last_audio_stream = is->audio_stream = -1;
    //        is->last_subtitle_stream = is->subtitle_stream = -1;
    //        is->filename                                   = path.data();
    //        is->iformat                                    = nullptr;
    //        is->ytop                                       = 0;
    //        is->xleft                                      = 0;
    //        is->audio_dev                                  = 0;
    //        is->window                                     = this->window;
    //        is->renderer                                   = this->renderer;

    //        if (is->videoq.init() < 0)
    //            throw std::runtime_error("ERROR: is->videoq.init");

    //        if (is->audioq.init() < 0)
    //            throw std::runtime_error("ERROR: is->audioq.init");

    //        if (is->subtitleq.init() < 0)
    //            throw std::runtime_error("ERROR: is->subtitleq.init");

    //        if (is->pictq.init(&is->videoq, VIDEO_PICTURE_QUEUE_SIZE, 1) < 0)
    //            throw std::runtime_error("ERROR: is->pictq.init");

    //        if (is->subpq.init(&is->subtitleq, SUBPICTURE_QUEUE_SIZE, 0) < 0)
    //            throw std::runtime_error("ERROR: is->subpq.init");

    //        if (is->sampq.init(&is->audioq, SAMPLE_QUEUE_SIZE, 1) < 0)
    //            throw std::runtime_error("ERROR: is->sampq.init");

    //        if (!(is->continue_read_thread = SDL_CreateCond()))
    //            throw std::runtime_error("ERROR: SDL_CreateCond continue_read_thread");

    //        init_clock(&is->vidclk, is->videoq.getSerialPtr());
    //        init_clock(&is->audclk, is->audioq.getSerialPtr());
    //        init_clock(&is->extclk, &is->extclk.serial);

    //        is->audio_clock_serial = -1;
    //        is->audio_volume       = SDL_MIX_MAXVOLUME;
    //        is->muted              = 0;
    //        // TODO: here swap?
    //        is->av_sync_type = avSyncType;
    //        is->read_tid     = SDL_CreateThread(read_thread, "read", is);
    //        if (!is->read_tid)
    //            throw std::runtime_error("ERROR: SDL_CreateThread read_thread");

    //    } catch (std::exception& ex) {
    //        auto str = ex.what();
    //        qDebug() << str;
    //        streamClose();
    //    } catch (...) {
    //        auto str = "unknown error";
    //        qDebug() << str;
    //        streamClose();
    //    }
}

void VideoController::streamClose()
{
    //    is->abort_request = 1;
    //    SDL_WaitThread(is->read_tid, NULL);

    //    /* close each stream */
    //    if (is->audio_stream >= 0)
    //        streamComponentClose(is->audio_stream);
    //    if (is->video_stream >= 0)
    //        streamComponentClose(is->video_stream);
    //    if (is->subtitle_stream >= 0)
    //        streamComponentClose(is->subtitle_stream);

    //    avformat_close_input(&is->ic);

    //    is->videoq.destroy();
    //    is->audioq.destroy();
    //    is->subtitleq.destroy();

    //    /* free all pictures */
    //    is->pictq.destroy();
    //    is->sampq.destroy();
    //    is->subpq.destroy();

    //    SDL_DestroyCond(is->continue_read_thread);
    //    sws_freeContext(is->img_convert_ctx);
    //    sws_freeContext(is->sub_convert_ctx);
    //    //    av_free(is->filename);
    //    if (is->vis_texture)
    //        SDL_DestroyTexture(is->vis_texture);
    //    if (is->vid_texture)
    //        SDL_DestroyTexture(is->vid_texture);
    //    if (is->sub_texture)
    //        SDL_DestroyTexture(is->sub_texture);
    //    //    av_free(is);
    //    delete is;
}

void VideoController::streamComponentClose(int idx)
{
    //    AVFormatContext*   ic = is->ic;
    //    AVCodecParameters* codecpar;

    //    if (idx < 0 || idx >= ic->nb_streams)
    //        return;
    //    codecpar = ic->streams[idx]->codecpar;

    //    switch (codecpar->codec_type) {
    //    case AVMEDIA_TYPE_AUDIO:
    //        is->auddec.abort(&is->sampq);
    //        SDL_CloseAudioDevice(is->audio_dev);
    //        is->auddec.destroy();
    //        swr_free(&is->swr_ctx);
    //        av_freep(&is->audio_buf1);
    //        is->audio_buf1_size = 0;
    //        is->audio_buf       = NULL;

    //        if (is->rdft) {
    //            av_rdft_end(is->rdft);
    //            av_freep(&is->rdft_data);
    //            is->rdft      = NULL;
    //            is->rdft_bits = 0;
    //        }
    //        break;
    //    case AVMEDIA_TYPE_VIDEO:
    //        is->viddec.abort(&is->pictq);
    //        is->viddec.destroy();
    //        break;
    //    case AVMEDIA_TYPE_SUBTITLE:
    //        is->subdec.abort(&is->subpq);
    //        is->subdec.destroy();
    //        break;
    //    default:
    //        break;
    //    }

    //    ic->streams[idx]->discard = AVDISCARD_ALL;
    //    switch (codecpar->codec_type) {
    //    case AVMEDIA_TYPE_AUDIO:
    //        is->audio_st     = NULL;
    //        is->audio_stream = -1;
    //        break;
    //    case AVMEDIA_TYPE_VIDEO:
    //        is->video_st     = NULL;
    //        is->video_stream = -1;
    //        break;
    //    case AVMEDIA_TYPE_SUBTITLE:
    //        is->subtitle_st     = NULL;
    //        is->subtitle_stream = -1;
    //        break;
    //    default:
    //        break;
    //    }
}

void VideoController::eventLoop()
{
    //    SDL_Event event;
    //    double    incr, pos, frac;

    //    for (;;) {
    //        double x;
    //        refresh_loop_wait_event(this->is, &event);
    //        switch (event.type) {
    //        case SDL_QUIT:
    //        case FF_QUIT_EVENT:
    //            doExit();
    //            break;
    //        default:
    //            break;
    //        }
    //    }
}

void VideoController::doExit()
{
    streamClose();
    //    if (is) {
    //        stream_close(is);
    //    }
    //    if (renderer)
    //        SDL_DestroyRenderer(renderer);
    //    if (window)
    //        SDL_DestroyWindow(window);
    //    //    uninit_opts();
    avformat_network_deinit();
    //    if (show_status)
    //        printf("\n");
    //    SDL_Quit();
    //    av_log(NULL, AV_LOG_QUIET, "%s", "");
    //    exit(0);
}
