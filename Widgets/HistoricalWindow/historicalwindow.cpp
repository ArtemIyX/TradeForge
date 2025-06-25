//
// Created by Stalker7274 on 23.06.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_historicalWindow.h" resolved

#include "historicalwindow.h"
#include "ui_historicalWindow.h"
#include <QStandardItemModel>
#include <QFile>
#include <QDir>
#include "Components/customStyledItemDelegate.h"
#include <QDirIterator>
#include <qmenu.h>
#include <QTreeWidget>

#include "../TitleBar/customTitleBar.h"
#include "Components/tableSymbolsStyledDelegate.h"
#include "CustomTable/historicatlWindowTable.h"
#include "Data/SymbolStructs.cuh"

historicalWindow::historicalWindow(QWidget *parent) :
    QDialog(parent), ui(new Ui::historicalWindow) {
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    QWidget* titleBar = new customTitleBar(this);

    ui->titleBarWidget->layout()->addWidget(titleBar);

    itemSettingsTable = new historicalWindowTable(this);

    itemSettingsTable->setStyleSheet(ui->folderItemsTable->styleSheet());

    ui->selectedFolderItemsWidget->layout()->addWidget(itemSettingsTable);

    connect(ui->createSymbolButton, SIGNAL(clicked()), this, SLOT(createSymbolClicked()));
    connect(ui->symbolsTreeView, &QTreeView::clicked, this, &historicalWindow::treeViewItemClicked);

    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Tree view"});

    ui->tabWidget->tabBar()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    ui->symbolsTreeView->setModel(model);
    ui->symbolsTreeView->setEnabled(true);

    ui->symbolsTreeView->setItemDelegate(new customStyledItemDelegate);

    ui->symbolsTreeView->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->symbolsTreeView->header(), &QHeaderView::customContextMenuRequested,
            this, &historicalWindow::showTreeViewHeaderContext);


    ui->symbolsTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->symbolsTreeView, &QTreeView::customContextMenuRequested,
            this, &historicalWindow::showTreeViewContextMenu);

    itemSettingsTable->setColumnCount(2);
    itemSettingsTable->setRowCount(10);
    itemSettingsTable->setHorizontalHeaderLabels({"USA-100, US Tech 100 Index CFD"});
    itemSettingsTable->verticalHeader()->setVisible(false);
    itemSettingsTable->horizontalHeader()->setVisible(false);
    itemSettingsTable->horizontalHeader()->setStretchLastSection(false);
    itemSettingsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    itemSettingsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    itemSettingsTable->setItem(0, 0, new QTableWidgetItem("Digits"));
    itemSettingsTable->setItem(0, 1, new QTableWidgetItem("1"));
    itemSettingsTable->setSelectionMode(QAbstractItemView::NoSelection);

    connect(itemSettingsTable, &historicalWindowTable::cellEditingFinished, this, &historicalWindow::settingValueChanged);

    ui->folderItemsTable->horizontalHeader()->setVisible(false);
    ui->folderItemsTable->setRowCount(1);
    ui->folderItemsTable->setItem(0, 0, new QTableWidgetItem("Symbol"));
    ui->folderItemsTable->setItem(0, 1, new QTableWidgetItem("Description"));
    ui->folderItemsTable->setItemDelegate(new tableSymbolsStyledDelegate());
    ui->folderItemsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->folderItemsTable, &QTableWidget::customContextMenuRequested,
        this, &historicalWindow::showFolderItemsContextMenu);

    connect(ui->folderItemsTable, &QTableWidget::currentCellChanged, this, &historicalWindow::folderItemSelected);

    loadTreeViewItems();

    connect(ui->folderItemsTable, &QTableWidget::itemChanged, this, &historicalWindow::symbolNameAccepted);
}

historicalWindow::~historicalWindow() {
    delete ui;
}

void historicalWindow::loadTreeViewItems() {

    if (!QDir(dataFolder).exists()) {
        QDir(dataFolder).mkdir(dataFolder);
    }

    QHash<QString, QTreeWidgetItem*> pathItems;

    QDirIterator it(dataFolder, QDir::Dirs | QDir::NoDotAndDotDot,
                       QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString path = it.next();
        QFileInfo info = it.fileInfo();

        QString relativePath = QDir(dataFolder).relativeFilePath(info.absoluteFilePath());
        QStringList parts = relativePath.split("/", Qt::SkipEmptyParts);

        QStandardItem* current = nullptr;
        for (int i = 0; i < parts.size(); ++i) {
            const QString& part = parts[i];

            QStandardItem* parentItem = current ? current : model->invisibleRootItem();
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
            }
        }
    }
}

void historicalWindow::createSymbolClicked() {

    QTableWidgetItem *item = new QTableWidgetItem("New Symbol");

    int rowCount = ui->folderItemsTable->rowCount();

    ui->folderItemsTable->setRowCount(rowCount + 1);

    ui->folderItemsTable->setItem(rowCount, 0, item);
    ui->folderItemsTable->setItem(rowCount, 1, new QTableWidgetItem(""));

    ui->folderItemsTable->editItem(item);

    QFile file = QFile(currentFolder + "/" + item->text() + ".hd");

    QList<symbolSettings> initialItemSettings = {symbolSettings("ticker", "EURUSD"),
        symbolSettings("description", "EUR/USD"),
        symbolSettings("contract_size", "1")};

    if (file.open(QIODevice::WriteOnly)) {
        QDataStream dataStream(&file);

        dataStream << initialItemSettings;

        file.close();

        item->setData(ItemDataPath, currentFolder + "/" + item->text() + ".hd");
    }
}

void historicalWindow::symbolNameAccepted(QTableWidgetItem* item) {

    QString cellDataPath = item->data(ItemDataPath).toString();

    if (item->text().isEmpty()) {
        return;
    }

    if (cellDataPath.isEmpty()) {

        //QFile file = QFile(currentFolder + "/" + item->text() + ".hd");

        item->setData(ItemDataPath, currentFolder + "/" + item->text() + ".hd");

    }else {

        QFile file = QFile(cellDataPath);

        QString oldFileName = file.fileName().split('/').last();

        if (file.fileName().split('/').last().split('.').first() != item->text()) {

            file.rename(cellDataPath.remove(oldFileName) + "/" + item->text() + ".hd");
        }

        item->setData(ItemDataPath, file.fileName());
    }
}

void historicalWindow::treeViewItemClicked(const QModelIndex &index) {

    const QStandardItem *item = model->itemFromIndex(index);
    if (!item) {
        return;
    }

    ui->folderItemsTable->setRowCount(1);

    currentFolder = item->data(ItemDataPath).toString();

    QDirIterator it(currentFolder, QDir::Files);

    while (it.hasNext()) {

        const QString path = it.next();
        const QString fileName = path.split('/').last().split('.').first();

        QTableWidgetItem *item = new QTableWidgetItem(fileName);

        item->setData(ItemDataPath, path);

        int rowCount = ui->folderItemsTable->rowCount();

        ui->folderItemsTable->setRowCount(rowCount + 1);

        ui->folderItemsTable->setItem(rowCount, 0, item);
        ui->folderItemsTable->setItem(rowCount, 1, new QTableWidgetItem(""));

    }
}

void historicalWindow::showTreeViewContextMenu(const QPoint &pos) {

    const QModelIndex index = ui->symbolsTreeView->indexAt(pos);
    if (!index.isValid()) {
        qDebug() << "Clicked to empty space";
        return;
    }

    QMenu contextMenu(this);

    contextMenu.setStyleSheet("color: rgb(255, 255, 255);");

    const QAction *addFolderAction = contextMenu.addAction("Add folder");
    const QAction *deleteAction = contextMenu.addAction("Delete");

    const QAction *selectedAction = contextMenu.exec(ui->symbolsTreeView->mapToGlobal(pos));

    if (selectedAction == addFolderAction) {

        QStandardItem *folderItem = new QStandardItem("New folder");
        QStandardItem *itemParent = model->itemFromIndex(ui->symbolsTreeView->indexAt(pos));
        itemParent->appendRow({folderItem});
        folderItem->setEditable(true);
        folderItem->setData(itemParent->data(), ItemDataPath);

        ui->symbolsTreeView->setCurrentIndex(model->indexFromItem(folderItem));
        ui->symbolsTreeView->edit(model->indexFromItem(folderItem));

        bool bTrue = false;

        auto func = [this, folderItem, &bTrue](QWidget *editor, QAbstractItemDelegate::EndEditHint hint) {

            if (bTrue == true) {
                return;
            }
            bTrue = true;

            const QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
            const QString folderPath = folderItem->data(ItemDataPath).toString() + "/" + lineEdit->text();
            const QDir dir(folderPath);


            if (!dir.exists()) {

                dir.mkpath(folderPath);
                folderItem->setData(folderPath, ItemDataPath);

            }else {

                model->removeRow(0, model->indexFromItem(folderItem));
            }
        };

        connect(ui->symbolsTreeView->itemDelegate(), &QAbstractItemDelegate::closeEditor, this, func);

    } else if (selectedAction == deleteAction) {

        const QString itemPath = model->itemFromIndex(index)->data(ItemDataPath).toString();

        QDir(itemPath).removeRecursively();

        QStandardItem* itemToRemoveParent = model->itemFromIndex(index)->parent();

        if (itemToRemoveParent) {

            itemToRemoveParent->removeRow(model->itemFromIndex(index)->row());
        }else {

             model->removeRow(model->indexFromItem(model->itemFromIndex(index)).row());
        }
    }
}

void historicalWindow::showTreeViewHeaderContext(const QPoint &pos) {

    QMenu contextMenu(this);

    contextMenu.setStyleSheet("color: rgb(255, 255, 255);");

    const QAction *addFolderAction = contextMenu.addAction("Add folder");

    const QAction *selectedAction = contextMenu.exec(ui->symbolsTreeView->mapToGlobal(pos));

    if (selectedAction == addFolderAction) {

        QStandardItem *folderItem = new QStandardItem("New folder");

        model->appendRow({folderItem});

        folderItem->setEditable(true);

        ui->symbolsTreeView->setCurrentIndex(model->indexFromItem(folderItem));
        ui->symbolsTreeView->edit(model->indexFromItem(folderItem));

        auto func = [this, folderItem](QWidget *editor, QAbstractItemDelegate::EndEditHint hint) {

            const QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);

            const QString symbolPath = dataFolder + "/" + lineEdit->text();

            const QDir dir(symbolPath);

            if (!dir.exists()) {

                qDebug() << dir.mkpath(symbolPath);

                folderItem->setData(dataFolder + "/" + lineEdit->text(), ItemDataPath);

                qDebug() << folderItem->data(ItemDataPath).toString();

            }else {

                model->removeRow(0, model->indexFromItem(folderItem));
            }
        };

        connect(ui->symbolsTreeView->itemDelegate(), &QAbstractItemDelegate::closeEditor, this, func);

    }
}

void historicalWindow::folderItemSelected(int currentRow, int currentColumn, int previousRow, int previousColumn) {

    currentFolderItem = ui->folderItemsTable->item(currentRow, 0)->data(ItemDataPath).toString();

    QFile file = QFile(currentFolderItem);

    QList<symbolSettings> itemSettings;

    if (file.open(QIODevice::ReadOnly)) {
        QDataStream dataStream(&file);

        dataStream >> itemSettings;

        file.close();

        if (itemSettings.isEmpty()) {
            itemSettings = {symbolSettings("ticker", "EURUAH"),
                symbolSettings("description", ""),
                symbolSettings("contract_size", "15")};
        }
    }

    itemSettingsTable->setRowCount(static_cast<int>(itemSettings.size()));

    for (int i = 0; i < itemSettings.size(); ++i) {

        itemSettingsTable->setItem(i, 0, new QTableWidgetItem(itemSettings[i].settingName));

        itemSettingsTable->setItem(i, 1, new QTableWidgetItem(itemSettings[i].settingValue.toString()));
    }
}

void historicalWindow::settingValueChanged(int row, int column) {

    QFile file = QFile(currentFolderItem);

    QList<symbolSettings> itemSettings;

    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QDataStream dataStream(&file);

    dataStream >> itemSettings;

    file.close();

    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    qDebug() << itemSettingsTable->item(row, column)->text();

    itemSettings[row].settingValue = itemSettingsTable->item(row, column)->text();

    dataStream << itemSettings;

    file.close();
}

void historicalWindow::showFolderItemsContextMenu(const QPoint &pos) {

    QTableWidgetItem *item = ui->folderItemsTable->itemAt(pos);
    if (!item or item->row() <= 0) return;

    QMenu contextMenu(this);

    contextMenu.setStyleSheet("color: rgb(255, 255, 255);");

    QAction *deleteAction = contextMenu.addAction("Удалить");

    QAction *selectedAction = contextMenu.exec(ui->folderItemsTable->mapToGlobal(pos));

    if (selectedAction == deleteAction) {

        QString pathToData = item->data(ItemDataPath).toString();
        QFile::remove(pathToData);

        ui->folderItemsTable->removeRow( ui->folderItemsTable->currentRow());


    }
}
