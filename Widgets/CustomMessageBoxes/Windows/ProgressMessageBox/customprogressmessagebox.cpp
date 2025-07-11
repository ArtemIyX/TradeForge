//
// Created by Stalker7274 on 10.07.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_customProgressMessageBox.h" resolved

#include "customprogressmessagebox.h"

#include <QLabel>

#include "customTitleBar.h"
#include "ui_customProgressMessageBox.h"


customProgressMessageBox::customProgressMessageBox(const QString &text, QWidget *parent) :
    QDialog(parent), ui(new Ui::customProgressMessageBox) {
    ui->setupUi(this);

    setWindowModality(Qt::WindowModal);
    setWindowFlags(Qt::FramelessWindowHint);
    customTitleBar* titleBar = new customTitleBar(this);
    ui->titleBarWidget->layout()->addWidget(titleBar);
    show();
    titleBar->setCallEventOnClose(true);
    connect(titleBar, &customTitleBar::onClose, this, &customProgressMessageBox::titleBarClose);

    QLayout *mainContentLayout = ui->mainContentWidget->layout();
    textLabel = new QLabel(text);
    textLabel->setWordWrap(true);
    textLabel->setStyleSheet("color: rgb(255, 255, 255);");

    mainContentLayout->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::Fixed));
    mainContentLayout->addWidget(textLabel);
    mainContentLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QWidget* widget = new QWidget();
    mainContentLayout->addWidget(widget);
}

customProgressMessageBox::~customProgressMessageBox() {
    delete ui;
}

void customProgressMessageBox::titleBarClose() {
}
