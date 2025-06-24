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

historicalWindow::historicalWindow(QWidget *parent) :
    QDialog(parent), ui(new Ui::historicalWindow) {
    ui->setupUi(this);

    connect(ui->createSymbolButton, SIGNAL(clicked()), this, SLOT(createSymbolClicked()));
    connect(ui->symbolsTreeView, &QTreeView::clicked, this, &historicalWindow::treeViewItemClicked);

    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Tree view"});

    ui->symbolsTreeView->setModel(model);
    ui->symbolsTreeView->setEnabled(true);

    ui->symbolsTreeView->setItemDelegate(new customStyledItemDelegate);

    ui->symbolsTreeView->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->symbolsTreeView->header(), &QHeaderView::customContextMenuRequested,
            this, &historicalWindow::showTreeViewHeaderContext);


    ui->symbolsTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->symbolsTreeView, &QTreeView::customContextMenuRequested,
            this, &historicalWindow::showTreeViewContextMenu);

    ui->itemSettingsTable->setColumnCount(2);
    ui->itemSettingsTable->setRowCount(10);
    ui->itemSettingsTable->setHorizontalHeaderLabels({"USA-100, US Tech 100 Index CFD"});
    ui->itemSettingsTable->verticalHeader()->setVisible(false);
    ui->itemSettingsTable->horizontalHeader()->setVisible(false);
    ui->itemSettingsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->itemSettingsTable->horizontalHeader()->setStretchLastSection(false); 
    ui->itemSettingsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->itemSettingsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->itemSettingsTable->setItem(0, 0, new QTableWidgetItem("Digits"));
    ui->itemSettingsTable->setItem(0, 1, new QTableWidgetItem("1"));

    ui->itemSettingsTable->setSelectionMode(QAbstractItemView::NoSelection);

    loadTreeViewItems();
}

historicalWindow::~historicalWindow() {
    delete ui;
}

void historicalWindow::loadTreeViewItems() {

    if (!QDir(dataFolder).exists()) {
        QDir(dataFolder).mkdir(dataFolder);
    }

    QHash<QString, QTreeWidgetItem*> pathItems;

    QDirIterator it(dataFolder, QDir::AllEntries | QDir::NoDotAndDotDot,
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

    return;
    QStandardItem *documentsItem = new QStandardItem("New symbol");
    model->appendRow({documentsItem});

    documentsItem->setEditable(true);

    ui->symbolsTreeView->setCurrentIndex(model->indexFromItem(documentsItem));
    ui->symbolsTreeView->edit(model->indexFromItem(documentsItem));

    auto func = [this, documentsItem](QWidget *editor, QAbstractItemDelegate::EndEditHint hint) {

        const QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);

        qDebug() << lineEdit->text();

        const QString symbolPath = dataFolder + "/" + lineEdit->text() + ".hd";

        const QFileInfo fileInfo(symbolPath);
        const QDir dir = fileInfo.absoluteDir();

        if (!dir.exists()) {
            dir.mkpath(".");
        }

        QFile file(symbolPath);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream in(&file);

            QString text = lineEdit->text();

            in << text;

            documentsItem->setData(symbolPath, ItemDataPath);
        }
    };

    connect(ui->symbolsTreeView->itemDelegate(), &QAbstractItemDelegate::closeEditor, this, func);
}

void historicalWindow::treeViewItemClicked(const QModelIndex &index) {

    return;
    const QStandardItem *item = model->itemFromIndex(index);
    if (!item) {
        return;
    }

    QFile file{item->data(ItemDataPath).toString()};
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);


    }
}

void historicalWindow::showTreeViewContextMenu(const QPoint &pos) {

    const QModelIndex index = ui->symbolsTreeView->indexAt(pos);
    if (!index.isValid()) {
        qDebug() << "Clicked to empty space";
        return;
    }

    QMenu contextMenu(this);

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

        qDebug() << "Delete folder";
    }
}

void historicalWindow::showTreeViewHeaderContext(const QPoint &pos) {

    QMenu contextMenu(this);

    const QAction *addFolderAction = contextMenu.addAction("Add folder");

    const QAction *selectedAction = contextMenu.exec(ui->symbolsTreeView->mapToGlobal(pos));

    if (selectedAction == addFolderAction) {

        QStandardItem *folderItem = new QStandardItem("New folder");
        //model->appendRow({documentsItem});

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
