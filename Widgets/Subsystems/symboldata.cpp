//
// Created by Stalker7274 on 04.07.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_symbolData.h" resolved#include "symboldata.h"
#include <QFile>
#include <qtconcurrentrun.h>

#include "symbolData.h"
#include "SymbolStructs.cuh"


symbolData::symbolData(QString path, QObject *parent) :
    QObject(parent) {

    filePath = path;
    startLoading();
}

symbolData::~symbolData() {

}

void symbolData::startLoading() {

    QtConcurrent::run([this]()
    {
        QFile historicalData(filePath);

        if (!historicalData.open(QIODevice::ReadOnly)) {
            qDebug() << "historicalWindow::folderItemSelected read failed: " << historicalData.errorString();
            return;
        }

        while (!historicalData.atEnd()) {

            historicalCSVStroke stroke{};
            const qint64 bytesRead = historicalData.read(reinterpret_cast<char*>(&stroke), sizeof(historicalCSVStroke));
            if (bytesRead == sizeof(historicalCSVStroke)) {

                if (!stroke.isValid()) {
                    qDebug() << "Found not valid stroke or it first stroke";
                    continue;
                }

                data.append(stroke);
                QMetaObject::invokeMethod(this, [stroke, this]{

                    emit strokeLoaded(stroke);
                }, Qt::QueuedConnection);

                QThread::msleep(10);
            } else {

                qWarning() << "empty file or damaged file";
                break;
            }
        }

    historicalData.close();
    });
}
