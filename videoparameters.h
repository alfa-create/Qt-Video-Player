#ifndef VIDEOPARAMETERS_H
#define VIDEOPARAMETERS_H

// sdl && ffmpeg
#include <QMutex>
#include <QWidget>
#include <string>

#define __STDC_CONSTANT_MACROS
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/samplefmt.h>
}
#undef main
#include <SDL.h>
#undef main

enum class SyncType {
    AUDIO_MASTER    = 1,
    EXTERNAL_MASTER = 2,
};

class VideoParameters {
public:
    explicit VideoParameters();

    SDL_Window*        screen { nullptr };
    WId                widgetId { 0 };
    SDL_Renderer*      renderer { nullptr };
    SDL_Texture*       texture { nullptr };
    struct SwsContext* imageConvertCtx { nullptr };

    QByteArray audioArr;
    QMutex*    audioMutex { nullptr };

    // ffmpeg parameters
    AVFormatContext* formatCtx { nullptr };
    //    std::string      path { "udp://127.0.0.1:7777" };
    std::string path { "D://work/_video/video.mp4" };
    //    std::string path { "D://work/_video/song.mp3" };
    //    std::string path { "rtsp://192.168.0.124:554/stream0" };

    int                                        videoIdx { -1 };
    int                                        audioIdx { -1 };
    std::pair<AVCodecContext*, const AVCodec*> videoCodecParams { nullptr, nullptr };
    std::pair<AVCodecContext*, const AVCodec*> audioCodecParams { nullptr, nullptr };

    SDL_AudioSpec      wanted_spec;
    AVChannelLayout    chLayout;
    struct SwrContext* swr_ctx { nullptr };

    SDL_Rect sdlRect;

    SDL_Rect presentRect { 0, 0, 0, 0 };
    AVFrame* pFrameYUV { nullptr };
};

#endif // VIDEOPARAMETERS_H
