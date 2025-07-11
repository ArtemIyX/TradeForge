//
// Created by Stalker7274 on 10.07.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_customAcceptMessageBox.h" resolved

#include "customacceptmessagebox.h"

#include <QLabel>

#include "customTitleBar.h"
#include "ui_customAcceptMessageBox.h"


customAcceptMessageBox::customAcceptMessageBox(QString text, QWidget *parent) :
    QDialog(parent), ui(new Ui::customAcceptMessageBox) {
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    customTitleBar* titleBar = new customTitleBar(this);
    ui->titleBarWidget->layout()->addWidget(titleBar);
	titleBar->setCallEventOnClose(true);
	connect(titleBar, &customTitleBar::onClose, this, &customAcceptMessageBox::titleBarClose);

    QLayout *mainContentLayout = ui->mainContentWidget->layout();
    textLabel = new QLabel(text);
    textLabel->setWordWrap(true);
    textLabel->setStyleSheet("color: rgb(255, 255, 255);");
    okButton = new QPushButton("Okay");
	cancelButton = new QPushButton("Cancel");

    const QString buttonsStyle = R"(
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
    )";

    okButton->setStyleSheet(buttonsStyle);
    connect(okButton, &QPushButton::clicked, [this]() {
        accept();
    });

	cancelButton->setStyleSheet(buttonsStyle);
	connect(cancelButton, &QPushButton::clicked, [this]() {
		reject();
	});

    mainContentLayout->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::Fixed));
    mainContentLayout->addWidget(textLabel);
    mainContentLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QWidget* widget = new QWidget();
    mainContentLayout->addWidget(widget);
    QHBoxLayout* horizontalBox = new QHBoxLayout();
    widget->setLayout(horizontalBox);
    horizontalBox->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	horizontalBox->addWidget(cancelButton);
    horizontalBox->addWidget(okButton);
}

customAcceptMessageBox::~customAcceptMessageBox() {
    delete ui;
}

void customAcceptMessageBox::titleBarClose() {
	reject();
}
