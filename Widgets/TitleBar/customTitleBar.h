//
// Created by Stalker7274 on 25.06.2025.
//

#ifndef CUSTOMTITLEBAR_H
#define CUSTOMTITLEBAR_H
#include <QPushButton>
#include <QWidget>


class QHBoxLayout;

class customTitleBar: public QWidget {
    Q_OBJECT
public:
    explicit customTitleBar(QWidget *parent = nullptr);
    ~customTitleBar() override;

public slots:

    void closeButtonClicked() const;
    void minimizeButtonClicked() const;

protected:

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

private:

    QPoint dragPosition;
    bool bDragging = false;
    QWidget* parentWindow;

    QWidget *background = nullptr;
    QHBoxLayout *backgroundLayout = nullptr;

    QPushButton *closeButton = nullptr;
    QPushButton *minimizeButton = nullptr;
};



#endif //CUSTOMTITLEBAR_H
