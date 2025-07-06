//
// Created by Stalker7274 on 23.06.2025.
//

#ifndef HISTORICALWINDOW_H
#define HISTORICALWINDOW_H

#include <QCandlestickSeries>
#include <QDialog>
#include <QDir>
#include <QPointer>

#include "Components/customStyledItemDelegate.h"
#include "SymbolStructs.cuh"

class symbolData;
class customMessageBox;
enum eMessageBoxType : int;
class importFilesWIndow;
class dataManager;
class QChartView;
class historicalWindowTable;
class QTableWidgetItem;

enum historicalWindowItemsData {
    ItemDataPath = Qt::UserRole + 1
};

class QStandardItemModel;
QT_BEGIN_NAMESPACE
namespace Ui { class historicalWindow; }
QT_END_NAMESPACE

class historicalWindow : public QDialog {
Q_OBJECT

public:
    explicit historicalWindow(QWidget *parent = nullptr);
    ~historicalWindow() override;

    void loadTreeViewItems() const;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:

    void createSymbolClicked() const;
    void exportFileClicked();
    void importFilesClicked(bool checked);

    void searchLineEditTextChanged(const QString &arg1) const;

    void symbolNameAccepted(QTableWidgetItem* item) const;
    void treeViewItemClicked(const QModelIndex &index);
    void showTreeViewContextMenu(const QPoint &pos);
    void treeViewSubDirectoryCreated(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);
    void showTreeViewHeaderContext(const QPoint &pos);
    void treeViewHeaderSubDirCreated(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);
    void folderItemSelected(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void settingValueChanged(int row, int column) const;
    void showFolderItemsContextMenu(const QPoint &pos);
    void tabBarClicked(int index);

    void currentTableDataChanged(const QTableWidgetItem *item);

    void showMessageBox(eMessageBoxType messageBoxType);
    void removeMessageBox();

    void onSymbolHistoricalDataUpdated(const QString& symbolPath);

    void onSymbolChanged(const QString& symbol);

    void folderItemsHeaderContextMenu(const QPoint &pos);

private:
    Ui::historicalWindow *ui;
    dataManager* historicalDataManager = nullptr;
    customMessageBox* waitWindow = nullptr;

    historicalWindowTable *itemSettingsTable;
    historicalWindowTable *folderItemsTable;

    QPointer<importFilesWIndow> importFiles;

    QString dataFolder = QDir::current().path() + "/data";

    QStandardItemModel *model;

    QString currentFolder = "";
    QString currentFolderItem = "";

    QList<historicalCSVStroke> currentTable;

    const int RESIZE_MARGIN = 5;
    bool resizing = false;
    QPointF dragStartPos;
    Qt::Edges resizeEdges;

    QChartView *chartView;
    QCandlestickSeries *series;
    QChart *chart;

    bool bSymbolDataNeedLoad = false;

    Qt::Edges edgesAt(const QPoint &pos) const;
    void updateCursorShape(const QPoint &pos);
    void updateTreeViewItemIcon(const QModelIndex& index) const;
    void drawSymbolData();


    ///////////////////////////////
    ///Setup widgets functs
    ///////////////////////////////

    void setupFolderItemsTable();
};


#endif //HISTORICALWINDOW_H
