#include "videowidget.h"

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget { parent }
{
}

void VideoWidget::resizeEvent(QResizeEvent* /*event*/)
{
    emit resized();
}
