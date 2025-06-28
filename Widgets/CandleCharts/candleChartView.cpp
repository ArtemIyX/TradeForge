//
// Created by Stalker7274 on 28.06.2025.
//

#include "candleChartView.h"

#include <qdatetime.h>
#include <qdatetimeaxis.h>
#include <qtimer.h>

candleChartView::candleChartView(QWidget *parent) : QChartView(parent) {

}

void candleChartView::wheelEvent(QWheelEvent *event) {
    const double scaleFactor = 1.15;

    if (event->angleDelta().y() > 0) {
        // Масштабирование вверх (приближение)
        chart()->zoom(scaleFactor);
    } else {
        // Масштабирование вниз (отдаление)
        chart()->zoom(1.0 / scaleFactor);
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
