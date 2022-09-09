#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include "VideoDefs.h"
#include "VideoState.h"
#include "decoder.h"
#include "framequeue.h"
#include "packetqueue.h"
#include "videohandlers.h"
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QWidget>

class VideoController : public QObject {
    Q_OBJECT
public:
    explicit VideoController(QObject* parent = nullptr);

    void setVideoParameters(QWidget* videoWidget);
    void setPlay(bool play);

    void initSDL(WId winId);
    void streamOpen();
    void streamClose();
    void streamComponentClose(int idx);

    void eventLoop();
    void doExit();

signals:

private:
    VideoState* is { nullptr };

    //    std::string path { "udp://127.0.0.1:7777" };
    std::string path { "rtsp://192.168.0.124:554/stream1" };
    int         avSyncType { AV_SYNC_AUDIO_MASTER };

    SDL_Window*   window { nullptr };
    SDL_Renderer* renderer { nullptr };
};

#endif // VIDEOCONTROLLER_H
