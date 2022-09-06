#include "videocontroller.h"
#include "libswresample/swresample.h"

VideoController::VideoController(
    //    IDataHandler*      dataHandler,
    //    IModem::IListener* listener,
    QObject* parent)
    : QObject(parent)
    , videoParameters(new VideoParameters)
//    , audioMutex(new QMutex)
//    , IController("VideoController",
//          { /*ICommand(MType::common, command::common::uplink::FIRMWARE_K)*/ },
//          dataHandler, listener)
{
    try {
        // ffmpeg init
        avdevice_register_all();
        avformat_network_init();

        //        connect(this, &VideoController::packetReceived, this, &VideoController::decode, Qt::QueuedConnection);

    } catch (const std::runtime_error& err) {
        auto str = err.what();
        qDebug() << str;
    }
}

void audio_callback(void* userdata, Uint8* stream, int len)
{
    /*
     * callback from sdl audio
     * need write len bytes on stream
     * read size (min(len, data.size)) into stream
     * if data.size < len remaining bytes is silent(0)
    */
    auto pair = reinterpret_cast<std::pair<QByteArray*, QMutex*>*>(userdata);
    memset(stream, '0', len);

    pair->second->lock();

    auto audioSize = std::min(len, pair->first->size());
    //    memcpy(stream, pair->first->data(), audioSize);
    SDL_MixAudioFormat(stream, reinterpret_cast<uint8_t*>(pair->first->data()), AUDIO_S16SYS, audioSize, SDL_MIX_MAXVOLUME);
    pair->first->remove(0, audioSize);

    pair->second->unlock();
}

void VideoController::initSdl()
{
    /* from ffplay src
     * Try to work around an occasional ALSA buffer underflow issue when the
     * period size is NPOT due to ALSA resampling by forcing the buffer size. */
    if (!SDL_getenv("SDL_AUDIO_ALSA_SET_BUFFER_SIZE"))
        SDL_setenv("SDL_AUDIO_ALSA_SET_BUFFER_SIZE", "1", 1);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    videoParameters->screen = SDL_CreateWindowFrom((void*)videoParameters->widgetId);
    if (!videoParameters->screen) {
        throw std::runtime_error("SDL: could not create window\n");
    }
    SDL_ShowWindow(videoParameters->screen);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    videoParameters->renderer = SDL_CreateRenderer(videoParameters->screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!videoParameters->renderer) {
        qDebug() << "failed initialize hardware accelerated renderer";
        videoParameters->renderer = SDL_CreateRenderer(videoParameters->screen, -1, NULL);
    }
    if (!videoParameters->renderer)
        throw std::runtime_error("SDL: could not create renderer\n");

    //    texture = SDL_CreateTexture(renderer, )
    videoParameters->pFrameYUV = av_frame_alloc();
}

void VideoController::streamOpen()
{
    videoParameters->formatCtx = avformat_alloc_context();
    if (!videoParameters->formatCtx)
        throw std::runtime_error("FFMPEG: could not create avformat_alloc_context\n");

    //    formatCtx->interrupt_callback.callback

    if (avformat_open_input(&videoParameters->formatCtx, videoParameters->path.data(), NULL, NULL) < 0)
        throw std::runtime_error("FFMPEG: Couldn't open input stream");

    av_format_inject_global_side_data(videoParameters->formatCtx);

    if (avformat_find_stream_info(videoParameters->formatCtx, NULL) < 0)
        throw std::runtime_error("FFMPEG: Couldn't find stream information");

    if (videoParameters->formatCtx->pb)
        videoParameters->formatCtx->pb->eof_reached = 0;

    //    maxFrameDuration = (formatCtx->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;

    for (uint32_t i { 0 }; i < videoParameters->formatCtx->nb_streams; i++) {
        videoParameters->formatCtx->streams[i]->discard = AVDISCARD_ALL;
        switch (videoParameters->formatCtx->streams[i]->codecpar->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
            videoParameters->videoIdx = i;
            break;
        case AVMEDIA_TYPE_AUDIO:
            videoParameters->audioIdx = i;
            break;
        default:
            break;
        }
    }

    if (videoParameters->videoIdx == -1)
        qDebug() << "Couldn't find video stream";
    else
        streamComponentOpen(videoParameters->videoIdx);

    if (videoParameters->audioIdx == -1)
        qDebug() << "Couldn't find audio stream";
    else
        streamComponentOpen(videoParameters->audioIdx);

    //     here size ?
    if (videoParameters->videoIdx != -1) {
        videoParameters->imageConvertCtx = sws_getContext(
            videoParameters->videoCodecParams.first->width,
            videoParameters->videoCodecParams.first->height,
            videoParameters->videoCodecParams.first->pix_fmt,
            videoParameters->videoCodecParams.first->width,
            videoParameters->videoCodecParams.first->height,
            AV_PIX_FMT_YUV420P,
            SWS_BICUBIC, NULL, NULL, NULL);
        videoParameters->texture = SDL_CreateTexture(
            videoParameters->renderer,
            SDL_PIXELFORMAT_IYUV,
            SDL_TEXTUREACCESS_STREAMING,
            videoParameters->videoCodecParams.first->width,
            videoParameters->videoCodecParams.first->height);

        videoParameters->sdlRect.x = 0;
        videoParameters->sdlRect.y = 0;
        videoParameters->sdlRect.w = videoParameters->videoCodecParams.first->width;
        videoParameters->sdlRect.h = videoParameters->videoCodecParams.first->height;
    }

    //start receive thread
    recvThread = QThread::create([&] {
        while (true) {
            auto pkt = av_packet_alloc();
            if (av_read_frame(videoParameters->formatCtx, pkt) >= 0) {
                emit packetReceived(pkt);
                //                videoPacketQueue.pus hBack(pkt);
            } else
                av_packet_unref(pkt);
        }
    });
    connect(this->thread(), &QThread::finished, recvThread, &QThread::deleteLater);
    recvThread->start(QThread::HighestPriority);

    videoDecoder       = new VideoDecoder(videoParameters, nullptr);
    videoDecoderThread = new QThread();

    // TODO: connect
    connect(this, &VideoController::packetReceived, videoDecoder, &VideoDecoder::decode, Qt::QueuedConnection);
    videoDecoder->moveToThread(videoDecoderThread);
    videoDecoderThread->start(QThread::HighestPriority);
}

void VideoController::streamComponentOpen(int idx)
{
    auto codecCtx = avcodec_alloc_context3(NULL);
    if (!codecCtx)
        throw std::runtime_error("FFMPEG: Couldn't not avcodec_alloc_context3");

    if (avcodec_parameters_to_context(codecCtx, videoParameters->formatCtx->streams[idx]->codecpar) < 0)
        throw std::runtime_error("FFMPEG: Couldn't not avcodec_parameters_to_context");

    codecCtx->pkt_timebase = videoParameters->formatCtx->streams[idx]->time_base;
    auto codec             = avcodec_find_decoder(codecCtx->codec_id);

    if (avcodec_open2(codecCtx, codec, NULL) < 0)
        throw std::runtime_error("FFMPEG: Couldn't not avcodec_open2");

    // ?
    codecCtx->codec_id = codec->id;

    videoParameters->formatCtx->streams[idx]->discard = AVDISCARD_DEFAULT;

    // TODO create decode thread
    switch (codecCtx->codec_type) {
    case AVMEDIA_TYPE_AUDIO: {
        //        codecCtx->request_sample_fmt = AV_SAMPLE_FMT_S16;
        SDL_AudioSpec spec;

        //        auto            sampleRate = codecCtx->sample_rate;
        if (av_channel_layout_copy(&videoParameters->chLayout, &codecCtx->ch_layout) < 0)
            throw std::runtime_error("FFMPEG: Couldn't not av_channel_layout_copy");

        videoParameters->wanted_spec.freq     = codecCtx->sample_rate;
        videoParameters->wanted_spec.format   = AUDIO_S16SYS;
        videoParameters->wanted_spec.channels = videoParameters->chLayout.nb_channels;
        videoParameters->wanted_spec.silence  = 0;
        videoParameters->wanted_spec.samples  = 1024;
        videoParameters->wanted_spec.callback = audio_callback;

        auto pair                             = new std::pair<QByteArray*, QMutex*>(&videoParameters->audioArr, videoParameters->audioMutex);
        videoParameters->wanted_spec.userdata = (void*)pair;

        if (SDL_OpenAudio(&videoParameters->wanted_spec, &spec) < 0)
            throw std::runtime_error("SDL: Couldn't not SDL_OpenAudio");

        av_channel_layout_default(&videoParameters->chLayout, videoParameters->wanted_spec.channels);

        SDL_PauseAudio(0);

        videoParameters->audioCodecParams.first  = codecCtx;
        videoParameters->audioCodecParams.second = codec;
        //        av_channel_layout_uninit(&chLayout);
        break;
    }
    case AVMEDIA_TYPE_VIDEO:
        videoParameters->videoCodecParams.first  = codecCtx;
        videoParameters->videoCodecParams.second = codec;

        break;

    default:
        break;
    }
}

void VideoController::synchronize_video(AVFrame* frame, double* pts)
{
    //    if (*pts != 0)
    //        clocks.first = *pts;
    //    else
    //        *pts = clocks.first;

    //    double frameDelay = av_q2d(videoCodecParams.first->time_base);
    //    frameDelay += frame->repeat_pict * (frameDelay * 0.5);
    //    clocks.first += frameDelay;
}

//void VideoController::handle(const ICommand* /*command*/, const QByteArray& /*body*/)
//{
//}

void VideoController::setPlay(bool play)
{
    qDebug() << av_version_info();

    static bool isInit = false;
    if (!isInit)
        initSdl();
    isInit = true;

    streamOpen();
}
void VideoController::setVideoParameters(WId widgetId /*const IModem::VideoData& videoData*/)
{
    //    this->widgetId                  = widgetId;
    this->videoParameters->widgetId = widgetId;
    //    this->widgetId = videoData.videoWidget->winId();

    //    auto size = videoData.videoWidget->size();

    //    this->presentRect.h = size.height();
    //    this->presentRect.w = size.width();
}
