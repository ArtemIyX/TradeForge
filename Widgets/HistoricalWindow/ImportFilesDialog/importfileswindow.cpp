//
// Created by Stalker7274 on 29.06.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_importFilesWIndow.h" resolved

#include "importfileswindow.h"

#include <QFileDialog>
#include "filetoimport.h"
#include "SymbolStructs.cuh"
#include "ui_importFilesWIndow.h"
#include "../../Subsystems/historicalDataManager.h"
#include "customTitleBar.h"

importFilesWIndow::importFilesWIndow(QWidget *parent) :
    QDialog(parent), ui(new Ui::importFilesWIndow) {
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    QWidget* titleBar = new customTitleBar(this);
    ui->titleBarWidget->layout()->addWidget(titleBar);

    connect(ui->addFilesToImportButton, Q_SIGNAL(&QPushButton::clicked), this, &importFilesWIndow::addFilesToImport);
    connect(ui->startImportButton, Q_SIGNAL(&QPushButton::clicked), this, &importFilesWIndow::startImport);
    connect(ui->cancelImportButton, Q_SIGNAL(&QPushButton::clicked), this, &importFilesWIndow::cancelImport);

    connect(ui->filesSettingsTable, &QTableWidget::itemChanged, this, &importFilesWIndow::settingsTableChanged);

    filesToImportScrollLayout = new QVBoxLayout();
    ui->filesToImportScroll->setLayout(filesToImportScrollLayout);

    currentFilesSettings = {{"ticker", "EURUSD"},
        {"description", "EUR/USD"},
        {"contract_size", "1"},
        {"units", "Share(s)"},
        {"min_volume", 1},
        {"max_volume", 100000000},
        {"volume_step", 0.01},
        {"min_tick", 0.00001},
        {"leverage", 0.05},
        {"trade_mode", "full"}
    };

    ui->filesSettingsTable->setRowCount(currentFilesSettings.size());
    ui->filesSettingsTable->setColumnCount(2);

    for (int i = 0; i < currentFilesSettings.size(); i++) {

        ui->filesSettingsTable->setItem(i, 0, new QTableWidgetItem(currentFilesSettings[i].settingName));
        ui->filesSettingsTable->setItem(i, 1, new QTableWidgetItem(currentFilesSettings[i].settingValue.toString()));

        ui->filesSettingsTable->item(i, 0)->setFlags(ui->filesSettingsTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
    }
}

importFilesWIndow::~importFilesWIndow() {
    delete ui;
}

void importFilesWIndow::addFilesToImport(bool checked) {
    QList<QString> files = QFileDialog::getOpenFileNames(
        this,
        "Выберите файлы для импорта",
        "",
        "CSV таблица (*.csv*)"
    );

    QLayoutItem* item = filesToImportScrollLayout->itemAt(filesToImportScrollLayout->count() - 1);
    if (item && item->spacerItem()) {
        delete filesToImportScrollLayout->takeAt(filesToImportScrollLayout->count() - 1);
    }

    for (QString file : files) {

        if (currentFiles.contains(file)) continue;

        fileToImport* widget = new fileToImport(file, this);

        filesToImportScrollLayout->addWidget(widget);
        currentFiles.append(file);
        connect(widget, &fileToImport::preDestroy, this, &importFilesWIndow::importFileDeleted);
    }

    filesToImportScrollLayout->addStretch();
}

void importFilesWIndow::importFileDeleted(const fileToImport* widget) {

    currentFiles.removeAll(widget->filePath);
}

void importFilesWIndow::settingsTableChanged(const QTableWidgetItem *item) {

    if (item->column() == 1) {
        const int row = item->row();
        if (row >= 0 && row < currentFilesSettings.size()) {

            currentFilesSettings[row].settingValue = item->text();
        }
    }
}

void importFilesWIndow::startImport(bool checked) {

    dataManager::instance()->importFiles(currentFiles, currentFolder, currentFilesSettings);
    deleteLater();
}

void importFilesWIndow::cancelImport(bool checked) {

    deleteLater();
}
