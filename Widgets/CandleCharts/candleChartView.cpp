//
// Created by Stalker7274 on 28.06.2025.
//

#include "candleChartView.h"

#include <qdatetime.h>
#include <qdatetimeaxis.h>
#include <qtimer.h>
#include <QValueAxis>

class QValueAxis;

candleChartView::candleChartView(QWidget *parent) : QChartView(parent) {

}

void candleChartView::wheelEvent(QWheelEvent *event) {
    const double scaleFactor = 1.15;

    QRectF plotArea = chart()->plotArea();
    QRectF yAxisArea(plotArea.left() - 50, plotArea.top(), 50, plotArea.height());

    QPointF cursorPos = event->position();
    if (yAxisArea.contains(cursorPos)) {
        QValueAxis *axisY = qobject_cast<QValueAxis*>(chart()->axisY());
        if (axisY) {
            qreal currentMin = axisY->min();
            qreal currentMax = axisY->max();
            qreal range = currentMax - currentMin;
            qreal center = (currentMax + currentMin) / 2;

            qreal scaleFactor = (event->angleDelta().y() > 0) ? 0.9 : 1.1;
            qreal newRange = range * scaleFactor;

            qreal newMin = center - newRange / 2;
            qreal newMax = center + newRange / 2;

            if (newRange > 0.1 && newRange < 1000) {
                axisY->setRange(newMin, newMax);
            }
        }
    }else {

        if (event->angleDelta().y() > 0) {
            chart()->zoom(scaleFactor);
        } else {
            chart()->zoom(1.0 / scaleFactor);
        }
    }

    event->accept();
}

void candleChartView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_lastPanPoint = event->pos();
        m_isDragging = true;
        setCursor(Qt::ClosedHandCursor);
    }
    QChartView::mousePressEvent(event);
}

void candleChartView::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint delta = event->pos() - m_lastPanPoint;
        chart()->scroll(-delta.x(), delta.y());
        m_lastPanPoint = event->pos();
    }
    QChartView::mouseMoveEvent(event);
}

void candleChartView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
    }
    QChartView::mouseReleaseEvent(event);
}
