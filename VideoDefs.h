//#ifndef VIDEODEFS_H
//#define VIDEODEFS_H

//extern "C" {
//#include "libavcodec/avcodec.h"
//#include "libavcodec/avfft.h"
//#include "libavcodec/packet.h"
//#include "libavdevice/avdevice.h"
//#include "libavfilter/avfilter.h"
//#include "libavformat/avformat.h"
//#include "libavutil/avstring.h"
//#include "libavutil/avutil.h"
//#include "libavutil/bprint.h"
//#include "libavutil/channel_layout.h"
//#include "libavutil/dict.h"
//#include "libavutil/error.h"
//#include "libavutil/eval.h"
//#include "libavutil/fifo.h"
//#include "libavutil/imgutils.h"
//#include "libavutil/mathematics.h"
//#include "libavutil/opt.h"
//#include "libavutil/parseutils.h"
//#include "libavutil/pixdesc.h"
//#include "libavutil/samplefmt.h"
//#include "libavutil/time.h"
//#include "libswresample/swresample.h"
//#include "libswscale/swscale.h"

//#include <SDL.h>
//#undef main
//}

//#define MAX_QUEUE_SIZE            (15 * 1024 * 1024)
//#define MIN_FRAMES                25
//#define EXTERNAL_CLOCK_MIN_FRAMES 2
//#define EXTERNAL_CLOCK_MAX_FRAMES 10

///* Minimum SDL audio buffer size, in samples. */
//#define SDL_AUDIO_MIN_BUFFER_SIZE 512
///* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
//#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

///* Step size for volume control in dB */
//#define SDL_VOLUME_STEP (0.75)

///* no AV sync correction is done if below the minimum AV sync threshold */
//#define AV_SYNC_THRESHOLD_MIN 0.04
///* AV sync correction is done if above the maximum AV sync threshold */
//#define AV_SYNC_THRESHOLD_MAX 0.1
///* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
//#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
///* no AV correction is done if too big error */
//#define AV_NOSYNC_THRESHOLD 10.0

///* maximum audio speed change to get correct sync */
//#define SAMPLE_CORRECTION_PERCENT_MAX 10

///* external clock speed adjustment constants for realtime sources based on buffer fullness */
//#define EXTERNAL_CLOCK_SPEED_MIN  0.900
//#define EXTERNAL_CLOCK_SPEED_MAX  1.010
//#define EXTERNAL_CLOCK_SPEED_STEP 0.001

///* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
//#define AUDIO_DIFF_AVG_NB 20

///* polls for possible required screen refresh at least this often, should be less than 1/fps */
//#define REFRESH_RATE 0.01

///* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
///* TODO: We assume that a decoded and resampled frame fits into this buffer */
//#define SAMPLE_ARRAY_SIZE (8 * 65536)

//#define CURSOR_HIDE_DELAY 1000000

//#define USE_ONEPASS_SUBTITLE_RENDER 1

//#define VIDEO_PICTURE_QUEUE_SIZE 3
//#define SUBPICTURE_QUEUE_SIZE    16
//#define SAMPLE_QUEUE_SIZE        9
//#define FRAME_QUEUE_SIZE         FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

//#define decoder_reorder_pts -1
//#define video_disable       0
//#define audio_disable       0
//#define subtitle_disable    0
//#define infinite_buffer     1
//#define autoexit            1
//#define FF_QUIT_EVENT       (SDL_USEREVENT + 2)
//#define framedrop           -1
//#define display_disable     0
//#define show_status         1

//enum ShowMode {
//    SHOW_MODE_NONE  = -1,
//    SHOW_MODE_VIDEO = 0,
//    SHOW_MODE_WAVES,
//    SHOW_MODE_RDFT,
//    SHOW_MODE_NB,
//};

//enum {
//    AV_SYNC_AUDIO_MASTER, /* default choice */
//    AV_SYNC_VIDEO_MASTER,
//    AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
//};

//typedef struct AudioParams {
//    int                 freq;
//    AVChannelLayout     ch_layout;
//    enum AVSampleFormat fmt;
//    int                 frame_size;
//    int                 bytes_per_sec;
//} AudioParams;

//typedef struct Clock {
//    double pts;       /* clock base */
//    double pts_drift; /* clock base minus time at which we updated the clock */
//    double last_updated;
//    double speed;
//    int    serial; /* clock is based on a packet with this serial */
//    int    paused;
//    int*   queue_serial; /* pointer to the current packet queue serial, used for obsolete clock detection */
//} Clock;

//typedef struct MyAVPacketList {
//    AVPacket* pkt;
//    int       serial;
//} MyAVPacketList;

///* Common struct for handling all types of decoded data and allocated render buffers. */
//typedef struct Frame {
//    AVFrame*   frame;
//    AVSubtitle sub;
//    int        serial;
//    double     pts;      /* presentation timestamp for the frame */
//    double     duration; /* estimated duration of the frame */
//    int64_t    pos;      /* byte position of the frame in the input file */
//    int        width;
//    int        height;
//    int        format;
//    AVRational sar;
//    int        uploaded;
//    int        flip_v;
//} Frame;

/////* options specified by the user */
////static const AVInputFormat* file_iformat;
//////static const char*          input_filename = "udp://127.0.0.1:7777";
//////static const char*   input_filename = "D://work/_video/video.mp4";
////static const char*   input_filename = "rtsp://192.168.0.124:554/stream1";
////static const char*   window_title;
////static int           default_width  = 640;
////static int           default_height = 480;
////static int           screen_width   = 0;
////static int           screen_height  = 0;
////static int           screen_left    = SDL_WINDOWPOS_CENTERED;
////static int           screen_top     = SDL_WINDOWPOS_CENTERED;
////static int           audio_disable;
////static int           video_disable;
////static int           subtitle_disable;
////static const char*   wanted_stream_spec[AVMEDIA_TYPE_NB] = { 0 };
////static int           seek_by_bytes                       = -1;
////static float         seek_interval                       = 10;
////static int           display_disable;
////static int           borderless;
////static int           alwaysontop;
////static int           startup_volume      = 100;
////static int           show_status         = -1;
////static int           av_sync_type        = AV_SYNC_AUDIO_MASTER;
////static int64_t       start_time          = AV_NOPTS_VALUE;
////static int64_t       duration            = AV_NOPTS_VALUE;
////static int           fast                = 0;
////static int           genpts              = 0;
////static int           lowres              = 0;
////static int decoder_reorder_pts = -1;
////static int           autoexit;
////static int           exit_on_keydown;
////static int           exit_on_mousedown;
////static int           loop            = 1;
////static int           framedrop       = -1;
////static int           infinite_buffer = -1;
////static enum ShowMode show_mode       = SHOW_MODE_NONE;
////static const char*   audio_codec_name;
////static const char*   subtitle_codec_name;
////static const char*   video_codec_name;
////double               rdftspeed = 0.02;
////static int64_t       cursor_last_shown;
////static int           cursor_hidden    = 0;
////static int           autorotate       = 1;
////static int           find_stream_info = 1;
////static int           filter_nbthreads = 0;

/////* current context */
////static int     is_full_screen;
////static int64_t audio_callback_time;

//double get_clock(Clock* c);
//void   set_clock_at(Clock* c, double pts, int serial, double time);
//void   set_clock(Clock* c, double pts, int serial);
//void   set_clock_speed(Clock* c, double speed);
//void   init_clock(Clock* c, int* queue_serial);
//void   sync_clock_to_slave(Clock* c, Clock* slave);

//#endif // VIDEODEFS_H
