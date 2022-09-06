#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include "threadsafevector.h"
#include "videodecoder.h"
#include "videoparameters.h"
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

public slots:
    void setPlay(bool play);
    void setVideoParameters(WId widgetId /*const IModem::VideoData& videoData*/);

private:
    VideoParameters* videoParameters { nullptr };
    VideoDecoder*    videoDecoder { nullptr };
    // threads
    QThread* recvThread { nullptr };
    QThread* videoDecoderThread { nullptr };

    ////////

    QThread* handlerThread { nullptr };

    // ffmpeg
    //    AVFrame*       pFrameYUV { nullptr };
    //    unsigned char* out_buffer { nullptr };

    //    SDL_Rect sdlRect;

    //    SDL_Rect presentRect { 0, 0, 0, 0 };

    //    QVector<AVPacket> packetVector;
    //    QMutex            packetMutex;
};

#endif // VIDEOCONTROLLER_H
