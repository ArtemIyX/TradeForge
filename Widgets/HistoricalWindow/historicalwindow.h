//
// Created by Stalker7274 on 23.06.2025.
//

#ifndef HISTORICALWINDOW_H
#define HISTORICALWINDOW_H

#include <QCandlestickSeries>
#include <QDialog>
#include <QDir>
#include <QPointer>
#include <QValueAxis>

#include "Components/customStyledItemDelegate.h"
#include "SymbolStructs.cuh"

class QDateTimeAxis;
class QStandardItemModel;
class symbolData;
class customMessageBox;
enum eMessageBoxType : int;
class importFilesWIndow;
class dataManager;
class QChartView;
class historicalWindowTable;
class QTableWidgetItem;

enum eMessageBoxType : int;


QT_BEGIN_NAMESPACE
namespace Ui { class historicalWindow; }
QT_END_NAMESPACE

class historicalWindow : public QDialog {
Q_OBJECT

public:
    explicit historicalWindow(QWidget *parent = nullptr);
    ~historicalWindow() override;

private:
    // UI-related members
    Ui::historicalWindow *ui;
    historicalWindowTable *folderItemsTable;
    historicalWindowTable *itemSettingsTable;
    QPointer<importFilesWIndow> importFiles;
    QChartView *chartView;
    QCandlestickSeries *series;
    QChart *chart;
    QValueAxis *axisY;
    QDateTimeAxis *axisX;

    // Data-related members
    dataManager *historicalDataManager = nullptr;
    QStandardItemModel *model;
    QString dataFolder = QDir::current().path() + "/data";
    QString currentFolder = "";
    QString currentFolderItem = "";
    QList<historicalCSVStroke> currentTable;

    // State-related members
    bool bSymbolDataNeedLoad = false;
    bool resizing = false;
    const int RESIZE_MARGIN = 5;
    QPointF dragStartPos;
    Qt::Edges resizeEdges;

protected:
    // Event handlers
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    // UI interaction slots
    void createSymbol() const;
    void exportFile();
    void importFilesClicked(bool checked);
    void searchLineEditTextChanged(const QString &arg1) const;
    void symbolNameAccepted(QTableWidgetItem *item) const;
    void tabBarClicked(int index);

    // Tree view slots
    void showTreeViewContextMenu(const QPoint &pos);
    void showTreeViewHeaderContext(const QPoint &pos);
    void treeViewHeaderSubDirCreated(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);
    void treeViewItemClicked(const QModelIndex &index);
    void treeViewSubDirectoryCreated(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);
    void treeViewDirRenamed(const QModelIndex &index);

    void currentTableDataChanged(const QTableWidgetItem *item);
    void folderItemSelected(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void settingValueChanged(int row, int column) const;
    void showFolderItemsContextMenu(const QPoint &pos);

    // Data update slots
    void onSymbolChanged(const QString &symbol);
    void onSymbolHistoricalDataUpdated(const QString &symbolPath);

private:
    // Helper functions
    void drawSymbolData();
    Qt::Edges edgesAt(const QPoint &pos) const;
    void updateCursorShape(const QPoint &pos);
    void updateTreeViewItemIcon(const QModelIndex &index) const;

    // Setup functions
    void setupFolderItemsTable();
};


#endif //HISTORICALWINDOW_H
