//#include "packetqueue.h"

//PacketQueue::PacketQueue()
//{
//}

//PacketQueue::~PacketQueue()
//{
//}

//int PacketQueue::putPrivate(AVPacket* pkt)
//{
//    MyAVPacketList pkt1;
//    int            ret;

//    if (abort_request)
//        return -1;

//    pkt1.pkt    = pkt;
//    pkt1.serial = serial;

//    ret = av_fifo_write(pkt_list, &pkt1, 1);
//    if (ret < 0)
//        return ret;
//    nb_packets++;
//    size += pkt1.pkt->size + sizeof(pkt1);
//    duration += pkt1.pkt->duration;
//    /* XXX: should duplicate packet data in DV case */
//    SDL_CondSignal(cond);
//    return 0;
//}

//int PacketQueue::put(AVPacket* pkt)
//{
//    AVPacket* pkt1;
//    int       ret;

//    pkt1 = (AVPacket*)av_packet_alloc();
//    if (!pkt1) {
//        av_packet_unref(pkt);
//        return -1;
//    }
//    av_packet_move_ref(pkt1, pkt);

//    SDL_LockMutex(mutex);
//    ret = putPrivate(pkt1);
//    SDL_UnlockMutex(mutex);

//    if (ret < 0)
//        av_packet_free(&pkt1);

//    return ret;
//}

//int PacketQueue::putNullpacket(AVPacket* pkt, int stream_index)
//{
//    pkt->stream_index = stream_index;
//    return put(pkt);
//}

//int PacketQueue::init()
//{
//    pkt_list = av_fifo_alloc2(1, sizeof(MyAVPacketList), AV_FIFO_FLAG_AUTO_GROW);
//    if (!pkt_list)
//        return AVERROR(ENOMEM);
//    mutex = SDL_CreateMutex();
//    if (!mutex) {
//        av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
//        return AVERROR(ENOMEM);
//    }
//    cond = SDL_CreateCond();
//    if (!cond) {
//        av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
//        return AVERROR(ENOMEM);
//    }
//    abort_request = 1;
//    return 0;
//}

//void PacketQueue::destroy()
//{
//    flush();
//    av_fifo_freep2(&pkt_list);
//    SDL_DestroyMutex(mutex);
//    SDL_DestroyCond(cond);
//}

//void PacketQueue::flush()
//{
//    MyAVPacketList pkt1;

//    SDL_LockMutex(mutex);
//    while (av_fifo_read(pkt_list, &pkt1, 1) >= 0)
//        av_packet_free(&pkt1.pkt);
//    nb_packets = 0;
//    size       = 0;
//    duration   = 0;
//    serial++;
//    SDL_UnlockMutex(mutex);
//}

//void PacketQueue::abort()
//{
//    SDL_LockMutex(mutex);

//    abort_request = 1;

//    SDL_CondSignal(cond);
//    SDL_UnlockMutex(mutex);
//}

//void PacketQueue::start()
//{
//    SDL_LockMutex(mutex);
//    abort_request = 0;
//    serial++;
//    SDL_UnlockMutex(mutex);
//}

//int PacketQueue::get(AVPacket* pkt, int block, int* serial)
//{
//    MyAVPacketList pkt1;
//    int            ret;

//    SDL_LockMutex(mutex);

//    for (;;) {
//        if (abort_request) {
//            ret = -1;
//            break;
//        }

//        if (av_fifo_read(pkt_list, &pkt1, 1) >= 0) {
//            nb_packets--;
//            size -= pkt1.pkt->size + sizeof(pkt1);
//            duration -= pkt1.pkt->duration;
//            av_packet_move_ref(pkt, pkt1.pkt);
//            if (serial)
//                *serial = pkt1.serial;
//            av_packet_free(&pkt1.pkt);
//            ret = 1;
//            break;
//        } else if (!block) {
//            ret = 0;
//            break;
//        } else {
//            SDL_CondWait(cond, mutex);
//        }
//    }
//    SDL_UnlockMutex(mutex);
//    return ret;
//}

//int PacketQueue::getAbortRequest() const
//{
//    return this->abort_request;
//}

//int PacketQueue::getSerial() const
//{
//    return this->serial;
//}

//int* PacketQueue::getSerialPtr()
//{
//    return &this->serial;
//}

//int PacketQueue::getNbPackets() const
//{
//    return this->nb_packets;
//}

//int PacketQueue::getSize()
//{
//    return this->size;
//}

//int64_t PacketQueue::getDuration()
//{
//    return this->duration;
//}
