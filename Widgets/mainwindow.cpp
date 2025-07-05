#include "mainwindow.h"

#include <qevent.h>

#include "ui_mainwindow.h"
#include "HistoricalWindow/historicalwindow.h"
#include "Subsystems/historicalDataManager.h"
#include "Terminal/terminal.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    subsystems.append(dataManager::instance());

    QMenu *fileMenu = ui->menubar->addMenu("File");
    fileMenu->addAction("Historical data", [this]() {

        if (!historicalMenuWidget) {

            historicalMenuWidget = new historicalWindow();
            historicalMenuWidget->show();
        }
    });

    fileMenu->addAction("Terminal", [this]() {

        if (!terminalWidget) {

            terminalWidget = new terminal();
            terminalWidget->show();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;

    for (const QObject *system : subsystems) {
        delete system;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    createLogBackup();
    event->accept();
}
