#ifndef VIDEOHANDLERS_H
#define VIDEOHANDLERS_H

#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavcodec/avfft.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavutil/avstring.h"
#include "libavutil/bprint.h"
#include "libavutil/channel_layout.h"
#include "libavutil/dict.h"
#include "libavutil/eval.h"
#include "libavutil/fifo.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/opt.h"
#include "libavutil/parseutils.h"
#include "libavutil/pixdesc.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/parseutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>

//#define SDL_MAIN_HANDLED
#undef main
#include <SDL.h>
#undef main
}

#include "SDL_render.h"
#include "SDL_video.h"
#include "libavformat/avformat.h"

#include <QMutex>

#define MAX_QUEUE_SIZE            (15 * 1024 * 1024)
#define MIN_FRAMES                25
#define EXTERNAL_CLOCK_MIN_FRAMES 2
#define EXTERNAL_CLOCK_MAX_FRAMES 10

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

/* Step size for volume control in dB */
#define SDL_VOLUME_STEP (0.75)

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB 20

/* polls for possible required screen refresh at least this often, should be less than 1/fps */
#define REFRESH_RATE 0.01

/* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
/* TODO: We assume that a decoded and resampled frame fits into this buffer */
#define SAMPLE_ARRAY_SIZE (8 * 65536)

#define CURSOR_HIDE_DELAY 1000000

#define USE_ONEPASS_SUBTITLE_RENDER 1

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE    16
#define SAMPLE_QUEUE_SIZE        9
#define FRAME_QUEUE_SIZE         FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

typedef struct MyAVPacketList {
    AVPacket* pkt;
    int       serial;
} MyAVPacketList;

typedef struct PacketQueue {
    AVFifo*    pkt_list;
    int        nb_packets;
    int        size;
    int64_t    duration;
    int        abort_request;
    int        serial;
    SDL_mutex* mutex;
    SDL_cond*  cond;
} PacketQueue;

typedef struct AudioParams {
    int                 freq;
    AVChannelLayout     ch_layout;
    enum AVSampleFormat fmt;
    int                 frame_size;
    int                 bytes_per_sec;
} AudioParams;

typedef struct Clock {
    double pts;       /* clock base */
    double pts_drift; /* clock base minus time at which we updated the clock */
    double last_updated;
    double speed;
    int    serial; /* clock is based on a packet with this serial */
    int    paused;
    int*   queue_serial; /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;

/* Common struct for handling all types of decoded data and allocated render buffers. */
typedef struct Frame {
    AVFrame*   frame;
    AVSubtitle sub;
    int        serial;
    double     pts;      /* presentation timestamp for the frame */
    double     duration; /* estimated duration of the frame */
    int64_t    pos;      /* byte position of the frame in the input file */
    int        width;
    int        height;
    int        format;
    AVRational sar;
    int        uploaded;
    int        flip_v;
} Frame;

enum ShowMode {
    SHOW_MODE_NONE  = -1,
    SHOW_MODE_VIDEO = 0,
    SHOW_MODE_WAVES,
    SHOW_MODE_RDFT,
    SHOW_MODE_NB,
};

typedef struct FrameQueue {
    Frame        queue[FRAME_QUEUE_SIZE];
    int          rindex;
    int          windex;
    int          size;
    int          max_size;
    int          keep_last;
    int          rindex_shown;
    SDL_mutex*   mutex;
    SDL_cond*    cond;
    PacketQueue* pktq;
} FrameQueue;

enum {
    AV_SYNC_AUDIO_MASTER, /* default choice */
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

typedef struct Decoder {
    AVPacket*       pkt;
    PacketQueue*    queue;
    AVCodecContext* avctx;
    int             pkt_serial;
    int             finished;
    int             packet_pending;
    SDL_cond*       empty_queue_cond;
    int64_t         start_pts;
    AVRational      start_pts_tb;
    int64_t         next_pts;
    AVRational      next_pts_tb;
    SDL_Thread*     decoder_tid;
} Decoder;

typedef struct VideoState {
    SDL_Thread*          read_tid;
    const AVInputFormat* iformat;
    int                  abort_request;
    int                  force_refresh;
    int                  paused;
    int                  last_paused;
    int                  queue_attachments_req;
    int                  seek_req;
    int                  seek_flags;
    int64_t              seek_pos;
    int64_t              seek_rel;
    int                  read_pause_return;
    AVFormatContext*     ic;
    int                  realtime;

    Clock audclk;
    Clock vidclk;
    Clock extclk;

    FrameQueue pictq;
    FrameQueue subpq;
    FrameQueue sampq;

    Decoder auddec;
    Decoder viddec;
    Decoder subdec;

    int audio_stream;

    int av_sync_type;

    double             audio_clock;
    int                audio_clock_serial;
    double             audio_diff_cum; /* used for AV difference average computation */
    double             audio_diff_avg_coef;
    double             audio_diff_threshold;
    int                audio_diff_avg_count;
    AVStream*          audio_st;
    PacketQueue        audioq;
    int                audio_hw_buf_size;
    uint8_t*           audio_buf;
    uint8_t*           audio_buf1;
    unsigned int       audio_buf_size; /* in bytes */
    unsigned int       audio_buf1_size;
    int                audio_buf_index; /* in bytes */
    int                audio_write_buf_size;
    int                audio_volume;
    int                muted;
    struct AudioParams audio_src;
#if CONFIG_AVFILTER
    struct AudioParams audio_filter_src;
#endif
    struct AudioParams audio_tgt;
    struct SwrContext* swr_ctx;
    int                frame_drops_early;
    int                frame_drops_late;

    ShowMode show_mode;

    int16_t      sample_array[SAMPLE_ARRAY_SIZE];
    int          sample_array_index;
    int          last_i_start;
    RDFTContext* rdft;
    int          rdft_bits;
    FFTSample*   rdft_data;
    int          xpos;
    double       last_vis_time;
    SDL_Texture* vis_texture;
    SDL_Texture* sub_texture;
    SDL_Texture* vid_texture;

    int         subtitle_stream;
    AVStream*   subtitle_st;
    PacketQueue subtitleq;

    double             frame_timer;
    double             frame_last_returned_time;
    double             frame_last_filter_delay;
    int                video_stream;
    AVStream*          video_st;
    PacketQueue        videoq;
    double             max_frame_duration; // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity
    struct SwsContext* img_convert_ctx;
    struct SwsContext* sub_convert_ctx;
    int                eof;

    char* filename;
    int   width, height, xleft, ytop;
    int   step;

    int last_video_stream, last_audio_stream, last_subtitle_stream;

    SDL_cond* continue_read_thread;

    bool    runEventLoop;
    QMutex* isLockerMutex { new QMutex };

} VideoState;

struct VideoState;
union SDL_Event;

int         read_thread(void* arg);
void        refresh_loop_wait_event(VideoState* is, SDL_Event* event);
VideoState* stream_open(const char* filename,
    SDL_Window* _window, SDL_Renderer* _renderer);

void event_loop(VideoState* cur_stream);
void do_exit(VideoState* is);

#endif // VIDEOHANDLERS_H
