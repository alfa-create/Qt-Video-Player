//#ifndef FRAMEQUEUE_H
//#define FRAMEQUEUE_H

//#include "VideoDefs.h"
//#include "packetqueue.h"
//#include <QObject>

//class FrameQueue {
//public:
//    explicit FrameQueue();
//    ~FrameQueue();

//    static void unrefItem(Frame* vp);

//    int  init(PacketQueue* pktq, int max_size, int keep_last);
//    void destroy();

//    void   signal();
//    Frame* peek();
//    Frame* peekNext();
//    Frame* peekLast();
//    Frame* peekWritable();
//    Frame* peekReadble();

//    void push();
//    void next();

//    int     nbRemaining();
//    int64_t lastPos();

//    SDL_mutex* getMutex();

//    int getRIndexShown();

//signals:

//private:
//    Frame        queue[FRAME_QUEUE_SIZE];
//    int          rindex { 0 };
//    int          windex { 0 };
//    int          size { 0 };
//    int          max_size { 0 };
//    int          keep_last { 0 };
//    int          rindex_shown { 0 };
//    SDL_mutex*   mutex { nullptr };
//    SDL_cond*    cond { nullptr };
//    PacketQueue* pktq { nullptr };
//};

//#endif // FRAMEQUEUE_H
