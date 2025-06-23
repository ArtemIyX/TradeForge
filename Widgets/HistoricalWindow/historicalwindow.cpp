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

historicalWindow::historicalWindow(QWidget *parent) :
    QDialog(parent), ui(new Ui::historicalWindow) {
    ui->setupUi(this);

    connect(ui->createSymbolButton, SIGNAL(clicked()), this, SLOT(createSymbolClicked()));
    connect(ui->symbolsTreeView, &QTreeView::clicked, this, &historicalWindow::treeViewItemClicked);

    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"RoboForex MT5"});

    ui->symbolsTreeView->setModel(model);

    ui->symbolsTreeView->setItemDelegate(new customStyledItemDelegate);

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

    ui->symbolsTreeView->expandAll();
}

historicalWindow::~historicalWindow() {
    delete ui;
}

void historicalWindow::loadTreeViewItems() {

    QDirIterator it{QDir(dataFolder)};

    qDebug() << it.filePath();

    while (it.hasNext()) {

        it.next();

        QFile file{it.filePath()};
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);

            QString text;
            out >> text;

            QStandardItem *item = new QStandardItem(it.filePath().split('.').first().split('/').last());

            item->setData(it.filePath(), ItemDataPath);

            model->appendRow(item);
        }

    }
}

void historicalWindow::createSymbolClicked() {

    QStandardItem *documentsItem = new QStandardItem("New symbol");
    model->appendRow({documentsItem});

    documentsItem->setEditable(true);

    ui->symbolsTreeView->setCurrentIndex(model->indexFromItem(documentsItem));
    ui->symbolsTreeView->edit(model->indexFromItem(documentsItem));

    auto func = [this, documentsItem](QWidget *editor, QAbstractItemDelegate::EndEditHint hint) {

        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);

        qDebug() << lineEdit->text();

        QString symbolPath = dataFolder + "/" + lineEdit->text() + ".hd";

        QFileInfo fileInfo(symbolPath);
        QDir dir = fileInfo.absoluteDir();

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

    QStandardItem *item = model->itemFromIndex(index);
    if (!item) {
        return;
    }

    QFile file{item->data(ItemDataPath).toString()};
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);


    }
}
