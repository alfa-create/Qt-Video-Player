#ifndef VIDEODECODER_H
#define VIDEODECODER_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include "videoparameters.h"
#include <QDebug>
#include <QObject>

class VideoDecoder : public QObject {
    Q_OBJECT
public:
    explicit VideoDecoder(VideoParameters* videoParameters, QObject* parent = nullptr);

public slots:
    void decode(AVPacket* pkt);

private:
    VideoParameters* videoParameters { nullptr };

signals:
};

#endif // VIDEODECODER_H
