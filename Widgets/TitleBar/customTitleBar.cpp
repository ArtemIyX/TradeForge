//
// Created by Stalker7274 on 25.06.2025.
//

#include "customTitleBar.h"

#include <QDialog>
#include <QMouseEvent>
#include <qlayout.h>

customTitleBar::customTitleBar(QWidget *parent) : QWidget(parent) {

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(30);

    parentWindow = parent;

    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);

    background = new QWidget(this);
    layout()->addWidget(background);
    background->setLayout(backgroundLayout = new QHBoxLayout(this));
    background->setStyleSheet("background-color: #3C3C3C;");
    backgroundLayout->setContentsMargins(0, 0, 3, 0);

    minimizeButton = new QPushButton("-", this);
    connect(minimizeButton, SIGNAL(clicked()), this, SLOT(minimizeButtonClicked()));
    closeButton = new QPushButton("X", this);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));

    minimizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    minimizeButton->setFixedSize(55,25);
    closeButton->setFixedSize(55,25);

    backgroundLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    backgroundLayout->addWidget(minimizeButton);
    backgroundLayout->addItem(new QSpacerItem(0, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));
    backgroundLayout->addWidget(closeButton);

    const QString buttonStyle = R"(

        QPushButton {
            background-color: #404040;
            border: 1px solid #555555;
            border-radius: 4px;
            color: #ffffff;
            font-size: 14px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #3a3a3a;
            border: 1px solid #505050;
            color: #ffffff;

        }

        QPushButton:pressed {
            background-color: #303030;
            border: 1px solid #757575;
            color: #cccccc;
        }

        QPushButton:focus {
            outline: none;
            border: 1px solid #555555;
        }

        QPushButton:disabled {
            background-color: #303030;
            border: 1px solid #202020;
            color: #666666;
        }
    )";

    minimizeButton->setStyleSheet(buttonStyle);
    closeButton->setStyleSheet(buttonStyle);
}

customTitleBar::~customTitleBar() {
}

void customTitleBar::setCallEventOnClose(bool newCallEventOnClose) {
    callEventOnClose = newCallEventOnClose;
}

void customTitleBar::closeButtonClicked() {
    if (callEventOnClose) {
        emit onClose();
    }else {
        parentWindow->deleteLater();
    }
}

void customTitleBar::minimizeButtonClicked() const {
    this->window()->showMinimized();
}

void customTitleBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPosition().toPoint() - parentWindow->frameGeometry().topLeft();
        bDragging = true;
    }
}

void customTitleBar::mouseMoveEvent(QMouseEvent *event) {
    if (bDragging && (event->buttons() & Qt::LeftButton)) {
        parentWindow->move(event->globalPosition().toPoint() - dragPosition);
    }
}

void customTitleBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        bDragging = false;
    }
}
