#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include "threadsafevector.h"
#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QUrl>
#include <QWidget>
#include <QtWidgets/QWidget>

//#include <controllers/icontroller.h>
//#include "imodem.h"

// sdl && ffmpeg
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
#include <exception>

void audio_callback(void* userdata, Uint8* stream, int len);

class VideoController : public QObject /*, public IController*/ {
    Q_OBJECT
    //    Q_INTERFACES(IController)
public:
    explicit VideoController(/*IDataHandler* dataHandler,
        IModem::IListener*                 listener,*/
        QObject* parent = nullptr);
    //    void handle(const ICommand* command, const QByteArray& body) override;
signals:
    void packetReceived(AVPacket* pkt);

private:
    void initSdl();

    void streamOpen();
    void streamComponentOpen(int idx);
    void synchronize_video(AVFrame* frame, double* pts);

    void decode(AVPacket* pkt);

public slots:
    void setPlay(bool play);
    void setVideoParameters(WId widgetId /*const IModem::VideoData& videoData*/);

private:
    // SDL parameters
    SDL_Window*        screen { nullptr };
    WId                widgetId { 0 };
    SDL_Renderer*      renderer { nullptr };
    SDL_Texture*       texture { nullptr };
    struct SwsContext* imageConvertCtx { nullptr };

    bool   pictureReady { false };
    double picturePts { 0 };

    ThreadSafeVector<AVPacket*> videoPacketQueue;
    ThreadSafeVector<AVPacket*> audioPacketQueue;

    QByteArray audioArr;
    QMutex*    audioMutex;

    // ffmpeg parameters
    AVFormatContext* formatCtx { nullptr };
    //    std::string      path { "udp://127.0.0.1:7777" };
    std::string path { "D://work/_video/video.mp4" };
    //    std::string path { "D://work/_video/song.mp3" };

    double                                     maxFrameDuration { 0.0 };
    int                                        videoIdx { -1 };
    int                                        audioIdx { -1 };
    std::pair<AVCodecContext*, const AVCodec*> videoCodecParams { nullptr, nullptr };
    std::pair<AVCodecContext*, const AVCodec*> audioCodecParams { nullptr, nullptr };
    std::pair<double, double>                  clocks { 0, 0 }; // video/audio clock

    SDL_AudioSpec      wanted_spec;
    AVChannelLayout    chLayout;
    struct SwrContext* swr_ctx { nullptr };

    // threads
    QThread* recvThread { nullptr };
    QThread* videoDecodeThread { nullptr };

    ////////

    QThread* handlerThread { nullptr };

    // ffmpeg
    //    AVFormatContext* pFormatCtx { nullptr };
    //    int              i, videoindex;
    //    AVCodecContext*  pCodecCtx { nullptr };
    //    AVCodec*         pCodec { nullptr };
    //    AVFrame *        pFrame { nullptr }, *pFrameYUV { nullptr };
    AVFrame*       pFrameYUV { nullptr };
    unsigned char* out_buffer { nullptr };
    //    AVPacket*          packet { nullptr };
    //    int y_size;
    //    int ret, got_picture;
    //    struct SwsContext* img_convert_ctx { nullptr };

    char filepath[21] = "udp://127.0.0.1:7777";
    // SDL
    //    int screen_w = 0, screen_h = 0;

    SDL_Rect sdlRect;

    SDL_Rect presentRect { 0, 0, 0, 0 };

    //    QThread* videoThread { nullptr };

    QVector<AVPacket> packetVector;
    QMutex            packetMutex;
};

#endif // VIDEOCONTROLLER_H
