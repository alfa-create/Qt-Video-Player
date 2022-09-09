#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>

class VideoWidget : public QWidget {
    Q_OBJECT
public:
    explicit VideoWidget(QWidget* parent = nullptr);

private:
    void resizeEvent(QResizeEvent* event) override;

signals:
    void resized();
};

#endif // VIDEOWIDGET_H
