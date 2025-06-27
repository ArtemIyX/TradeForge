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
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QMouseEvent>

#include "../TitleBar/customTitleBar.h"
#include "Components/tableSymbolsStyledDelegate.h"
#include "CustomTable/historicatlWindowTable.h"
#include "Data/SymbolStructs.cuh"

historicalWindow::historicalWindow(QWidget *parent) :
    QDialog(parent), ui(new Ui::historicalWindow) {
    ui->setupUi(this);

    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint);

    QWidget* titleBar = new customTitleBar(this);
    ui->titleBarWidget->layout()->addWidget(titleBar);

    connect(ui->createSymbolButton, SIGNAL(clicked()), this, SLOT(createSymbolClicked()));
    connect(ui->importButton, SIGNAL(clicked()), this, SLOT(importFileClicked()));

    ui->createSymbolButton->setDisabled(true);
    ui->importButton->setDisabled(true);

    itemSettingsTable = new historicalWindowTable(this);

    itemSettingsTable->setStyleSheet(ui->folderItemsTable->styleSheet());

    ui->selectedFolderItemsWidget->layout()->addWidget(itemSettingsTable);

    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Tree view"});

    ui->tabWidget->tabBar()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);

    ui->symbolsTreeView->setModel(model);
    ui->symbolsTreeView->setEnabled(true);
    ui->symbolsTreeView->setItemDelegate(new customStyledItemDelegate);

    ui->symbolsTreeView->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->symbolsTreeView->header(), &QHeaderView::customContextMenuRequested,
            this, &historicalWindow::showTreeViewHeaderContext);

    connect(ui->symbolsTreeView, &QTreeView::clicked, this, &historicalWindow::treeViewItemClicked);

    ui->symbolsTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->symbolsTreeView, &QTreeView::customContextMenuRequested,
            this, &historicalWindow::showTreeViewContextMenu);

    connect(model, &QAbstractItemModel::rowsInserted, this,
        [this](const QModelIndex& parentLoc, int, int){

            updateTreeViewItemIcon(parentLoc);
    });

    connect(model, &QAbstractItemModel::rowsRemoved, this,
            [this](const QModelIndex& parentLoc, int, int){

                updateTreeViewItemIcon(parentLoc);
    });

    itemSettingsTable->setColumnCount(2);
    itemSettingsTable->setRowCount(10);
    itemSettingsTable->setHorizontalHeaderLabels({""});
    itemSettingsTable->verticalHeader()->setVisible(false);
    itemSettingsTable->horizontalHeader()->setVisible(false);
    itemSettingsTable->horizontalHeader()->setStretchLastSection(false);
    itemSettingsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    itemSettingsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    itemSettingsTable->setSelectionMode(QAbstractItemView::NoSelection);

    connect(itemSettingsTable, &historicalWindowTable::cellEditingFinished, this, &historicalWindow::settingValueChanged);

    ui->folderItemsTable->setHorizontalHeaderLabels({"Symbol","Description" });
    ui->folderItemsTable->setItemDelegate(new tableSymbolsStyledDelegate());
    ui->folderItemsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->folderItemsTable, &QTableWidget::customContextMenuRequested,
        this, &historicalWindow::showFolderItemsContextMenu);

    connect(ui->folderItemsTable, &QTableWidget::currentCellChanged, this, &historicalWindow::folderItemSelected);

    loadTreeViewItems();

    connect(ui->folderItemsTable, &QTableWidget::itemChanged, this, &historicalWindow::symbolNameAccepted);

    ui->symbolDataTableWidget->setColumnCount(6);
    ui->symbolDataTableWidget->resizeColumnsToContents();
    ui->symbolDataTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->symbolDataTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->symbolDataTableWidget->setHorizontalHeaderLabels({"Date","Open","High","Low","Close","Volume" });

    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, &historicalWindow::tabBarClicked);
}

historicalWindow::~historicalWindow() {
    delete ui;
}

void historicalWindow::loadTreeViewItems() const {

    if (!QDir(dataFolder).exists()) {
        QDir(dataFolder).mkdir(dataFolder);
    }

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
                updateTreeViewItemIcon(model->indexFromItem(newItem));
            }
        }
    }
}

void historicalWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        resizeEdges = edgesAt(event->pos());
        if (resizeEdges != Qt::Edges()) {
            resizing = true;
            dragStartPos = event->globalPos();
            event->accept();
        }
    }
}

void historicalWindow::mouseMoveEvent(QMouseEvent *event) {
    if (resizing) {
        QPoint delta = event->globalPos() - dragStartPos;
        QRect geom = geometry();

        if (resizeEdges.testFlag(Qt::LeftEdge))
            geom.setLeft(geom.left() + delta.x());
        if (resizeEdges.testFlag(Qt::RightEdge))
            geom.setRight(geom.right() + delta.x());
        if (resizeEdges.testFlag(Qt::TopEdge))
            geom.setTop(geom.top() + delta.y());
        if (resizeEdges.testFlag(Qt::BottomEdge))
            geom.setBottom(geom.bottom() + delta.y());

        setGeometry(geom);
        dragStartPos = event->globalPos();
    } else {
        updateCursorShape(event->pos());
    }
}

void historicalWindow::mouseReleaseEvent(QMouseEvent *event) {
    resizing = false;
}

Qt::Edges historicalWindow::edgesAt(const QPoint &pos) const {
    Qt::Edges edges;
    if (pos.x() <= RESIZE_MARGIN)
        edges |= Qt::LeftEdge;
    if (pos.x() >= width() - RESIZE_MARGIN)
        edges |= Qt::RightEdge;
    if (pos.y() <= RESIZE_MARGIN)
        edges |= Qt::TopEdge;
    if (pos.y() >= height() - RESIZE_MARGIN)
        edges |= Qt::BottomEdge;
    return edges;
}

void historicalWindow::updateCursorShape(const QPoint &pos) {
    const Qt::Edges edges = edgesAt(pos);
    if (edges == (Qt::LeftEdge | Qt::TopEdge) || edges == (Qt::RightEdge | Qt::BottomEdge))
        setCursor(Qt::SizeFDiagCursor);
    else if (edges == (Qt::RightEdge | Qt::TopEdge) || edges == (Qt::LeftEdge | Qt::BottomEdge))
        setCursor(Qt::SizeBDiagCursor);
    else if (edges.testFlag(Qt::LeftEdge) || edges.testFlag(Qt::RightEdge))
        setCursor(Qt::SizeHorCursor);
    else if (edges.testFlag(Qt::TopEdge) || edges.testFlag(Qt::BottomEdge))
        setCursor(Qt::SizeVerCursor);
    else
        setCursor(Qt::ArrowCursor);
}

void historicalWindow::updateTreeViewItemIcon(const QModelIndex &index) const {

    if (!index.isValid()) return;
    if (model->rowCount(index) == 0) {

        model->setData(index, QApplication::style()->standardIcon(QStyle::SP_DirIcon), Qt::DecorationRole);
    }else {

        model->setData(index, QIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon)), Qt::DecorationRole);
    }
}


void historicalWindow::createSymbolClicked() const {

    if (currentFolder.isEmpty()) { return;}

    QTableWidgetItem *item = new QTableWidgetItem("New Symbol");

    const int rowCount = ui->folderItemsTable->rowCount();

    ui->folderItemsTable->setRowCount(rowCount + 1);

    ui->folderItemsTable->setItem(rowCount, 0, item);
    ui->folderItemsTable->setItem(rowCount, 1, new QTableWidgetItem(""));

    ui->folderItemsTable->editItem(item);

    QFile file = QFile(currentFolder + "/" + item->text() + ".hd");

    QList<symbolSettings> initialItemSettings = {{"ticker", "EURUSD"},
        {"description", "EUR/USD"},
        {"contract_size", "1"},
        {"units", "Share(s)"},
        {"min_volume", 1},
        {"max_volume", 100000000},
        {"volume_step", 0.01},
        {"min_tick", 0.00001},
        {"leverage", 0.05},
        {"trade_mode", "full"}
    };

    if (file.open(QIODevice::WriteOnly)) {
        QDataStream dataStream(&file);

        dataStream << initialItemSettings;

        file.close();

        item->setData(ItemDataPath, currentFolder + "/" + item->text() + ".hd");
    }
}

void historicalWindow::importFileClicked() {

    if (currentFolderItem.isEmpty()) { return; }
    
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Выберите файл для импорта",
        "",
        "CSV таблица (*.csv*)"
    );

    QFile file(fileName);
    QFile historicalData(currentFolderItem.split('.').first() + ".data");

    if (!historicalData.open(QIODevice::WriteOnly)) {
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QDataStream dataStream(&historicalData);
    QTextStream in(&file);
    in.setAutoDetectUnicode(true);

    QList<historicalCSVStroke> data;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields;
        historicalCSVStroke stroke;

        if (!line.contains('"')) {
            fields = line.split(',');
        }

        QString dateTimeStr = fields[0].left(19);  // "2025-06-23 00:00:00"
        QString offsetStr = fields[0].mid(19);     // "-04:00"

        QDateTime dt = QDateTime::fromString(dateTimeStr, "yyyy-MM-dd HH:mm:ss");
        dt.setTimeZone(QTimeZone::utc());

        QRegularExpression re("([+-])(\\d{2}):(\\d{2})");
        QRegularExpressionMatch match = re.match(offsetStr);
        if (match.hasMatch()) {
            int hours = match.captured(2).toInt();
            int minutes = match.captured(3).toInt();
            int offsetSecs = hours * 3600 + minutes * 60;
            if (match.captured(1) == "-")
                offsetSecs = -offsetSecs;

            QTimeZone tz(offsetSecs);
            dt.setTimeZone(tz);
        }

        stroke.timestamp = dt.toSecsSinceEpoch();
        stroke.open = fields[1].toDouble();
        stroke.high = fields[2].toDouble();
        stroke.low = fields[3].toDouble();
        stroke.close = fields[4].toDouble();
        stroke.volume = fields[5].toLongLong();

        if (stroke.isValid()) {

            data.append(stroke);
        }else {
            qDebug() << "Found not valid stroke or it first stroke";
        }
    }

    for (historicalCSVStroke stroke : data) {

        historicalData.write(reinterpret_cast<const char*>(&stroke), sizeof(historicalCSVStroke));
    }

    file.close();
    historicalData.close();

    ui->tabWidget->setTabEnabled(1, true);
}

void historicalWindow::symbolNameAccepted(QTableWidgetItem* item) const {

    QString cellDataPath = item->data(ItemDataPath).toString();

    if (item->text().isEmpty()) {
        return;
    }

    if (cellDataPath.isEmpty()) {

        item->setData(ItemDataPath, currentFolder + "/" + item->text() + ".hd");
    }else {

        QFile file = QFile(cellDataPath);

        const QString oldFileName = file.fileName().split('/').last();

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

    ui->createSymbolButton->setDisabled(false);
    currentFolder = item->data(ItemDataPath).toString();

    QDirIterator it(currentFolder, QDir::Files);

    while (it.hasNext()) {

        const QString path = it.next();
        const QString fileName = path.split('/').last().split('.').first();

        if (it.filePath().split('.').last() != "hd") {
            continue;
        }

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

        connect(ui->symbolsTreeView->itemDelegate(), &QAbstractItemDelegate::closeEditor, this, &historicalWindow::treeViewSubDirectoryCreated);

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

void historicalWindow::treeViewSubDirectoryCreated(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) {

    disconnect(ui->symbolsTreeView->itemDelegate(), &QAbstractItemDelegate::closeEditor, this, &historicalWindow::treeViewSubDirectoryCreated);

    QStandardItem *folderItem = model->itemFromIndex(ui->symbolsTreeView->currentIndex());

    const QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    const QString folderPath = folderItem->data(ItemDataPath).toString() + "/" + lineEdit->text();
    const QDir dir(folderPath);

    if (!dir.exists()) {

        dir.mkpath(folderPath);
        folderItem->setData(folderPath, ItemDataPath);

    }else {

        model->removeRow(0, model->indexFromItem(folderItem));
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

        connect(ui->symbolsTreeView->itemDelegate(), &QAbstractItemDelegate::closeEditor,
            this, &historicalWindow::treeViewHeaderSubDirCreated);
    }
}

void historicalWindow::treeViewHeaderSubDirCreated(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) {

    disconnect(ui->symbolsTreeView->itemDelegate(), &QAbstractItemDelegate::closeEditor,
            this, &historicalWindow::treeViewHeaderSubDirCreated);

    const QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    const QString symbolPath = dataFolder + "/" + lineEdit->text();
    const QDir dir(symbolPath);

    QStandardItem *folderItem = model->itemFromIndex(ui->symbolsTreeView->currentIndex());

    if (!dir.exists()) {

        dir.mkpath(symbolPath);
        folderItem->setData(dataFolder + "/" + lineEdit->text(), ItemDataPath);
    }else {

        model->removeRow(0, model->indexFromItem(folderItem));
    }
}

void historicalWindow::folderItemSelected(const int currentRow, int currentColumn, int previousRow, int previousColumn) {

    ui->importButton->setDisabled(false);
    if (!ui->folderItemsTable->item(currentRow, 0)) {
        qDebug() << "historicalWindow::folderItemSelected: cell in row " << currentRow << " not valid maybe click on table without selected folder";
        return;
    }
    currentFolderItem = ui->folderItemsTable->item(currentRow, 0)->data(ItemDataPath).toString();

    if (QFile::exists(currentFolderItem.split('.').first() + ".data")) {

        ui->tabWidget->setTabEnabled(1, true);
    }else {

        ui->tabWidget->setTabEnabled(1, false);
    }

    QFile file(currentFolderItem);

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

void historicalWindow::settingValueChanged(const int row, const int column) const {

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

    const QTableWidgetItem *item = ui->folderItemsTable->itemAt(pos);
    if (!item or item->row() <= 0) return;

    QMenu contextMenu(this);

    contextMenu.setStyleSheet("color: rgb(255, 255, 255);");

    const QAction *deleteAction = contextMenu.addAction("Удалить");
    const QAction *selectedAction = contextMenu.exec(ui->folderItemsTable->mapToGlobal(pos));

    if (selectedAction == deleteAction) {

        const QString pathToData = item->data(ItemDataPath).toString();
        QFile::remove(pathToData);

        ui->folderItemsTable->removeRow( ui->folderItemsTable->currentRow());
    }
}

void historicalWindow::tabBarClicked(int index) {

    if (index == 1) {

        QString path = currentFolderItem.split('.').first() + ".data";

        QFile historicalData(path);

        if (!historicalData.open(QIODevice::ReadOnly)) {
            qDebug() << "Fail to open file: " << historicalData.errorString();
            return;
        }

        while (!historicalData.atEnd()) {
            historicalCSVStroke stroke;
            qint64 bytesRead = historicalData.read(reinterpret_cast<char*>(&stroke), sizeof(historicalCSVStroke));
            if (bytesRead == sizeof(historicalCSVStroke)) {

                if (!stroke.isValid()) {
                    qDebug() << "Found not valid stroke or it first stroke";
                    continue;
                }

                const int rowCount = ui->symbolDataTableWidget->rowCount();
                ui->symbolDataTableWidget->setRowCount(rowCount + 1);

                ui->symbolDataTableWidget->setItem(rowCount, 0, new QTableWidgetItem(stroke.getDate().toString()));
                ui->symbolDataTableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::number(stroke.open, 'g', 17)));
                ui->symbolDataTableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(stroke.high, 'g', 17)));
                ui->symbolDataTableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(stroke.low, 'g', 17)));
                ui->symbolDataTableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::number(stroke.close, 'g', 17)));
                ui->symbolDataTableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::number(stroke.volume)));
            } else {
                qWarning() << "Неполная запись или повреждённый файл.";
                break;
            }
        }

        historicalData.close();
    }else {


    }
}
