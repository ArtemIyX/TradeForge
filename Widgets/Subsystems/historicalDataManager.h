//
// Created by Stalker7274 on 29.06.2025.
//

#ifndef HISTORICALDATAMANAGER_H
#define HISTORICALDATAMANAGER_H

#include <QStandardItemModel>
#include <QDir>
#include <QNetworkCookieJar>
#include "SymbolStructs.cuh"

class QNetworkAccessManager;
class QTableWidget;
struct historicalCSVStroke;
class QTableWidgetItem;

class dataManager : public QObject {
    Q_OBJECT

public:
    static dataManager* instance();

    void initialize(const QString& dataFolderPath = QDir::currentPath() + "/data");

    // Folder and symbol management
    bool createFolder(const QString& parentPath, const QString& folderName);
    bool deleteFolder(const QString& folderPath);
    bool createSymbol(const QString& folderPath, const QString& symbolName, const QList<symbolSettings>& settings = {});
    bool deleteSymbol(const QString& symbolPath);
    bool renameSymbol(const QString& oldPath, const QString& newName);

    // Historical data management
    bool importCSV(const QString& symbolPath, const QString& csvFilePath);
    bool exportCSV(const QString& symbolPath, const QString& exportDir);
    bool deleteHistoricalData(const QString& symbolPath);
    void importFiles(const QList<QString>& files, const QString &importPath, const QList<symbolSettings>& filesSettings);
    bool downloadYahooFinanceData(const QString& symbol, const QDate& startDate, const QDate& endDate, const QString& outputFilePath);

    // Data access
    QStandardItemModel* getTreeModel() const;
    QList<symbolSettings> getSymbolSettings(const QString& symbolPath) const;
    QList<historicalCSVStroke> getHistoricalData(const QString& symbolPath) const;

    // Table population
    void populateFolderItemsTable(const QString& folderPath, QTableWidget* tableWidget) const;
    void populateSymbolDataTable(const QString& symbolPath, QTableWidget* tableWidget) const;

signals:
    void treeModelUpdated();
    void symbolSettingsUpdated(const QString& symbolPath);
    void historicalDataUpdated(const QString& symbolPath);
    void yahooDataDownloaded(const QString& outputFilePath, bool success);

private:
    explicit dataManager(QObject* parent = nullptr);
    ~dataManager() override;

    dataManager(const dataManager&) = delete;
    dataManager& operator=(const dataManager&) = delete;

    void loadTreeViewItems();
    void updateTreeViewItemIcon(const QModelIndex& index) const;
    void fetchYahooCrumbAndCookie(const QString& symbol, const QString& outputFilePath, const QDate& startDate, const QDate& endDate);

    QString dataFolder;
    QStandardItemModel* treeModel;
    QNetworkAccessManager* networkManager;
    QNetworkCookieJar* cookieJar;
    QString crumb;

    static dataManager* m_instance;
};

#endif //HISTORICALDATAMANAGER_H
