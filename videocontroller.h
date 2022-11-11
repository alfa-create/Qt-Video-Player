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
    ~VideoController();

    void setVideoParameters(QWidget* videoWidget);
    void setVideoBufferSize(const uint64_t& size);
    void setPlay(bool play);

    void initSDL(WId winId);

private:
    // send sdl event exit
    void streamClose();
    // delete pointers in controller
    void deleteStream();

signals:

private:
    VideoState* is { nullptr };
    QWidget*    videoWidget { nullptr };

    QThread* videoThread { nullptr };

    std::string path { "udp://127.0.0.1:7777" };
    //    std::string path { "rtsp://192.168.0.124:554/stream1" };
    //    std::string path { "D://work/_video/video.mp4" };
    int avSyncType { AV_SYNC_AUDIO_MASTER };

    SDL_Window*   window { nullptr };
    SDL_Renderer* renderer { nullptr };
};

#endif // VIDEOCONTROLLER_H
