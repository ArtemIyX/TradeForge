//
// Created by Stalker7274 on 04.07.2025.
//

#ifndef TERMINAL_H
#define TERMINAL_H

#include <QWidget>
#include "messageHandler.h"

class QFileSystemWatcher;
class QTextEdit;
QT_BEGIN_NAMESPACE
namespace Ui { class terminal; }
QT_END_NAMESPACE

class terminal : public QWidget {
Q_OBJECT

public:
    explicit terminal(QWidget *parent = nullptr);
    ~terminal() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void updateLogFromFile(const QString &filePath) const;

private:
    Ui::terminal *ui;

    QTextEdit *textEdit;
    QFileSystemWatcher *fileWatcher = nullptr;

    const int RESIZE_MARGIN = 5;
    bool resizing = false;
    QPointF dragStartPos;
    Qt::Edges resizeEdges;

    [[nodiscard]] Qt::Edges edgesAt(const QPoint &pos) const;
    void updateCursorShape(const QPoint &pos);
};


#endif //TERMINAL_H
