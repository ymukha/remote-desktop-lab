#pragma once

#include <QWidget>
#include <QImage>

class FrameView : public QWidget
{
    Q_OBJECT

public:
    explicit FrameView(QWidget *parent = nullptr);

public slots:
    void setFrame(const QImage& frame);

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

    QImage m_Frame;
};