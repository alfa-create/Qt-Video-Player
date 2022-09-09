//#ifndef PACKETQUEUE_H
//#define PACKETQUEUE_H

//#include "VideoDefs.h"
//#include <QObject>

//class PacketQueue {
//public:
//    explicit PacketQueue();
//    ~PacketQueue(); //destroy

//    int  putPrivate(AVPacket* pkt);
//    int  put(AVPacket* pkt);
//    int  putNullpacket(AVPacket* pkt, int stream_index);
//    int  init();
//    void destroy();
//    void flush();
//    void abort();
//    void start();
//    int  get(AVPacket* pkt, int block, int* serial);

//    int  getAbortRequest() const;
//    int  getSerial() const;
//    int* getSerialPtr();

//    int getNbPackets() const;

//    int     getSize();
//    int64_t getDuration();

//signals:

//private:
//    AVFifo*    pkt_list { nullptr };
//    int        nb_packets { 0 };
//    int        size { 0 };
//    int64_t    duration { 0 };
//    int        abort_request { 0 };
//    int        serial { 0 };
//    SDL_mutex* mutex { nullptr };
//    SDL_cond*  cond { nullptr };
//};

//#endif // PACKETQUEUE_H
