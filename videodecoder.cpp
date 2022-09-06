#include "videodecoder.h"

VideoDecoder::VideoDecoder(
    VideoParameters* videoParameters,
    QObject*         parent)
    : QObject(parent)
    , videoParameters(videoParameters)
{
    qRegisterMetaType<AVPacket*>("AVPacket*");
}

void VideoDecoder::decode(AVPacket* pkt)
{
    if (pkt->stream_index == videoParameters->audioIdx) {
        auto ret = avcodec_send_packet(videoParameters->audioCodecParams.first, pkt);
        if (ret < 0)
            qDebug() << "Decode Error 1";

        while (true) {
            auto frame = av_frame_alloc();
            ret        = avcodec_receive_frame(videoParameters->audioCodecParams.first, frame);
            //            auto a     = 0;
            //            if (ret == AVERROR_EOF)
            //                a = 1;
            //            else if (ret == AVERROR(EAGAIN)) {
            //                a = 2;
            //            } else if (ret == AVERROR(EINVAL))
            //                a = 3;

            if (ret < 0) {
                //                qDebug() << "Decode Error 2"
                //                         << "audio" << ((pkt->stream_index == videoParameters->audioIdx) ? "1" : "0") << a;
                return;
            }

            auto dataSize = av_samples_get_buffer_size(
                NULL, frame->ch_layout.nb_channels, frame->nb_samples,
                AVSampleFormat(frame->format), 1);

            // FIX: frame->format не AV_SAMPLE_FMT_S16 но уже есть swr -> не создавать заново
            // только если исходный формат фрейма не поменялся!
            if (((frame->format != AV_SAMPLE_FMT_S16) /*&& !videoParameters->swr_ctx*/)
                || av_channel_layout_compare(&frame->ch_layout, &videoParameters->chLayout)) {

                if (videoParameters->swr_ctx)
                    swr_free(&videoParameters->swr_ctx);

                auto err = swr_alloc_set_opts2(&videoParameters->swr_ctx,
                    &videoParameters->chLayout,
                    AV_SAMPLE_FMT_S16,
                    videoParameters->wanted_spec.freq,
                    &frame->ch_layout,
                    (AVSampleFormat)(frame->format),
                    frame->sample_rate,
                    0, NULL);

                if (!videoParameters->swr_ctx || swr_init(videoParameters->swr_ctx) < 0)
                    qDebug() << "error swr_ctx";

                av_channel_layout_copy(&videoParameters->chLayout, &frame->ch_layout);
                //                wanted_spec.freq = frame->sample_rate;

                qDebug() << "asdf";
            }

            // refactor
            if (videoParameters->swr_ctx) {
                const uint8_t** in = (const uint8_t**)frame->extended_data;

                int out_count = (int64_t)frame->nb_samples * videoParameters->wanted_spec.freq / frame->sample_rate + 256;
                int out_size  = av_samples_get_buffer_size(NULL, videoParameters->chLayout.nb_channels, out_count, AV_SAMPLE_FMT_S16, 0);
                int len2;
                if (out_size < 0) {
                    qDebug() << "error 1";
                }
                uint8_t* out = new uint8_t[out_size];
                len2         = swr_convert(videoParameters->swr_ctx, &out, out_count, in, frame->nb_samples);
                if (len2 < 0) {
                    qDebug() << "error 2";
                }
                if (len2 == out_count) {
                    qDebug() << "error 3";
                    //                av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
                    //                if (swr_init(is->swr_ctx) < 0)
                    //                    swr_free(&is->swr_ctx);
                }
                auto size = len2 * videoParameters->chLayout.nb_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

                videoParameters->audioMutex->lock();

                //            audioArr.append((char*)frame->data[0], dataSize);
                videoParameters->audioArr.append((char*)out, size);
                delete[] out;

                videoParameters->audioMutex->unlock();
            } else {
                videoParameters->audioMutex->lock();
                videoParameters->audioArr.append((char*)frame->data[0], dataSize);

                videoParameters->audioMutex->unlock();
            }
            av_frame_unref(frame);
        }
        av_packet_unref(pkt);

        //        }

    } else if (pkt->stream_index == videoParameters->videoIdx) {
        auto ret = avcodec_send_packet(videoParameters->videoCodecParams.first, pkt);
        if (ret < 0)
            qDebug() << "Decode Error 1";

        auto frame = av_frame_alloc();
        ret        = avcodec_receive_frame(videoParameters->videoCodecParams.first, frame);
        if (ret < 0) {
            qDebug() << "Decode Error 2";
            return;
        }

        //    if (ret >= 0)
        //        emit

        //        qDebug() << frame->pts << av_q2d(frame->time_base) << frame->pts << av_q2d(videoParameters->videoCodecParams.first->framerate);
        //        qDebug() << av_q2d(videoParameters->videoCodecParams.first->time_base) << av_q2d(videoParameters->videoCodecParams.first->pkt_timebase);
        //        qDebug() << frame->pkt_dts << frame->pkt_duration << frame->best_effort_timestamp;
        //        auto stream = videoParameters->formatCtx->streams[videoParameters->videoIdx];
        //        qDebug() << av_q2d(stream->avg_frame_rate) << av_q2d(stream->r_frame_rate) << av_q2d(stream->time_base) << stream->duration;

        //        double pts = frame->pkt_dts;
        //        if (pts == AV_NOPTS_VALUE)
        //            pts = frame->pkt_dts;
        //        if (pts == AV_NOPTS_VALUE)
        //            pts = 0;
        //        pts *= av_q2d(videoParameters->formatCtx->streams[videoParameters->videoIdx]->time_base);
        //        synchronize_video(frame, &pts);

        //        qDebug() << clocks.first;

        //        while (pictureReady)
        //            ;
        //        picturePts = pts;

        auto buffer = new uint8_t[av_image_get_buffer_size(
            AV_PIX_FMT_YUV420P,
            videoParameters->videoCodecParams.first->width,
            videoParameters->videoCodecParams.first->height,
            1)];

        av_image_fill_arrays(
            videoParameters->pFrameYUV->data,
            videoParameters->pFrameYUV->linesize,
            reinterpret_cast<const uint8_t*>(buffer),
            AV_PIX_FMT_YUV420P,
            videoParameters->videoCodecParams.first->width,
            videoParameters->videoCodecParams.first->height,
            1);

        sws_scale(
            videoParameters->imageConvertCtx,
            (const unsigned char* const*)frame->data,
            frame->linesize,
            0,
            videoParameters->videoCodecParams.first->height,
            videoParameters->pFrameYUV->data,
            videoParameters->pFrameYUV->linesize);

        SDL_UpdateYUVTexture(
            videoParameters->texture,
            &videoParameters->sdlRect,
            videoParameters->pFrameYUV->data[0],
            videoParameters->pFrameYUV->linesize[0],
            videoParameters->pFrameYUV->data[1],
            videoParameters->pFrameYUV->linesize[1],
            videoParameters->pFrameYUV->data[2],
            videoParameters->pFrameYUV->linesize[2]);

        SDL_RenderClear(videoParameters->renderer);
        //                SDL_RenderCopy(renderer, texture, NULL, &presentRect);
        SDL_RenderCopy(videoParameters->renderer, videoParameters->texture, NULL, &videoParameters->sdlRect);

        SDL_RenderPresent(videoParameters->renderer);

        av_packet_unref(pkt);
    }
}
