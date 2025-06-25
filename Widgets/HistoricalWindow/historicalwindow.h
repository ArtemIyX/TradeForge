//
// Created by Stalker7274 on 23.06.2025.
//

#ifndef HISTORICALWINDOW_H
#define HISTORICALWINDOW_H

#include <QDialog>
#include <QDir>

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

    void loadTreeViewItems();

public slots:

    void createSymbolClicked();
    void symbolNameAccepted(QTableWidgetItem* item);
    void treeViewItemClicked(const QModelIndex &index);
    void showTreeViewContextMenu(const QPoint &pos);
    void showTreeViewHeaderContext(const QPoint &pos);
    void folderItemSelected(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void settingValueChanged(int row, int column);

private:
    Ui::historicalWindow *ui;

    historicalWindowTable *itemSettingsTable;

    QString dataFolder = QDir::current().path() + "/data";

    QStandardItemModel *model;

    QString currentFolder = "";

    QString currentFolderItem = "";
};


#endif //HISTORICALWINDOW_H
