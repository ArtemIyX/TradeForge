//
// Created by Stalker7274 on 30.06.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_downloadCSVWindow.h" resolved

#include "downloadcsvwindow.h"
#include "ui_downloadCSVWindow.h"
#include "../../Subsystems/historicaldataManager.h"


downloadCSVWindow::downloadCSVWindow(QString Path, QWidget *parent) :
    QDialog(parent), ui(new Ui::downloadCSVWindow) {
    ui->setupUi(this);

    symbolPath = Path;

    connect(ui->cancelButton, &QPushButton::clicked, this, &downloadCSVWindow::cancelClicked);
    connect(ui->acceptButton, &QPushButton::clicked, this, &downloadCSVWindow::acceptClicked);
}

downloadCSVWindow::~downloadCSVWindow() {
    delete ui;
}

void downloadCSVWindow::acceptClicked(){
    dataManager::instance()->downloadYahooFinanceData(ui->symbolNameLineEdit->text(),
        ui->startDateEdit->date(), ui->endDateEdit->date(),  symbolPath);
    deleteLater();
}

void downloadCSVWindow::cancelClicked() {
    deleteLater();
}
