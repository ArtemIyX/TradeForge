//
// Created by Stalker7274 on 28.06.2025.
//

#ifndef CANDLECHARTVIEW_H
#define CANDLECHARTVIEW_H

#include <QtCharts/QChartView>

class candleAdaptiveSeries;

class candleChartView : public QChartView
{
    Q_OBJECT
public:
    explicit candleChartView(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_isDragging;
    QPoint m_lastPanPoint;
};



#endif //CANDLECHARTVIEW_H
