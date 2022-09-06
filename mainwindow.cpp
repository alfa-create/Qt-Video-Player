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
    auto widget2 = new VideoController(this);
    widget2->setVideoParameters(ui->widget->winId());
    widget2->setPlay(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPlay(bool play)
{
    auto inf = av_version_info();
    qDebug() << inf;
    if (play) {
        // create player + ffmpeg lib

        videoThread = QThread::create([&] {
            try {

                while (true) {
                    packetMutex->lock();

                    if (packetVector.size()) {

                        //                        qDebug() << packetVector.size();

                        auto packet2 = packetVector[0];

                        auto ret2 = avcodec_send_packet(pCodecCtx, &packet2);
                        if (ret2 < 0) {
                            int a = 0;
                            Q_UNUSED(a);
                            if (ret == AVERROR(EAGAIN))
                                a = 1;
                            else if (ret == AVERROR(EINVAL)) {
                                a = 2;
                            } else if (ret == AVERROR(ENOMEM))
                                a = 3;
                            else if (ret == AVERROR_EOF) {
                                a = 4;
                            }

                            qDebug() << "Decode Error 1" << a << ret2;
                            //                            throw std::runtime_error("Decode Error 1");
                            //                            packetMutex->unlock();
                            //                            continue;
                        }
                        ret2 = avcodec_receive_frame(pCodecCtx, pFrame);
                        if (ret2 < 0) {
                            int a = 0;
                            if (ret == AVERROR_EOF)
                                a = 1;
                            else if (ret == AVERROR(EAGAIN)) {
                                a = 2;
                                continue;
                            } else if (ret == AVERROR(EINVAL))
                                a = 3;

                            qDebug() << "Decode Error 2" << a << ret2;

                            //                            throw std::runtime_error("Decode Error 2");
                            //                            packetMutex->unlock();
                            //                            continue;
                        }
                        packetVector.pop_front();
                        packetMutex->unlock();

                        sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                            pFrameYUV->data, pFrameYUV->linesize);

                        SDL_UpdateYUVTexture(sdlTexture, &sdlRect,
                            pFrameYUV->data[0], pFrameYUV->linesize[0],
                            pFrameYUV->data[1], pFrameYUV->linesize[1],
                            pFrameYUV->data[2], pFrameYUV->linesize[2]);

                        SDL_RenderClear(sdlRenderer);
                        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &presentRect);
                        SDL_RenderPresent(sdlRenderer);
                    } else {
                        packetMutex->unlock();
                    }
                }

            } catch (const std::runtime_error& ex) {
                std::string err = ex.what();
                qDebug() << "!" << ex.what();
            } catch (const std::exception& ex) {
                qDebug("! unknown error: %s", ex.what());
            }
        });

        connect(this->thread(), &QThread::finished, videoThread, &QThread::deleteLater);
        videoThread->start(QThread::HighestPriority);

        sdlEventLoopThread = QThread::create([&] {
            SDL_Event event;

            //            while (true) {
            //                while (SDL_PollEvent(&event)) {
            //                    if (event.type == SDL_QUIT) {
            //                        if (videoThread) {
            //                            videoThread->terminate();
            //                            videoThread->wait();
            //                            videoThread->deleteLater();
            //                            videoThread = nullptr;
            //                        }
            //                    }
            //                }
            //            }
        });

        connect(this->thread(), &QThread::finished, sdlEventLoopThread, &QThread::deleteLater);
        sdlEventLoopThread->start(QThread::HighestPriority);

        readThread = QThread::create([&] {
            packetMutex->lock();

            if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {
                throw std::runtime_error("Couldn't open input stream");
            }

            if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
                throw std::runtime_error("Couldn't find stream information");
            }
            videoindex = -1;
            for (i = 0; i < pFormatCtx->nb_streams; i++)
                if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {

                    videoindex = i;
                    break;
                }
            if (videoindex == -1) {
                throw std::runtime_error("Didn't find a video stream");
            }

            AVStream*          stream   = pFormatCtx->streams[videoindex];
            AVCodecParameters* codecpar = stream->codecpar;
            auto               avctx    = avcodec_alloc_context3(NULL);

            ret        = avcodec_parameters_to_context(avctx, codecpar);
            auto codec = avcodec_find_decoder(avctx->codec_id);

            pCodecCtx = avctx;
            if (avcodec_open2(pCodecCtx, codec, NULL) < 0)
                qDebug() << "open";

            //            pCodecCtx = pFormatCtx->streams[videoindex]->codec;

            //            pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
            //            if (pCodec == NULL) {
            //                throw std::runtime_error("Codec not found");
            //            }
            //            if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
            //                throw std::runtime_error("Could not open codec");
            //            }
            pFrame     = av_frame_alloc();
            pFrameYUV  = av_frame_alloc();
            out_buffer = (unsigned char*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
            av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
                AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

            //            packet          = (AVPacket*)av_malloc(sizeof(AVPacket));
            packet = av_packet_alloc();

            img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

            //                if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
            //                    throw std::runtime_error("Could not initialize SDL");
            //                }
            screen_w = pCodecCtx->width;
            screen_h = pCodecCtx->height;

            //                screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            //                    screen_w, screen_h, SDL_WINDOW_OPENGL);

            //                int a  = widgetId;
            screen = SDL_CreateWindowFrom((void*)widgetId);
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
            if (!screen) {
                throw std::runtime_error("SDL: could not create window");
            }
            SDL_ShowWindow(screen);

            sdlRenderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            sdlTexture  = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);

            sdlRect.x = 0;
            sdlRect.y = 0;
            sdlRect.w = screen_w;
            sdlRect.h = screen_h;
            //                sdlRect.w = 600;
            //                sdlRect.h = 600;

            packetMutex->unlock();

            while (true) {
                if (av_read_frame(pFormatCtx, packet) >= 0) {
                    if (packet->stream_index == videoindex) {
                        packetMutex->lock();
                        packetVector.push_back(*packet);
                        //                        av_packet_unref(packet);
                        packetMutex->unlock();
                    }
                }
            }
        });
        connect(this->thread(), &QThread::finished, readThread, &QThread::deleteLater);
        readThread->start(QThread::HighestPriority);
    } else {
        // delete thread + unitialize
        if (videoThread) {
            if (videoThread->isRunning()) {
                videoThread->terminate();
                videoThread->wait();
                //                videoThread->deleteLater();
                delete videoThread;
                videoThread = nullptr;
            }

            SDL_Event event;
            while (SDL_PollEvent(&event)) {
            }

            if (pFormatCtx) {
                avformat_close_input(&pFormatCtx);
                pFormatCtx = nullptr;
            }

            if (img_convert_ctx) {
                sws_freeContext(img_convert_ctx);
                img_convert_ctx = nullptr;
            }

            if (screen) {
                SDL_DestroyWindow(screen);
                screen = nullptr;
            }

            if (sdlRenderer) {
                SDL_DestroyRenderer(sdlRenderer);
                sdlRenderer = nullptr;
            }

            if (sdlTexture) {
                SDL_DestroyTexture(sdlTexture);
                sdlTexture = nullptr;
            }

            if (pFrameYUV) {
                av_frame_free(&pFrameYUV);
                pFrameYUV = nullptr;
            }
            if (pFrame) {
                av_frame_free(&pFrame);
                pFrame = nullptr;
            }
        }
    }
}

void MainWindow::setVideoParameters(QWidget* videoWidget)
{
    videoWidget->resize(QSize(704, 576));
    this->widgetId = videoWidget->winId();
    auto size      = videoWidget->size();

    this->presentRect.h = size.height();
    this->presentRect.w = size.width();
}
