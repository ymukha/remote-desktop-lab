#include "frame_view.h"

#include <QPainter>

FrameView::FrameView(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(640, 360);
}

void FrameView::setFrame(const QImage& frame)
{
    m_Frame = frame;

    update();
}

void FrameView::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    p.fillRect(rect(), Qt::black);

    if (m_Frame.isNull())
        return;

    const QRect targetRect = rect();
    const QSize frameSize = m_Frame.size();
    const QSize scaledSize = frameSize.scaled(targetRect.size(), Qt::KeepAspectRatio);

    const int x = (targetRect.width() - scaledSize.width()) / 2;
    const int y = (targetRect.height() - scaledSize.height()) / 2;

    p.drawImage(QRect(QPoint(x, y), scaledSize), m_Frame);
}

QSize FrameView::sizeHint() const
{
    return !m_Frame.isNull()
            ? m_Frame.size()
            : QSize(640, 360);
}