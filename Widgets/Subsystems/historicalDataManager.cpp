//
// Created by Stalker7274 on 29.06.2025.
//

#include "historicaldataManager.h"
#include <QApplication>
#include <QFile>
#include <QDirIterator>
#include <QFileDialog>
#include <QTextStream>
#include <QTableWidget>

dataManager* dataManager::m_instance = nullptr;

dataManager* dataManager::instance() {
    if (!m_instance) {
        m_instance = new dataManager();
    }
    return m_instance;
}

dataManager::dataManager(QObject* parent) : QObject(parent), treeModel(new QStandardItemModel(this)) {
    treeModel->setHorizontalHeaderLabels({"Tree view"});
}

dataManager::~dataManager() {
    delete treeModel;
}

void dataManager::initialize(const QString& dataFolderPath) {
    dataFolder = dataFolderPath;
    if (!QDir(dataFolder).exists()) {
        QDir().mkpath(dataFolder);
    }
    loadTreeViewItems();
}

bool dataManager::createFolder(const QString& parentPath, const QString& folderName) {
    QString folderPath = parentPath + "/" + folderName;
    QDir dir(folderPath);
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

    QStandardItem* item = nullptr;
    QDirIterator it(dataFolder, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        if (path == folderPath) {
            QString relativePath = QDir(dataFolder).relativeFilePath(path);
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
    QString filePath = folderPath + "/" + symbolName + ".hd";
    QFile file(filePath);
    if (file.exists()) {
        return false;
    }

    QList<symbolSettings> initialSettings = settings.isEmpty() ? QList<symbolSettings>{
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
    QString newPath = oldPath.section('/', 0, -2) + "/" + newName + ".hd";
    if (file.rename(newPath)) {
        emit symbolSettingsUpdated(newPath);
        return true;
    }
    return false;
}

bool dataManager::importCSV(const QString& symbolPath, const QString& csvFilePath) {
    QFile file(csvFilePath);
    QFile historicalData(symbolPath.section('.', 0, -2) + ".data");
    if (!file.exists() || !historicalData.open(QIODevice::WriteOnly)) {
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

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
    return true;
}

bool dataManager::exportCSV(const QString& symbolPath, const QString& exportDir) {
    QList<historicalCSVStroke> table = getHistoricalData(symbolPath);
    if (table.isEmpty()) {
        return false;
    }

    QString filePath = exportDir + "/" + symbolPath.section('/', -1).section('.', 0, -2) + ".csv";
    QFile file(filePath);
    if (file.exists()) {
        file.remove();
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

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
    return true;
}

bool dataManager::deleteHistoricalData(const QString& symbolPath) {
    QString dataPath = symbolPath.section('.', 0, -2) + ".data";
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
    QString dateFormat = "yyyy-MM-dd HH:mm:ss";
    for (const historicalCSVStroke& stroke : table) {
        int rowCount = tableWidget->rowCount();
        tableWidget->setRowCount(rowCount + 1);
        tableWidget->setItem(rowCount, 0, new QTableWidgetItem(stroke.getDate().toString(dateFormat)));
        tableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::number(stroke.open, 'g', 17)));
        tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(stroke.high, 'g', 17)));
        tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(stroke.low, 'g', 17)));
        tableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::number(stroke.close, 'g', 17)));
        tableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::number(stroke.volume)));
    }
}

void dataManager::loadTreeViewItems() {
    treeModel->clear();
    treeModel->setHorizontalHeaderLabels({"Tree view"});
    QDirIterator it(dataFolder, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        QFileInfo info = it.fileInfo();
        QString relativePath = QDir(dataFolder).relativeFilePath(info.absoluteFilePath());
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
                newItem->setData(path, Qt::UserRole + 1);
                current->appendRow(newItem);
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