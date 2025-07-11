//
// Created by Stalker7274 on 10.07.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_customInfoMessageBox.h" resolved

#include "custominfomessagebox.h"

#include <QLabel>

#include "customTitleBar.h"
#include "ui_customInfoMessageBox.h"


customInfoMessageBox::customInfoMessageBox(QString text, QWidget *parent) :
    QDialog(parent), ui(new Ui::customInfoMessageBox) {
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    customTitleBar* titleBar = new customTitleBar(this);
    ui->titleBarWidget->layout()->addWidget(titleBar);
	titleBar->setCallEventOnClose(true);
	connect(titleBar, &customTitleBar::onClose, this, &customInfoMessageBox::titleBarClose);

    QLayout *mainContentLayout = ui->mainContentWidget->layout();
    textLabel = new QLabel(text);
	textLabel->setWordWrap(true);
    textLabel->setStyleSheet("color: rgb(255, 255, 255);");
    okButton = new QPushButton("Okay");
    okButton->setStyleSheet(R"(
        QPushButton{
			color: #fff;
			border: 1px solid rgb(102, 102, 102);
			background-color: rgb(58, 58, 58);
			padding: 3px 2px;
		}
		QPushButton::hover{
		    color: #CCCCCC;
			background-color: #666666;
			padding: 3px 2px;
		}
		QPushButton::disabled{
			color: #CCCCCC;
			background-color: #666666;
			padding: 3px 2px;
		}
		QPushButton::pressed {
			background-color: #ffffff;
			padding: 3px 2px;
		}
    )");
    connect(okButton, &QPushButton::clicked, [this]() {
        accept();
    });

    mainContentLayout->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::Fixed));
    mainContentLayout->addWidget(textLabel);
    mainContentLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QWidget* widget = new QWidget();
    mainContentLayout->addWidget(widget);
    QHBoxLayout* horizontalBox = new QHBoxLayout();
    widget->setLayout(horizontalBox);
    horizontalBox->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    horizontalBox->addWidget(okButton);
}

customInfoMessageBox::~customInfoMessageBox() {
    delete ui;
}

void customInfoMessageBox::titleBarClose() {
	accept();
}
