//
// Created by Stalker7274 on 29.06.2025.
//

#include "historicaldataManager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <QTableWidget>
#include <QApplication>
#include <qfuture.h>
#include <qgraphicsitem.h>
#include <QtConcurrent>
#include <QMessageBox>
#include <qprocess.h>
#include "e_HistoricalWindowItemData.h"
#include "symboldata.h"
#include "../CustomMessageBoxes/messageBoxFactory.h"

dataManager* dataManager::m_instance = nullptr;

dataManager* dataManager::instance() {
    if (!m_instance) {
        m_instance = new dataManager();
    }
    return m_instance;
}

dataManager::dataManager(QObject* parent) : QObject(parent), treeModel(new QStandardItemModel(this)) {
    treeModel->setHorizontalHeaderLabels({"Tree view"});
    loadTreeViewItems();
}

dataManager::~dataManager() {
    delete treeModel;
}

bool dataManager::createFolder(const QString& parentPath, const QString& folderName) {
    const QString folderPath = parentPath + "/" + folderName;
    const QDir dir(folderPath);
    if (dir.exists()) {
        return false;
    }
    if (!dir.mkpath(folderPath)) {
        return false;
    }

    const QString relativePath = QDir(dataFolder).relativeFilePath(folderPath);
    QStringList parts = relativePath.split("/", Qt::SkipEmptyParts);
    QStandardItem* current = treeModel->invisibleRootItem();

    for (const auto& part : parts) {
        bool found = false;
        for (int row = 0; row < current->rowCount(); ++row) {
            QStandardItem* child = current->child(row);
            if (child->text() == part) {
                current = child;
                found = true;
                break;
            }
        }
        if (!found) {
            QStandardItem* newItem = new QStandardItem(part);
            newItem->setData(folderPath, Qt::UserRole + 1);
            current->appendRow(newItem);
            current = newItem;
            updateTreeViewItemIcon(treeModel->indexFromItem(newItem));
        }
    }

    emit treeModelUpdated();
    return true;
}

bool dataManager::deleteFolder(const QString& folderPath) {
    QDir dir(folderPath);
    if (!dir.exists()) {
        return false;
    }
    if (!dir.removeRecursively()) {
        return false;
    }

    const QStandardItem* item = nullptr;
    QDirIterator it(dataFolder, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        if (path == folderPath) {
            const QString relativePath = QDir(dataFolder).relativeFilePath(path);
            QStringList parts = relativePath.split("/", Qt::SkipEmptyParts);
            const QStandardItem* current = treeModel->invisibleRootItem();
            for (const auto& part : parts) {
                bool found = false;
                for (int row = 0; row < current->rowCount(); ++row) {
                    const QStandardItem* child = current->child(row);
                    if (child->text() == part) {
                        current = child;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return false;
                }
            }
            item = current;
            break;
        }
    }

    if (item) {
        QStandardItem* parent = item->parent() ? item->parent() : treeModel->invisibleRootItem();
        parent->removeRow(item->row());
        emit treeModelUpdated();
    }
    return true;
}

bool dataManager::createSymbol(const QString& folderPath, const QString& symbolName, const QList<symbolSettings>& settings) {
    const QString filePath = folderPath + "/" + symbolName + ".hd";
    QFile file(filePath);
    if (file.exists()) {
        qWarning() << "dataManager::createSymbol: File" + symbolName + " already exists";
        return false;
    }

    const QList<symbolSettings> initialSettings = settings.isEmpty() ? QList<symbolSettings>{
        {"ticker", "EURUSD"},
        {"description", "EUR/USD"},
        {"contract_size", "1"},
        {"units", "Share(s)"},
        {"min_volume", 1},
        {"max_volume", 100000000},
        {"volume_step", 0.01},
        {"min_tick", 0.00001},
        {"leverage", 0.05},
        {"trade_mode", "full"}
    } : settings;

    if (file.open(QIODevice::WriteOnly)) {
        QDataStream dataStream(&file);
        dataStream << initialSettings;
        file.close();
        emit symbolSettingsUpdated(filePath);
        return true;
    }
    return false;
}

bool dataManager::deleteSymbol(const QString& symbolPath) {
    QFile file(symbolPath);
    if (!file.exists()) {
        return false;
    }
    if (!file.remove()) {
        return false;
    }
    emit symbolSettingsUpdated(symbolPath);
    return true;
}

bool dataManager::renameSymbol(const QString& oldPath, const QString& newName) {
    QFile file(oldPath);
    if (!file.exists()) {
        return false;
    }
    const QString newPath = oldPath.section('/', 0, -2) + "/" + newName + ".hd";
    if (file.rename(newPath)) {
        emit symbolSettingsUpdated(newPath);
        return true;
    }
    return false;
}

bool dataManager::importCSV(const QString& symbolPath, const QString& csvFilePath) {
    QFile file(csvFilePath);
    QFile historicalData(symbolPath.section('.', 0, -2) + ".data");

    if (!historicalData.open(QIODevice::WriteOnly)) {
        messageBoxFactory::showError(nullptr, "Fail to import", "Fail to open symbol Error: " + historicalData.errorString());
        qCritical() << "dataManager::importCSV: fail to open symbol file by path: " + symbolPath;
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        messageBoxFactory::showError(nullptr, "Fail to import", "Fail to open csv table Error: " + file.errorString());
        qCritical() << "dataManager::importCSV: fail to open csv file by path: " + csvFilePath;
        return false;
    }

    bool created = messageBoxFactory::showProgressWindow(nullptr, "Importing CSV", "Importing please wait");

    QList<historicalCSVStroke> table;
    QTextStream in(&file);
    in.setAutoDetectUnicode(true);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields;
        historicalCSVStroke stroke{};

        if (!line.contains('"')) {
            fields = line.split(',');
        }

        if (fields.size() < 6) continue;

        QString dateTimeStr = fields[0].left(19);
        QDateTime dt = QDateTime::fromString(dateTimeStr, "yyyy-MM-dd HH:mm:ss");
        stroke.timestamp = dt.toSecsSinceEpoch();
        stroke.open = fields[1].toDouble();
        stroke.high = fields[2].toDouble();
        stroke.low = fields[3].toDouble();
        stroke.close = fields[4].toDouble();
        stroke.volume = fields[5].toLongLong();

        if (stroke.isValid()) {
            table.append(stroke);
        }
    }

    for (const historicalCSVStroke& stroke : table) {
        historicalData.write(reinterpret_cast<const char*>(&stroke), sizeof(historicalCSVStroke));
    }

    file.close();
    historicalData.close();

    emit historicalDataUpdated(symbolPath);
    emit importDone();

    if (created) {

        messageBoxFactory::hideProgressWindow();
        messageBoxFactory::showInfo(nullptr, "Import done", file.fileName() + " succesfully imported to " +
            " " + historicalData.fileName());
    }

    return true;
}

bool dataManager::exportCSV(const QString& symbolPath, const QString& exportDir) {
    QList<historicalCSVStroke> table = getHistoricalData(symbolPath);
    if (table.isEmpty()) {
        return false;
    }

    const QString filePath = exportDir + "/" + symbolPath.section('/', -1).section('.', 0, -2) + ".csv";
    QFile file(filePath);
    if (file.exists()) {
        file.remove();
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        messageBoxFactory::showError(nullptr, "Fail to export", "Fail to open csv file: " + file.errorString());
        return false;
    }

    messageBoxFactory::showProgressWindow(nullptr, "Exporting CSV", "Exporting please wait");

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    const QStringList headers = {"Date", "Open", "High", "Low", "Close", "Volume"};
    out << headers.join(',') << "\n";

    for (const historicalCSVStroke& stroke : table) {
        QStringList rowData;
        QDateTime dateTime = stroke.getDate();
        if (!dateTime.isValid()) {
            dateTime = QDateTime(QDate::currentDate(), QTime(12, 0, 0));
        }
        rowData << dateTime.toString("yyyy-MM-dd HH:mm:ss");
        rowData << QString::number(stroke.open, 'g', 17);
        rowData << QString::number(stroke.high, 'g', 17);
        rowData << QString::number(stroke.low, 'g', 17);
        rowData << QString::number(stroke.close, 'g', 17);
        rowData << QString::number(stroke.volume);
        out << rowData.join(',') << "\n";
    }

    file.close();
    emit historicalDataUpdated(symbolPath);
    emit exportDone();

    messageBoxFactory::hideProgressWindow();
    messageBoxFactory::showInfo(nullptr, "Export done", file.fileName() + " succesfully exported to " + exportDir);

    return true;
}

bool dataManager::deleteHistoricalData(const QString& symbolPath) {
    const QString dataPath = symbolPath.section('.', 0, -2) + ".data";
    QFile file(dataPath);
    if (!file.exists()) {
        return false;
    }
    if (!file.remove()) {
        return false;
    }
    emit historicalDataUpdated(symbolPath);
    return true;
}

void dataManager::importFiles(const QList<QString>& files, const QString &importPath, const QList<symbolSettings>& filesSettings) {
    if (!QDir(importPath).exists()) {
        return;
    }

    bool success = true;
    for (const QString& csvFilePath : files) {
        QString symbolName = QFileInfo(csvFilePath).baseName();
        QString symbolPath = importPath + "/" + symbolName + ".hd";

        if (!createSymbol(importPath, symbolName, filesSettings)) {
            qDebug() << "Failed to create symbol for" << symbolName;
            success = false;
            continue;
        }

        if (!importCSV(symbolPath, csvFilePath)) {
            qDebug() << "Failed to import CSV for" << symbolName;
            success = false;
            continue;
        }
    }

    if (success) {
        emit treeModelUpdated();
        emit symbolSettingsUpdated(importPath);
        emit historicalDataUpdated(importPath);
    }
}

bool dataManager::downloadYahooFinanceData(const QString &symbol, const QDate &startDate, const QDate &endDate,
    const QString &outputFilePath) {
    if (!startDate.isValid() || !endDate.isValid() || startDate > endDate) {
        qDebug() << "Invalid date range:" << startDate << "to" << endDate;
        emit yahooDataDownloaded(outputFilePath, false);
        return false;
    }

    messageBoxFactory::showProgressWindow(nullptr, "Downloading CSV", "Downloading CSV please wait");

    QProcess pythonCheck;
    pythonCheck.start("python", {"--version"});
    pythonCheck.waitForFinished(3000); // Wait 3 seconds
    if (pythonCheck.exitCode() != 0) {
        qDebug() << "Python not found:" << QString(pythonCheck.readAllStandardError());
        QMessageBox::warning(nullptr, "Python Required",
                             "Python is not installed. Please install Python 3.6+ and ensure 'python' is in your PATH.\n"
                             "Alternatively, download the CSV manually from https://finance.yahoo.com/quote/" + symbol + "/history "
                             "and import it using the 'Import CSV' option.");
        emit yahooDataDownloaded(outputFilePath, false);
        return false;
    }

    const QString scriptPath = QDir(QDir::currentPath()).filePath("Utils/YahooDownloader.py");

    QProcess process;
    const QStringList arguments = {
        scriptPath,
        symbol,
        startDate.toString("yyyy-MM-dd"),
        endDate.toString("yyyy-MM-dd"),
        outputFilePath
    };

    process.start("python", arguments);
    process.waitForFinished(-1);

    const bool success = (process.exitCode() == 0);
    if (!success) {
        qDebug() << "Python script failed with exit code:" << process.exitCode();
        qDebug() << "Standard output:" << QString(process.readAllStandardOutput());
        qDebug() << "Error output:" << QString(process.readAllStandardError());
    } else {
        qDebug() << "Python script completed successfully";
    }

    const QString symbolPath = QFileInfo(outputFilePath).absolutePath() + "/" + QFileInfo(outputFilePath).fileName().split('.').first() + ".hd";

    importCSV(symbolPath, outputFilePath);
    QFile::remove(outputFilePath);

    messageBoxFactory::hideProgressWindow();
    messageBoxFactory::showInfo(nullptr, "Download done", "AAPL csv succesfully downloaded to " +
        symbolPath.split('/').last());

    return true;
}

QList<QString> dataManager::setCurrentFolder(const QString &folder) {

    QList<QString> result;
    currentFolder = folder;
    QDirIterator it(currentFolder, QDir::Files);

    while (it.hasNext()) {

        result.append(it.next());
    }

    return result;
}

void dataManager::importDownloadedCSVDone(const QString& symbolPath) {

    disconnect(this, &dataManager::historicalDataUpdated, this, &dataManager::importDownloadedCSVDone);

    const QString safeOutputFilePath = symbolPath.split('.').first() + ".csv";

    QFile csvFile(safeOutputFilePath);
    if (csvFile.exists()) {
        if (!csvFile.remove()) {
            qDebug() << "Failed to delete CSV file:" << safeOutputFilePath;
            QMessageBox::warning(nullptr, "Deletion Error",
                                 "Failed to delete temporary CSV file: " + safeOutputFilePath + ".\n"
                                 "Please delete it manually.");
            emit yahooDataDownloaded(safeOutputFilePath, false);
            return;
        }
        qDebug() << "Deleted temporary CSV file:" << safeOutputFilePath;
    }

    emit yahooDataDownloaded(safeOutputFilePath, true);
}

QStandardItemModel* dataManager::getTreeModel() const {
    return treeModel;
}

QList<symbolSettings> dataManager::getSymbolSettings(const QString& symbolPath) const {
    QFile file(symbolPath);
    QList<symbolSettings> settings;
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream dataStream(&file);
        dataStream >> settings;
        file.close();
    }
    return settings;
}

QList<historicalCSVStroke> dataManager::getHistoricalData(const QString& symbolPath) const {
    QList<historicalCSVStroke> table;
    QFile historicalData(symbolPath.section('.', 0, -2) + ".data");
    if (!historicalData.open(QIODevice::ReadOnly)) {
        return table;
    }

    while (!historicalData.atEnd()) {
        historicalCSVStroke stroke{};
        qint64 bytesRead = historicalData.read(reinterpret_cast<char*>(&stroke), sizeof(historicalCSVStroke));
        if (bytesRead == sizeof(historicalCSVStroke) && stroke.isValid()) {
            table.append(stroke);
        }
    }

    historicalData.close();
    return table;
}

void dataManager::populateFolderItemsTable(const QString& folderPath, QTableWidget* tableWidget) const {
    tableWidget->setRowCount(0);
    QDirIterator it(folderPath, QDir::Files);
    while (it.hasNext()) {
        QString path = it.next();
        if (it.filePath().split('.').last() != "hd") {
            continue;
        }
        QString fileName = path.split('/').last().split('.').first();
        QTableWidgetItem* item = new QTableWidgetItem(fileName);
        item->setData(Qt::UserRole + 1, path);
        const int rowCount = tableWidget->rowCount();
        tableWidget->setRowCount(rowCount + 1);
        tableWidget->setItem(rowCount, 0, item);
        tableWidget->setItem(rowCount, 1, new QTableWidgetItem(""));
    }
}

void dataManager::populateSymbolDataTable(const QString& symbolPath, QTableWidget* tableWidget) const {
    tableWidget->setRowCount(0);
    QList<historicalCSVStroke> table = getHistoricalData(symbolPath);
    const QString dateFormat = "yyyy-MM-dd HH:mm:ss";
    for (const historicalCSVStroke& stroke : table) {
        const int rowCount = tableWidget->rowCount();
        tableWidget->setRowCount(rowCount + 1);
        tableWidget->setItem(rowCount, 0, new QTableWidgetItem(stroke.getDate().toString(dateFormat)));
        tableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::number(stroke.open, 'g', 17)));
        tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(stroke.high, 'g', 17)));
        tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(stroke.low, 'g', 17)));
        tableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::number(stroke.close, 'g', 17)));
        tableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::number(stroke.volume)));
    }
}

bool dataManager::loadCurrentSymbolData() {

    bool startLoading;
    const QString filePath = currentSymbol.split('.').first() + ".data";

    if (symbol) {
        if (symbol->getPath() != filePath) {

            symbol->deleteLater();
            symbol = new symbolData(filePath);
            symbol->startLoading();
        }
        startLoading = symbol->getIsStartLoading();
    }else {

        symbol = new symbolData(filePath);
        startLoading = symbol->getIsStartLoading();
        symbol->startLoading();
    }

    return startLoading;
}

void dataManager::loadTreeViewItems() {

    if (!QDir(dataFolder).exists()) {
        QDir(dataFolder).mkdir(dataFolder);
    }

    treeModel->clear();
    treeModel->setHorizontalHeaderLabels({"Tree view"});
    QDirIterator it(dataFolder, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        QFileInfo info = it.fileInfo();

        QString relativePath = QDir(dataFolder).relativeFilePath(info.absoluteFilePath());
        QStringList parts = relativePath.split("/", Qt::SkipEmptyParts);

        QStandardItem* current = nullptr;
        for (const auto & part : parts) {
            QStandardItem* parentItem = current ? current : treeModel->invisibleRootItem();
            bool found = false;

            for (int row = 0; row < parentItem->rowCount(); ++row) {
                QStandardItem* child = parentItem->child(row);
                if (child->text() == part) {
                    current = child;
                    found = true;
                    break;
                }
            }

            if (!found) {
                QStandardItem* newItem = new QStandardItem(part);
                newItem->setData(path, ItemDataPath);
                parentItem->appendRow(newItem);
                current = newItem;
                updateTreeViewItemIcon(treeModel->indexFromItem(newItem));
            }
        }
    }
    emit treeModelUpdated();
}

void dataManager::updateTreeViewItemIcon(const QModelIndex& index) const {
    if (!index.isValid()) return;
    QStandardItem* item = treeModel->itemFromIndex(index);
    if (treeModel->rowCount(index) == 0) {
        item->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    } else {
        item->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    }
}
