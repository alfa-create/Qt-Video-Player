#include "videowidget.h"

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget { parent }
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_UpdatesDisabled);
    setUpdatesEnabled(false);
}

void VideoWidget::resizeEvent(QResizeEvent* event)
{
    //    emit resized();
    //    QWidget::resizeEvent(event);
}
