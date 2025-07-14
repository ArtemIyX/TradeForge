//
// Created by Stalker7274 on 29.06.2025.
//

#ifndef HISTORICALDATAMANAGER_H
#define HISTORICALDATAMANAGER_H

#include <QStandardItemModel>
#include <QDir>
#include <QPointer>

#include "symboldata.h"
#include "SymbolStructs.cuh"

enum eMessageBoxType : int {
    startImport,
    startExport,
    startDownload
};

class QTableWidget;
struct historicalCSVStroke;
class QTableWidgetItem;

class dataManager : public QObject {
    Q_OBJECT

public:
    static dataManager* instance();

    bool createFolder(const QString& parentPath, const QString& folderName);
    bool deleteFolder(const QString& folderPath);
    bool createSymbol(const QString& folderPath, const QString& symbolName, const QList<symbolSettings>& settings = {});
    bool deleteSymbol(const QString& symbolPath);
    bool renameSymbol(const QString& oldPath, const QString& newName);

    bool importCSV(QString symbolPath, const QString& csvFilePath);
    bool exportCSV(const QString& symbolPath, const QString& exportDir);
    bool deleteHistoricalData(const QString& symbolPath);
    void importFiles(const QList<QString>& files, const QString &importPath, const QList<symbolSettings>& filesSettings);
    bool downloadYahooFinanceData(const QString& symbol, const QDate& startDate, const QDate& endDate, const QString& outputFilePath);

    ///sets QString currentFolder and returns files in this folder
    QList<QString> setCurrentFolder(const QString &folder);
    void setCurrentSymbol(const QString &symbolPath){ currentSymbol = symbolPath; emit currentSymbolChanged(currentSymbol); }

    //Data access
    QStandardItemModel* getTreeModel() const;
    QList<symbolSettings> getSymbolSettings(const QString& symbolPath) const;
    QList<historicalCSVStroke> getHistoricalData(const QString& symbolPath) const;
    symbolData* getSymbolData() {return symbol;}

    //Table population
    void populateFolderItemsTable(const QString& folderPath, QTableWidget* tableWidget) const;
    void populateSymbolDataTable(const QString& symbolPath, QTableWidget* tableWidget) const;

    bool loadCurrentSymbolData();

private slots:
    void importDownloadedCSVDone(const QString& symbolPath);

signals:
    void showMessageBox(eMessageBoxType boxType);
    void treeModelUpdated();
    void importDone();
    void exportDone();
    void symbolSettingsUpdated(const QString& symbolPath);
    void historicalDataUpdated(const QString& symbolPath);
    void yahooDataDownloaded(const QString& outputFilePath, bool success);
    void strokeLoaded(historicalCSVStroke stroke);
    void currentSymbolChanged(const QString& symbolPath);

private:
    explicit dataManager(QObject* parent = nullptr);
    ~dataManager() override;

    dataManager(const dataManager&) = delete;
    dataManager& operator=(const dataManager&) = delete;

    void loadTreeViewItems();
    void updateTreeViewItemIcon(const QModelIndex& index) const;

    QString dataFolder = QDir::currentPath() + "/data";
    QStandardItemModel* treeModel;

    static dataManager* m_instance;

    QString currentFolder;
    QString currentSymbol;
    QList<historicalCSVStroke> currentGraphic;

    QPointer<symbolData> symbol;
};

#endif //HISTORICALDATAMANAGER_H
