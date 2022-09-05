#include "videocontroller.h"
#include "libswresample/swresample.h"

VideoController::VideoController(
    //    IDataHandler*      dataHandler,
    //    IModem::IListener* listener,
    QObject* parent)
    : QObject(parent)
    , audioMutex(new QMutex)
//    , IController("VideoController",
//          { /*ICommand(MType::common, command::common::uplink::FIRMWARE_K)*/ },
//          dataHandler, listener)
{
    try {
        // ffmpeg init
        avdevice_register_all();
        avformat_network_init();

        connect(this, &VideoController::packetReceived, this, &VideoController::decode, Qt::QueuedConnection);

    } catch (const std::runtime_error& err) {
        auto str = err.what();
        qDebug() << str;
    }

    //    pFormatCtx = avformat_alloc_context();
}

void audio_callback(void* userdata, Uint8* stream, int len)
{

    auto pair = reinterpret_cast<std::pair<QByteArray*, QMutex*>*>(userdata);
    pair->second->lock();

    auto audioSize = std::min(len, pair->first->size());
    memset(stream, '0', len);

    if (audioSize < len) {
        audioSize = len;
        memset(stream, '0', len);
        //        SDL_MixAudioFormat(stream, stream, AUDIO_S16SYS, audioSize, 5);
    } else {
        memcpy(stream, pair->first->data(), audioSize);
        pair->first->remove(0, audioSize);
        //        SDL_MixAudioFormat(stream, (uint8_t*)pair->first->data(), AUDIO_S16SYS, audioSize, 12);
        //        SDL_MixAudio(stream, (uint8_t*)pair->first->data(), audioSize, 5);
        //        pair->first->remove(0, audioSize);
    }
    //    qDebug() << "write audio " << std::min(len, pair->first->size()) << pair->first->size();
    //    SDL_MixAudioFormat(stream, (uint8_t*)pair->first->data(), AUDIO_S16SYS, std::min(len, pair->first->size()), 12);
    //    SDL_MixAudio(stream, (uint8_t*)pair->first->data(), NULL, NULL);

    pair->second->unlock();

    //    AVCodecContext* aCodecCtx = (AVCodecContext*)userdata;
    //    int             len1, audio_size;

    //    static uint8_t      audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
    //    static unsigned int audio_buf_size  = 0;
    //    static unsigned int audio_buf_index = 0;

    //    while (len > 0) {
    //        if (audio_buf_index >= audio_buf_size) {
    //            /* We have already sent all our data; get more */
    //            audio_size = audio_decode_frame(aCodecCtx, audio_buf, sizeof(audio_buf));
    //            if (audio_size < 0) {
    //                /* If error, output silence */
    //                audio_buf_size = 1024; // arbitrary?
    //                memset(audio_buf, 0, audio_buf_size);
    //            } else {
    //                audio_buf_size = audio_size;
    //            }
    //            audio_buf_index = 0;
    //        }
    //        len1 = audio_buf_size - audio_buf_index;
    //        if (len1 > len)
    //            len1 = len;
    //        memcpy(stream, (uint8_t*)audio_buf + audio_buf_index, len1);
    //        len -= len1;
    //        stream += len1;
    //        audio_buf_index += len1;
    //    }
}

void VideoController::initSdl()
{
    /* from ffplay src
     * Try to work around an occasional ALSA buffer underflow issue when the
     * period size is NPOT due to ALSA resampling by forcing the buffer size. */
    //    if (!SDL_getenv("SDL_AUDIO_ALSA_SET_BUFFER_SIZE"))
    //        SDL_setenv("SDL_AUDIO_ALSA_SET_BUFFER_SIZE", "1", 1);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    screen = SDL_CreateWindowFrom((void*)widgetId);
    if (!screen) {
        throw std::runtime_error("SDL: could not create window\n");
    }
    SDL_ShowWindow(screen);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    //    renderer = SDL_CreateRenderer(screen, -1, NULL);
    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        qDebug() << "failed initialize hardware accelerated renderer";
        renderer = SDL_CreateRenderer(screen, -1, NULL);
    }
    if (!renderer)
        throw std::runtime_error("SDL: could not create renderer\n");

    //    texture = SDL_CreateTexture(renderer, )
    pFrameYUV = av_frame_alloc();
}

void VideoController::streamOpen()
{
    formatCtx = avformat_alloc_context();
    if (!formatCtx)
        throw std::runtime_error("FFMPEG: could not create avformat_alloc_context\n");

    //    formatCtx->interrupt_callback.callback

    if (avformat_open_input(&formatCtx, path.data(), NULL, NULL) < 0)
        throw std::runtime_error("FFMPEG: Couldn't open input stream");

    av_format_inject_global_side_data(formatCtx);

    if (avformat_find_stream_info(formatCtx, NULL) < 0)
        throw std::runtime_error("FFMPEG: Couldn't find stream information");

    if (formatCtx->pb)
        formatCtx->pb->eof_reached = 0;

    maxFrameDuration = (formatCtx->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;

    for (uint32_t i { 0 }; i < formatCtx->nb_streams; i++) {
        formatCtx->streams[i]->discard = AVDISCARD_ALL;
        switch (formatCtx->streams[i]->codecpar->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
            videoIdx = i;
            break;
        case AVMEDIA_TYPE_AUDIO:
            audioIdx = i;
            break;
        default:
            break;
        }
    }

    if (videoIdx == -1)
        qDebug() << "Couldn't find video stream";
    else
        streamComponentOpen(videoIdx);

    if (audioIdx == -1)
        qDebug() << "Couldn't find audio stream";
    else
        streamComponentOpen(audioIdx);

    //     here size ?
    if (videoIdx != -1) {
        imageConvertCtx = sws_getContext(videoCodecParams.first->width, videoCodecParams.first->height, videoCodecParams.first->pix_fmt,
            videoCodecParams.first->width, videoCodecParams.first->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
        texture         = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, videoCodecParams.first->width, videoCodecParams.first->height);
        sdlRect.x       = 0;
        sdlRect.y       = 0;
        sdlRect.w       = videoCodecParams.first->width;
        sdlRect.h       = videoCodecParams.first->height;
    }

    //start receive thread
    recvThread = QThread::create([&] {
        while (true) {
            auto pkt = av_packet_alloc();
            if (av_read_frame(formatCtx, pkt) >= 0) {
                emit packetReceived(pkt);
                //                videoPacketQueue.pus hBack(pkt);
            } else
                av_packet_unref(pkt);
        }
    });
    connect(this->thread(), &QThread::finished, recvThread, &QThread::deleteLater);
    recvThread->start(QThread::HighestPriority);

    //    videoDecodeThread = QThread::create([&] {
    //        while (true) {

    //            if (videoPacketQueue.size()) {
    //                auto pkt = videoPacketQueue.popFront();
    //                qDebug() << videoPacketQueue.size();

    //                auto ret = avcodec_send_packet(videoCodecParams.first, pkt);
    //                if (ret < 0)
    //                    qDebug() << "Decode Error 1";

    //                auto frame = av_frame_alloc();
    //                ret        = avcodec_receive_frame(videoCodecParams.first, frame);
    //                if (ret < 0)
    //                    qDebug() << "Decode Error 2";

    //                double pts = frame->pkt_dts;
    //                if (pts == AV_NOPTS_VALUE)
    //                    pts = frame->pkt_dts;
    //                if (pts == AV_NOPTS_VALUE)
    //                    pts = 0;
    //                pts *= av_q2d(formatCtx->streams[videoIdx]->time_base);
    //                synchronize_video(frame, &pts);

    //                while (pictureReady)
    //                    ;
    //                picturePts = pts;

    //                out_buffer = (unsigned char*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, videoCodecParams.first->width, videoCodecParams.first->height, 1));
    //                av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
    //                    AV_PIX_FMT_YUV420P, videoCodecParams.first->width, videoCodecParams.first->height, 1);

    //                sws_scale(imageConvertCtx, (const unsigned char* const*)frame->data, frame->linesize, 0, videoCodecParams.first->height,
    //                    pFrameYUV->data, pFrameYUV->linesize);

    //                SDL_UpdateYUVTexture(texture, &sdlRect,
    //                    pFrameYUV->data[0], pFrameYUV->linesize[0],
    //                    pFrameYUV->data[1], pFrameYUV->linesize[1],
    //                    pFrameYUV->data[2], pFrameYUV->linesize[2]);

    //                SDL_RenderClear(renderer);
    //                //                SDL_RenderCopy(renderer, texture, NULL, &presentRect);
    //                SDL_RenderCopy(renderer, texture, NULL, &sdlRect);

    //                SDL_RenderPresent(renderer);
    //            }
    //        }
    //    });
    //    connect(this->thread(), &QThread::finished, videoDecodeThread, &QThread::deleteLater);
    //    videoDecodeThread->start(QThread::HighestPriority);
}

void VideoController::streamComponentOpen(int idx)
{
    auto codecCtx = avcodec_alloc_context3(NULL);
    if (!codecCtx)
        throw std::runtime_error("FFMPEG: Couldn't not avcodec_alloc_context3");

    if (avcodec_parameters_to_context(codecCtx, formatCtx->streams[idx]->codecpar) < 0)
        throw std::runtime_error("FFMPEG: Couldn't not avcodec_parameters_to_context");

    codecCtx->pkt_timebase = formatCtx->streams[idx]->time_base;
    auto codec             = avcodec_find_decoder(codecCtx->codec_id);

    if (avcodec_open2(codecCtx, codec, NULL) < 0)
        throw std::runtime_error("FFMPEG: Couldn't not avcodec_open2");

    // ?
    codecCtx->codec_id = codec->id;

    formatCtx->streams[idx]->discard = AVDISCARD_DEFAULT;

    // TODO create decode thread
    switch (codecCtx->codec_type) {
    case AVMEDIA_TYPE_AUDIO: {
        //        codecCtx->request_sample_fmt = AV_SAMPLE_FMT_S16;
        SDL_AudioSpec spec;

        //        auto            sampleRate = codecCtx->sample_rate;
        if (av_channel_layout_copy(&chLayout, &codecCtx->ch_layout) < 0)
            throw std::runtime_error("FFMPEG: Couldn't not av_channel_layout_copy");

        wanted_spec.freq     = codecCtx->sample_rate;
        wanted_spec.format   = AUDIO_S16SYS;
        wanted_spec.channels = chLayout.nb_channels;
        wanted_spec.silence  = 0;
        wanted_spec.samples  = 8192;
        wanted_spec.callback = audio_callback;

        auto pair    = new std::pair<QByteArray*, QMutex*>;
        pair->first  = &audioArr;
        pair->second = audioMutex;

        //        auto pair            = std::make_pair(&this->audioArr, &this->audioMutex);
        wanted_spec.userdata = (void*)pair;
        //        auto dev             = SDL_OpenAudioDevice(NULL, 0, &wanted_spec,
        //            &spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
        if (SDL_OpenAudio(&wanted_spec, &spec) /*dev */ < 0)
            throw std::runtime_error("SDL: Couldn't not SDL_OpenAudio");

        av_channel_layout_default(&chLayout, wanted_spec.channels);

        SDL_PauseAudio(0);
        //        SDL_PauseAudioDevice(dev, 0);
        //        SDL_

        /*
        if ((ret = audio_open(is, &ch_layout, sample_rate, &is->audio_tgt)) < 0)
            goto fail;
        is->audio_hw_buf_size = ret;
        is->audio_src = is->audio_tgt;
        is->audio_buf_size = 0;
        is->audio_buf_index = 0;

        is->audio_diff_avg_coef = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
        is->audio_diff_avg_count = 0;

        is->audio_diff_threshold = (double)(is->audio_hw_buf_size) / is->audio_tgt.bytes_per_sec;

        is->audio_stream = stream_index;
        is->audio_st = ic->streams[stream_index];

        if ((ret = decoder_init(&is->auddec, avctx, &is->audioq, is->continue_read_thread)) < 0)
            goto fail;
        if ((is->ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !is->ic->iformat->read_seek) {
            is->auddec.start_pts = is->audio_st->start_time;
            is->auddec.start_pts_tb = is->audio_st->time_base;
        }
        if ((ret = decoder_start(&is->auddec, audio_thread, "audio_decoder", is)) < 0)
            goto out;
        SDL_PauseAudioDevice(audio_dev, 0);
        */
        audioCodecParams.first  = codecCtx;
        audioCodecParams.second = codec;
        //        av_channel_layout_uninit(&chLayout);
        break;
    }
    case AVMEDIA_TYPE_VIDEO:
        videoCodecParams.first  = codecCtx;
        videoCodecParams.second = codec;

        break;

    default:
        break;
    }
}

void VideoController::synchronize_video(AVFrame* frame, double* pts)
{
    if (*pts != 0)
        clocks.first = *pts;
    else
        *pts = clocks.first;

    double frameDelay = av_q2d(videoCodecParams.first->time_base);
    frameDelay += frame->repeat_pict * (frameDelay * 0.5);
    clocks.first += frameDelay;
}

void VideoController::decode(AVPacket* pkt)
{
    if (pkt->stream_index == audioIdx) {
        auto ret = avcodec_send_packet(audioCodecParams.first, pkt);
        if (ret < 0)
            qDebug() << "Decode Error 1";

        while (true) {
            auto frame = av_frame_alloc();
            ret        = avcodec_receive_frame(audioCodecParams.first, frame);
            auto a     = 0;
            if (ret == AVERROR_EOF)
                a = 1;
            else if (ret == AVERROR(EAGAIN)) {
                a = 2;
            } else if (ret == AVERROR(EINVAL))
                a = 3;

            if (ret < 0) {
                qDebug() << "Decode Error 2"
                         << "audio" << ((pkt->stream_index == audioIdx) ? "1" : "0") << a;
                return;
            }

            auto dataSize = av_samples_get_buffer_size(
                NULL, frame->ch_layout.nb_channels, frame->nb_samples,
                AVSampleFormat(frame->format), 1);

            if (frame->format != AV_SAMPLE_FMT_S16
                || av_channel_layout_compare(&frame->ch_layout, &chLayout)) {

                if (swr_ctx)
                    swr_free(&swr_ctx);

                swr_alloc_set_opts2(&swr_ctx,
                    &chLayout,
                    AV_SAMPLE_FMT_S16,
                    wanted_spec.freq,
                    &frame->ch_layout,
                    (AVSampleFormat)(frame->format),
                    frame->sample_rate,
                    0, NULL);

                if (!swr_ctx || swr_init(swr_ctx) < 0)
                    qDebug() << "error swr_ctx";

                av_channel_layout_copy(&chLayout, &frame->ch_layout);
                //                wanted_spec.freq = frame->sample_rate;

                qDebug() << "asdf";
            }

            // refactor
            const uint8_t** in = (const uint8_t**)frame->extended_data;

            int out_count = (int64_t)frame->nb_samples * wanted_spec.freq / frame->sample_rate + 256;
            int out_size  = av_samples_get_buffer_size(NULL, chLayout.nb_channels, out_count, AV_SAMPLE_FMT_S16, 0);
            int len2;
            if (out_size < 0) {
                qDebug() << "error 1";
            }
            uint8_t* out = new uint8_t[out_size];

            //            if (wanted_nb_samples != af->frame->nb_samples) {
            //                if (swr_set_compensation(is->swr_ctx, (wanted_nb_samples - af->frame->nb_samples) * is->audio_tgt.freq / af->frame->sample_rate,
            //                                         wanted_nb_samples * is->audio_tgt.freq / af->frame->sample_rate) < 0) {
            //                    av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
            //                    return -1;
            //                }
            //            }

            len2 = swr_convert(swr_ctx, &out, out_count, in, frame->nb_samples);
            if (len2 < 0) {
                qDebug() << "error 2";
            }
            if (len2 == out_count) {
                qDebug() << "error 3";
                //                av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
                //                if (swr_init(is->swr_ctx) < 0)
                //                    swr_free(&is->swr_ctx);
            }
            auto size = len2 * chLayout.nb_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

            audioMutex->lock();

            //            audioArr.append((char*)frame->data[0], dataSize);
            audioArr.append((char*)out, size);

            audioMutex->unlock();
            av_frame_unref(frame);
        }
        av_packet_unref(pkt);

        //        }

    } else if (pkt->stream_index == videoIdx) {
        //        auto ret = avcodec_send_packet(videoCodecParams.first, pkt);
        //        if (ret < 0)
        //            qDebug() << "Decode Error 1";

        //        auto frame = av_frame_alloc();
        //        ret        = avcodec_receive_frame(videoCodecParams.first, frame);
        //        if (ret < 0) {
        //            qDebug() << "Decode Error 2";
        //            return;
        //        }

        //        //    if (ret >= 0)
        //        //        emit

        //        qDebug() << frame->pts << av_q2d(frame->time_base) << frame->pts << av_q2d(videoCodecParams.first->framerate);
        //        qDebug() << av_q2d(videoCodecParams.first->time_base) << av_q2d(videoCodecParams.first->pkt_timebase);
        //        qDebug() << frame->pkt_dts << frame->pkt_duration << frame->best_effort_timestamp;
        //        auto stream = formatCtx->streams[videoIdx];
        //        qDebug() << av_q2d(stream->avg_frame_rate) << av_q2d(stream->r_frame_rate) << av_q2d(stream->time_base) << stream->duration;

        //        double pts = frame->pkt_dts;
        //        if (pts == AV_NOPTS_VALUE)
        //            pts = frame->pkt_dts;
        //        if (pts == AV_NOPTS_VALUE)
        //            pts = 0;
        //        pts *= av_q2d(formatCtx->streams[videoIdx]->time_base);
        //        synchronize_video(frame, &pts);

        //        qDebug() << clocks.first;

        //        while (pictureReady)
        //            ;
        //        picturePts = pts;

        //        out_buffer = (unsigned char*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, videoCodecParams.first->width, videoCodecParams.first->height, 1));
        //        av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
        //            AV_PIX_FMT_YUV420P, videoCodecParams.first->width, videoCodecParams.first->height, 1);

        //        sws_scale(imageConvertCtx, (const unsigned char* const*)frame->data, frame->linesize, 0, videoCodecParams.first->height,
        //            pFrameYUV->data, pFrameYUV->linesize);

        //        SDL_UpdateYUVTexture(texture, &sdlRect,
        //            pFrameYUV->data[0], pFrameYUV->linesize[0],
        //            pFrameYUV->data[1], pFrameYUV->linesize[1],
        //            pFrameYUV->data[2], pFrameYUV->linesize[2]);

        //        SDL_RenderClear(renderer);
        //        //                SDL_RenderCopy(renderer, texture, NULL, &presentRect);
        //        SDL_RenderCopy(renderer, texture, NULL, &sdlRect);

        //        SDL_RenderPresent(renderer);

        av_packet_unref(pkt);
    }
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
    this->widgetId = widgetId;
    //    this->widgetId = videoData.videoWidget->winId();

    //    auto size = videoData.videoWidget->size();

    //    this->presentRect.h = size.height();
    //    this->presentRect.w = size.width();
}
