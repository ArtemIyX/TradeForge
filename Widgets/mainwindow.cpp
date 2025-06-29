#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "HistoricalWindow/historicalwindow.h"
#include "Subsystems/historicalDataManager.h"

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

            connect(historicalMenuWidget, Q_SIGNAL(&QObject::destroyed), [this]() {

                disconnect(historicalMenuWidget, nullptr, this, nullptr);

                historicalMenuWidget = nullptr;
            });

        }else {

            qDebug() << historicalMenuWidget->objectName();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
