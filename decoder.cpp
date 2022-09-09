//#include "decoder.h"

//Decoder::Decoder()
//{
//}

//Decoder::~Decoder()
//{
//}

//int Decoder::init(AVCodecContext* avctx, PacketQueue* queue, SDL_cond* empty_queue_cond)
//{
//    pkt = av_packet_alloc();
//    if (!pkt)
//        return AVERROR(ENOMEM);
//    this->avctx            = avctx;
//    this->queue            = queue;
//    this->empty_queue_cond = empty_queue_cond;
//    this->start_pts        = AV_NOPTS_VALUE;
//    this->pkt_serial       = -1;
//    return 0;
//}

//void Decoder::destroy()
//{
//    av_packet_free(&pkt);
//    avcodec_free_context(&avctx);
//}

//int Decoder::decodeFrame(AVFrame* frame, AVSubtitle* sub)
//{
//    int ret = AVERROR(EAGAIN);

//    for (;;) {
//        if (queue->getSerial() == pkt_serial) {
//            do {
//                if (queue->getAbortRequest())
//                    return -1;

//                switch (avctx->codec_type) {
//                case AVMEDIA_TYPE_VIDEO:
//                    ret = avcodec_receive_frame(avctx, frame);
//                    if (ret >= 0) {
//                        if (decoder_reorder_pts == -1) {
//                            frame->pts = frame->best_effort_timestamp;
//                        } else if (!decoder_reorder_pts) {
//                            frame->pts = frame->pkt_dts;
//                        }
//                    }
//                    break;
//                case AVMEDIA_TYPE_AUDIO:
//                    ret = avcodec_receive_frame(avctx, frame);
//                    if (ret >= 0) {
//                        AVRational tb = (AVRational) { 1, frame->sample_rate };
//                        if (frame->pts != AV_NOPTS_VALUE)
//                            frame->pts = av_rescale_q(frame->pts, avctx->pkt_timebase, tb);
//                        else if (next_pts != AV_NOPTS_VALUE)
//                            frame->pts = av_rescale_q(next_pts, next_pts_tb, tb);
//                        if (frame->pts != AV_NOPTS_VALUE) {
//                            next_pts    = frame->pts + frame->nb_samples;
//                            next_pts_tb = tb;
//                        }
//                    }
//                    break;
//                default:
//                    break;
//                }
//                if (ret == AVERROR_EOF) {
//                    finished = pkt_serial;
//                    avcodec_flush_buffers(avctx);
//                    return 0;
//                }
//                if (ret >= 0)
//                    return 1;
//            } while (ret != AVERROR(EAGAIN));
//        }

//        do {
//            if (queue->getNbPackets() == 0)
//                SDL_CondSignal(empty_queue_cond);
//            if (packet_pending) {
//                packet_pending = 0;
//            } else {
//                int old_serial = pkt_serial;
//                if (queue->get(pkt, 1, &pkt_serial) < 0)
//                    return -1;
//                if (old_serial != pkt_serial) {
//                    avcodec_flush_buffers(avctx);
//                    finished    = 0;
//                    next_pts    = start_pts;
//                    next_pts_tb = start_pts_tb;
//                }
//            }
//            if (queue->getSerial() == pkt_serial)
//                break;
//            av_packet_unref(pkt);
//        } while (1);

//        if (avctx->codec_type == AVMEDIA_TYPE_SUBTITLE) {
//            int got_frame = 0;
//            ret           = avcodec_decode_subtitle2(avctx, sub, &got_frame, pkt);
//            if (ret < 0) {
//                ret = AVERROR(EAGAIN);
//            } else {
//                if (got_frame && !pkt->data) {
//                    packet_pending = 1;
//                }
//                ret = got_frame ? 0 : (pkt->data ? AVERROR(EAGAIN) : AVERROR_EOF);
//            }
//            av_packet_unref(pkt);
//        } else {
//            if (avcodec_send_packet(avctx, pkt) == AVERROR(EAGAIN)) {
//                av_log(avctx, AV_LOG_ERROR, "Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
//                packet_pending = 1;
//            } else {
//                av_packet_unref(pkt);
//            }
//        }
//    }
//}

//int Decoder::start(int (*fn)(void*), const char* thread_name, void* arg)
//{
//    queue->start();
//    decoder_tid = SDL_CreateThread(fn, thread_name, arg);
//    if (!decoder_tid) {
//        av_log(NULL, AV_LOG_ERROR, "SDL_CreateThread(): %s\n", SDL_GetError());
//        return AVERROR(ENOMEM);
//    }
//    return 0;
//}

//void Decoder::abort(FrameQueue* fq)
//{
//    queue->abort();
//    fq->signal();
//    SDL_WaitThread(decoder_tid, NULL);
//    decoder_tid = NULL;
//    queue->flush();
//}

//void Decoder::setStartPts(int64_t pts)
//{
//    start_pts = pts;
//}

//void Decoder::setStartPtsTb(AVRational ptsTb)
//{
//    start_pts_tb = ptsTb;
//}

//int Decoder::getPktSerial() const
//{
//    return this->pkt_serial;
//}

//AVCodecContext* Decoder::getAVCtx()
//{
//    return this->avctx;
//}
