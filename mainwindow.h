#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// sdl && ffmpeg
//#define __STDC_CONSTANT_MACROS
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libpostproc/postprocess.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
}
#undef main
#include <SDL.h>
#undef main
#include <QDebug>
#include <QMutex>
#include <QThread>

#include "videocontroller.h"
#include <exception>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void setPlay(bool play);
    void setVideoParameters(QWidget* videoWidget);

private:
    Ui::MainWindow* ui;

    QThread* handlerThread { nullptr };

    // ffmpeg
    AVFormatContext*   pFormatCtx { nullptr };
    int                i, videoindex;
    AVCodecContext*    pCodecCtx { nullptr };
    AVCodec*           pCodec { nullptr };
    AVFrame *          pFrame { nullptr }, *pFrameYUV { nullptr };
    unsigned char*     out_buffer { nullptr };
    AVPacket*          packet { nullptr };
    int                y_size;
    int                ret, got_picture { 0 };
    struct SwsContext* img_convert_ctx { nullptr };

    char filepath[21] = "udp://127.0.0.1:7777";
    // SDL
    int           screen_w = 0, screen_h = 0;
    SDL_Window*   screen { nullptr };
    SDL_Renderer* sdlRenderer { nullptr };
    SDL_Texture*  sdlTexture { nullptr };
    SDL_Rect      sdlRect;

    SDL_Rect presentRect { 0, 0, 0, 0 };

    QVector<AVPacket> packetVector;
    QMutex*           packetMutex { nullptr };

    QThread* videoThread { nullptr };
    QThread* sdlEventLoopThread { nullptr };
    QThread* readThread { nullptr };

    WId widgetId { 0 };
};
#endif // MAINWINDOW_H
