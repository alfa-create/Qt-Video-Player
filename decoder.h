//#ifndef DECODER_H
//#define DECODER_H

//#include "VideoDefs.h"
//#include "framequeue.h"
//#include "packetqueue.h"

//class Decoder {
//public:
//    Decoder();
//    ~Decoder();

//    int  init(AVCodecContext* avctx, PacketQueue* queue, SDL_cond* empty_queue_cond);
//    void destroy();

//    int decodeFrame(AVFrame* frame, AVSubtitle* sub);
//    int start(int (*fn)(void*), const char* thread_name, void* arg);

//    void abort(FrameQueue* fq);

//    void            setStartPts(int64_t pts);
//    void            setStartPtsTb(AVRational ptsTb);
//    int             getPktSerial() const;
//    AVCodecContext* getAVCtx();

//private:
//    AVPacket*       pkt { nullptr };
//    PacketQueue*    queue { nullptr };
//    AVCodecContext* avctx { nullptr };
//    int             pkt_serial { 0 };
//    int             finished { 0 };
//    int             packet_pending { 0 };
//    SDL_cond*       empty_queue_cond { nullptr };
//    int64_t         start_pts { 0 };
//    AVRational      start_pts_tb { 0, 0 };
//    int64_t         next_pts { 0 };
//    AVRational      next_pts_tb { 0, 0 };
//    SDL_Thread*     decoder_tid { nullptr };
//};

//#endif // DECODER_H
