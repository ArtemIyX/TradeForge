#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "HistoricalWindow/historicalwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenu *fileMenu = ui->menubar->addMenu("File");
    fileMenu->addAction("Historical data", [this]() {

        if (!historicalMenuWidget) {

            historicalMenuWidget = new historicalWindow();

            historicalMenuWidget->show();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
