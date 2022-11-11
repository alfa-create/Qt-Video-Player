#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ui/videoui.h>

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

private:
    Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
