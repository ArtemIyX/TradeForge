//
// Created by Stalker7274 on 04.07.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_terminal.h" resolved

#include "terminal.h"

#include <qfilesystemwatcher.h>
#include <QMouseEvent>
#include <qtextedit.h>
#include <QVBoxLayout>

#include "customTitleBar.h"
#include "ui_terminal.h"

terminal::terminal(QWidget *parent) :
    QWidget(parent), ui(new Ui::terminal) {
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    QWidget* titleBar = new customTitleBar(this);
    ui->titleBarWidget->layout()->addWidget(titleBar);

    resize(600, 400);
    setStyleSheet("background-color: #353535;");

    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    globalTextEdit = textEdit;
    textEdit->setStyleSheet("background-color: transparent; color: white;");

    ui->terminalMainContentWidget->layout()->addWidget(textEdit);

    fileWatcher = new QFileSystemWatcher(this);
    fileWatcher->addPath(logPath);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &terminal::updateLogFromFile);
    updateLogFromFile(logPath);
}

terminal::~terminal() {
    globalTextEdit = nullptr;
    delete ui;
}

void terminal::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        resizeEdges = edgesAt(event->pos());
        if (resizeEdges != Qt::Edges()) {
            resizing = true;
            dragStartPos = event->globalPosition();
            event->accept();
        }
    }
}

void terminal::mouseMoveEvent(QMouseEvent *event) {
    if (resizing) {
        const QPointF delta = event->globalPosition() - dragStartPos;
        QRect geom = geometry();

        if (resizeEdges.testFlag(Qt::LeftEdge))
            geom.setLeft(geom.left() + static_cast<int>(delta.x()));
        if (resizeEdges.testFlag(Qt::RightEdge))
            geom.setRight(geom.right() + static_cast<int>(delta.x()));
        if (resizeEdges.testFlag(Qt::TopEdge))
            geom.setTop(geom.top() + static_cast<int>(delta.y()));
        if (resizeEdges.testFlag(Qt::BottomEdge))
            geom.setBottom(geom.bottom() + static_cast<int>(delta.y()));

        setGeometry(geom);
        dragStartPos = event->globalPosition();
    } else {
        updateCursorShape(event->pos());
    }
}

void terminal::mouseReleaseEvent(QMouseEvent *event) {
    resizing = false;
}

Qt::Edges terminal::edgesAt(const QPoint &pos) const {
    Qt::Edges edges;
    if (pos.x() <= RESIZE_MARGIN)
        edges |= Qt::LeftEdge;
    if (pos.x() >= width() - RESIZE_MARGIN)
        edges |= Qt::RightEdge;
    if (pos.y() <= RESIZE_MARGIN)
        edges |= Qt::TopEdge;
    if (pos.y() >= height() - RESIZE_MARGIN)
        edges |= Qt::BottomEdge;
    return edges;
}

void terminal::updateCursorShape(const QPoint &pos) {
    const Qt::Edges edges = edgesAt(pos);
    if (edges == (Qt::LeftEdge | Qt::TopEdge) || edges == (Qt::RightEdge | Qt::BottomEdge))
        setCursor(Qt::SizeFDiagCursor);
    else if (edges == (Qt::RightEdge | Qt::TopEdge) || edges == (Qt::LeftEdge | Qt::BottomEdge))
        setCursor(Qt::SizeBDiagCursor);
    else if (edges.testFlag(Qt::LeftEdge) || edges.testFlag(Qt::RightEdge))
        setCursor(Qt::SizeHorCursor);
    else if (edges.testFlag(Qt::TopEdge) || edges.testFlag(Qt::BottomEdge))
        setCursor(Qt::SizeVerCursor);
    else
        setCursor(Qt::ArrowCursor);
}

void terminal::updateLogFromFile(const QString &filePath) const {

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        textEdit->setText(in.readAll());
        file.close();
    } else {
        qWarning() << "terminal::updateLogFromFile: unable to open log file " << filePath << ":" << file.errorString();
    }
}
