//#include "framequeue.h"

//FrameQueue::FrameQueue()
//{
//}

//FrameQueue::~FrameQueue()
//{
//}

//void FrameQueue::unrefItem(Frame* vp)
//{
//    av_frame_unref(vp->frame);
//    avsubtitle_free(&vp->sub);
//}

//int FrameQueue::init(PacketQueue* pktq, int max_size, int keep_last)
//{
//    int i;
//    if (!(mutex = SDL_CreateMutex())) {
//        av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
//        return AVERROR(ENOMEM);
//    }
//    if (!(cond = SDL_CreateCond())) {
//        av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
//        return AVERROR(ENOMEM);
//    }
//    this->pktq      = pktq;
//    max_size        = FFMIN(max_size, FRAME_QUEUE_SIZE);
//    this->keep_last = !!keep_last;

//    for (i = 0; i < max_size; i++)
//        if (!(queue[i].frame = av_frame_alloc()))
//            return AVERROR(ENOMEM);
//    return 0;
//}

//void FrameQueue::destroy()
//{
//    int i;
//    for (i = 0; i < max_size; i++) {
//        Frame* vp = &queue[i];
//        unrefItem(vp);
//        av_frame_free(&vp->frame);
//    }
//    SDL_DestroyMutex(mutex);
//    SDL_DestroyCond(cond);
//}

//void FrameQueue::signal()
//{
//    SDL_LockMutex(mutex);
//    SDL_CondSignal(cond);
//    SDL_UnlockMutex(mutex);
//}

//Frame* FrameQueue::peek()
//{
//    return &queue[(rindex + rindex_shown) % max_size];
//}

//Frame* FrameQueue::peekNext()
//{
//    return &queue[(rindex + rindex_shown + 1) % max_size];
//}

//Frame* FrameQueue::peekLast()
//{
//    return &queue[rindex];
//}

//Frame* FrameQueue::peekWritable()
//{
//    /* wait until we have space to put a new frame */
//    SDL_LockMutex(mutex);
//    while (size >= max_size && !pktq->getAbortRequest()) {
//        SDL_CondWait(cond, mutex);
//    }
//    SDL_UnlockMutex(mutex);

//    if (pktq->getAbortRequest())
//        return NULL;

//    return &queue[windex];
//}

//Frame* FrameQueue::peekReadble()
//{
//    /* wait until we have a readable a new frame */
//    SDL_LockMutex(mutex);
//    while (size - rindex_shown <= 0 && !pktq->getAbortRequest()) {
//        SDL_CondWait(cond, mutex);
//    }
//    SDL_UnlockMutex(mutex);

//    if (pktq->getAbortRequest())
//        return NULL;

//    return &queue[(rindex + rindex_shown) % max_size];
//}

//void FrameQueue::push()
//{
//    if (++windex == max_size)
//        windex = 0;
//    SDL_LockMutex(mutex);
//    size++;
//    SDL_CondSignal(cond);
//    SDL_UnlockMutex(mutex);
//}

//void FrameQueue::next()
//{
//    if (keep_last && !rindex_shown) {
//        rindex_shown = 1;
//        return;
//    }
//    unrefItem(&queue[rindex]);
//    if (++rindex == max_size)
//        rindex = 0;
//    SDL_LockMutex(mutex);
//    size--;
//    SDL_CondSignal(cond);
//    SDL_UnlockMutex(mutex);
//}

//int FrameQueue::nbRemaining()
//{
//    return size - rindex_shown;
//}

//int64_t FrameQueue::lastPos()
//{
//    Frame* fp = &queue[rindex];
//    if (rindex_shown && fp->serial == pktq->getSerial())
//        return fp->pos;
//    else
//        return -1;
//}

//SDL_mutex* FrameQueue::getMutex()
//{
//    return this->mutex;
//}

//int FrameQueue::getRIndexShown()
//{
//    return this->rindex_shown;
//}
