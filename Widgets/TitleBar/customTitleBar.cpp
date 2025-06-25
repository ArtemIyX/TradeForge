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
    backgroundLayout->setContentsMargins(0, 0, 0, 0);

    closeButton = new QPushButton("X", this);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));

    backgroundLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    backgroundLayout->addWidget(closeButton);

    closeButton->setStyleSheet("background-color: rgb(255, 255, 255);");
}

customTitleBar::~customTitleBar() {

}

void customTitleBar::closeButtonClicked() {
    qobject_cast<QDialog*>(parentWindow)->deleteLater();
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
