//
// Created by Stalker7274 on 23.06.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_historicalWindow.h" resolved

#include "historicalwindow.h"
#include "ui_historicalWindow.h"
#include <QFile>
#include <QDir>
#include "Components/customStyledItemDelegate.h"
#include <qmenu.h>
#include <QtCharts>
#include <QFileDialog>
#include <QMouseEvent>
#include "../CandleCharts/candleChartView.h"
#include "../Subsystems/historicaldataManager.h"
#include "../Subsystems/symboldata.h"
#include "../TitleBar/customTitleBar.h"
#include "Components/tableSymbolsStyledDelegate.h"
#include "CustomMessageBox/custommessagebox.h"
#include "CustomTable/historicatlWindowTable.h"
#include "Data/SymbolStructs.cuh"
#include "DownloadCSVWindow/downloadcsvwindow.h"
#include "ImportFilesDialog/importfileswindow.h"

historicalWindow::historicalWindow(QWidget *parent) :
    QDialog(parent), ui(new Ui::historicalWindow) {
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    QWidget* titleBar = new customTitleBar(this);
    ui->titleBarWidget->layout()->addWidget(titleBar);

    historicalDataManager = dataManager::instance();

    connect(ui->createSymbolButton, SIGNAL(clicked()), this, SLOT(createSymbolClicked()));
    connect(ui->importFilesButton, Q_SIGNAL(QPushButton::clicked), this, &historicalWindow::importFilesClicked);

    connect(ui->searchSymbolLineEdit, &QLineEdit::textEdited, this, &historicalWindow::searchLineEditTextChanged);

    itemSettingsTable = new historicalWindowTable(this);
    folderItemsTable = new historicalWindowTable(this);

    const QString tablesStyle = R"(
        QTableWidget {
            border: 1px solid #666666;
            gridline-color: #555555;
		    color: #fff;
        }

        QHeaderView::section {
            background-color: #2e2e2e;
            color: #CCCCCC;
            padding: 4px;
            border-top: none;
            border-left: none;
            border-right: 1px solid #555555;
		    border-bottom: 1px solid #555555;
        }

        QTableView::item:selected:active {
            background: #2e2e2e;
            color: white;
        }

        QTableView::item:selected:!active {
            background: #2e2e2e;
            color: white;
        }
    )";

    itemSettingsTable->setStyleSheet(tablesStyle);
    folderItemsTable->setStyleSheet(tablesStyle);

    ui->selectedFolderItemsWidget->layout()->addWidget(folderItemsTable);
    ui->selectedFolderItemsWidget->layout()->addWidget(itemSettingsTable);

    folderItemsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    model = historicalDataManager->getTreeModel();
    model->setHorizontalHeaderLabels({"Tree view"});

    connect(historicalDataManager, &dataManager::showMessageBox, this, &historicalWindow::showMessageBox);
    connect(historicalDataManager, &dataManager::historicalDataUpdated, this, &historicalWindow::onSymbolHistoricalDataUpdated);
    connect(historicalDataManager, &dataManager::currentSymbolChanged, this, &historicalWindow::onSymbolChanged);

    ui->tabWidget->tabBar()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);

    ui->symbolsTreeView->setModel(model);
    ui->symbolsTreeView->setItemDelegate(new customStyledItemDelegate);

    connect(ui->symbolsTreeView->header(), Q_SIGNAL(&QHeaderView::customContextMenuRequested),
            this, &historicalWindow::showTreeViewHeaderContext);

    connect(ui->symbolsTreeView, Q_SIGNAL(&QTreeView::clicked), this, &historicalWindow::treeViewItemClicked);

    ui->symbolsTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->symbolsTreeView, Q_SIGNAL(&QTreeView::customContextMenuRequested),
            this, &historicalWindow::showTreeViewContextMenu);

    connect(model, Q_SIGNAL(&QAbstractItemModel::rowsInserted), this,
        [this](const QModelIndex& parentLoc, int, int){

            updateTreeViewItemIcon(parentLoc);
    });

    connect(model, Q_SIGNAL(&QAbstractItemModel::rowsRemoved), this,
            [this](const QModelIndex& parentLoc, int, int){

                updateTreeViewItemIcon(parentLoc);
    });

    itemSettingsTable->setColumnCount(2);
    itemSettingsTable->setRowCount(0);
    itemSettingsTable->setHorizontalHeaderLabels({""});
    itemSettingsTable->verticalHeader()->setVisible(false);
    itemSettingsTable->horizontalHeader()->setVisible(false);
    itemSettingsTable->horizontalHeader()->setStretchLastSection(true);
    itemSettingsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    itemSettingsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    itemSettingsTable->setSelectionMode(QAbstractItemView::NoSelection);
    itemSettingsTable->setItemDelegate(new tableSymbolsStyledDelegate());

    connect(itemSettingsTable, Q_SIGNAL(&historicalWindowTable::cellEditingFinished), this, &historicalWindow::settingValueChanged);

    folderItemsTable->setColumnCount(2);
    folderItemsTable->setHorizontalHeaderLabels({"Symbol","Description" });
    folderItemsTable->setItemDelegate(new tableSymbolsStyledDelegate());
    folderItemsTable->verticalHeader()->setVisible(false);
    folderItemsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    folderItemsTable->setRowCount(0);
    folderItemsTable->horizontalHeader()->setStretchLastSection(true);
    folderItemsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(folderItemsTable, Q_SIGNAL(&QTableWidget::customContextMenuRequested),
        this, &historicalWindow::showFolderItemsContextMenu);

    connect(folderItemsTable, Q_SIGNAL(&QTableWidget::currentCellChanged), this, &historicalWindow::folderItemSelected);

    loadTreeViewItems();

    connect(folderItemsTable, Q_SIGNAL(&QTableWidget::itemChanged), this, &historicalWindow::symbolNameAccepted);

    ui->symbolDataTableWidget->resizeColumnsToContents();
    ui->symbolDataTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->symbolDataTableWidget->setHorizontalHeaderLabels({"Date","Open","High","Low","Close","Volume" });

    connect(ui->tabWidget, Q_SIGNAL(&QTabWidget::tabBarClicked), this, &historicalWindow::tabBarClicked);

    chartView = new candleChartView();
    chartView->setRenderHint(QPainter::Antialiasing);

    series = new QCandlestickSeries();

    series->setIncreasingColor(QColor("#2ecc71"));
    series->setDecreasingColor(QColor("#e74c3c"));
    series->setBodyOutlineVisible(false);
    series->setMaximumColumnWidth(5);
    series->setMinimumColumnWidth(1.5);

    chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Candlestick");

    auto *axisX = new QDateTimeAxis;
    axisX->setFormat("dd.MM.yyyy");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisX->setMin(QDateTime(QDate(2000, 1, 1), QTime(12, 0, 0)));
    axisX->setMax(QDateTime(QDate(2025, 1, 1), QTime(12, 0, 0)));

    axisX->setTickCount(10);

    auto *axisY = new QValueAxis;
    axisY->setLabelFormat("%.2f");
    axisY->setTitleText("Price");
    axisY->setRange(40, 50);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setBackgroundBrush(QBrush(QColor("#252525")));
    chart->setTitleBrush(QBrush(Qt::white));
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setLabelsColor(Qt::white);
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setLabelsColor(Qt::white);

    //series->setUseOpenGL(true);
    chartView->setChart(chart);

    chartView->setDragMode(QGraphicsView::NoDrag);
    chartView->setInteractive(true);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    chartView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    ui->currentCSVTable->setLayout(new QVBoxLayout());
    ui->currentCSVTable->layout()->addWidget(chartView);
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
        for (const auto & part : parts) {
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
            dragStartPos = event->globalPosition();
            event->accept();
        }
    }
}

void historicalWindow::mouseMoveEvent(QMouseEvent *event) {
    if (resizing) {
        const QPointF delta = event->globalPosition() - dragStartPos;
        QRect geom = geometry();

        if (resizeEdges.testFlag(Qt::LeftEdge))
            geom.setLeft(geom.left() + static_cast<int>(delta.x()));
        if (resizeEdges.testFlag(Qt::RightEdge))
            geom.setRight(geom.right() + static_cast<int>(delta.x()));
        if (resizeEdges.testFlag(Qt::TopEdge))
            geom.setTop(geom.top() + static_cast<int>(delta.y()));
        if (resizeEdges.testFlag(Qt::BottomEdge))
            geom.setBottom(geom.bottom() + static_cast<int>(delta.y()));

        setGeometry(geom);
        dragStartPos = event->globalPosition();
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

    const int rowCount = folderItemsTable->rowCount();

    folderItemsTable->setRowCount(rowCount + 1);

    folderItemsTable->setItem(rowCount, 0, item);
    folderItemsTable->setItem(rowCount, 1, new QTableWidgetItem(""));

    folderItemsTable->editItem(item);

    QFile file = QFile(currentFolder + "/" + item->text() + ".hd");

    const QList<symbolSettings> initialItemSettings = {{"ticker", "EURUSD"},
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

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        "Выберите файл для импорта",
        "",
        "CSV таблица (*.csv*)"
    );

    dataManager::instance()->importCSV(currentFolderItem, fileName);
}

void historicalWindow::exportFileClicked() {

    const QString dirPath = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку для экспорта",
        ""
    );

    if (currentTable.isEmpty()) {
        qDebug() << "Error exportFileClicked: No data to export";
        return;
    }

    constexpr QChar sep = ',';

    const QString filePath = dirPath + "/" + currentFolderItem.split('/').last().split('.').first() + ".csv";

    if (QFile::exists(filePath)) {
        QFile::remove(filePath);
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: Cannot open file" << filePath;
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    const QStringList headers = {"Date","Open","High","Low","Close","Volume"};
    out << headers.join(sep) << "\n";

    for (historicalCSVStroke stroke : currentTable) {
        QStringList rowData;
        QDateTime dateTime = stroke.getDate();

        if (!dateTime.isValid()) {
            qDebug() << "Invalid QDateTime for stroke, setting to current date with 12:00:00";
            dateTime = QDateTime(QDate::currentDate(), QTime(12, 0, 0));
        } else {
            QTime time = dateTime.time();
            if (time == QTime(0, 0, 0)) {
                qDebug() << "No time specified for" << dateTime.toString() << ", setting to 12:00:00";
                dateTime.setTime(QTime(12, 0, 0));
            } else {
                qDebug() << "Time already specified:" << time.toString("HH:mm:ss");
            }
        }

        rowData << dateTime.toString("yyyy-MM-dd HH:mm:ss");
        rowData << QString::number(stroke.open, 'g', 17);
        rowData << QString::number(stroke.high, 'g', 17);
        rowData << QString::number(stroke.low, 'g', 17);
        rowData << QString::number(stroke.close, 'g', 17);
        rowData << QString::number(stroke.volume);

        out << rowData.join(sep) << "\n";
    }

    file.close();
    qDebug() << "Data successfully exported to" << filePath << "with separator" << sep;
}

void historicalWindow::importFilesClicked(bool checked) {

    if (!importFiles) {

        importFiles = new importFilesWIndow();
        importFiles->currentFolder = currentFolder;
        importFiles->setModal(true);
        importFiles->show();
    }
}

void historicalWindow::searchLineEditTextChanged(const QString &arg1) const {

    if (arg1.isEmpty()) {

        folderItemsTable->setRowCount(0);
        QDirIterator it(currentFolder, QDir::Files);

        while (it.hasNext()) {

            const QString path = it.next();
            const QString fileName = path.split('/').last().split('.').first();

            if (it.filePath().split('.').last() != "hd") {
                continue;
            }

            QTableWidgetItem *item = new QTableWidgetItem(fileName);
            item->setData(ItemDataPath, path);

            const int rowCount = folderItemsTable->rowCount();

            folderItemsTable->setRowCount(rowCount + 1);

            folderItemsTable->setItem(rowCount, 0, item);
            folderItemsTable->setItem(rowCount, 1, new QTableWidgetItem(""));

        }
    }else {

        while (folderItemsTable->rowCount() > 0) {
            folderItemsTable->removeRow(0);
        }

        QDirIterator it(currentFolder, QDir::Files);

        while (it.hasNext()) {
            const QString path = it.next();
            const QString fileName = path.split('/').last().split('.').first();

            if (it.filePath().split('.').last() != "hd") {
                continue;
            }

            if (!fileName.startsWith(arg1)) {
                continue;
            }

            QTableWidgetItem *item = new QTableWidgetItem(fileName);

            item->setData(ItemDataPath, path);

            const int rowCount = folderItemsTable->rowCount();

            folderItemsTable->setRowCount(rowCount + 1);

            folderItemsTable->setItem(rowCount, 0, item);
            folderItemsTable->setItem(rowCount, 1, new QTableWidgetItem(""));

        }
    }
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

    folderItemsTable->setRowCount(0);

    ui->createSymbolButton->setDisabled(false);
    ui->searchSymbolLineEdit->setDisabled(false);
    ui->importFilesButton->setDisabled(false);
    currentFolder = item->data(ItemDataPath).toString();

    QList<QString> files = historicalDataManager->setCurrentFolder(item->data(ItemDataPath).toString());

    for (const QString& filePath : files) {

        const QString fileName = filePath.split('/').last().split('.').first();

        if (filePath.split('.').last() != "hd") {
            continue;
        }

        QTableWidgetItem *tableWidget= new QTableWidgetItem(fileName);

        tableWidget->setData(ItemDataPath, filePath);

        const int rowCount = folderItemsTable->rowCount();

        folderItemsTable->setRowCount(rowCount + 1);

        folderItemsTable->setItem(rowCount, 0, tableWidget);
        folderItemsTable->setItem(rowCount, 1, new QTableWidgetItem(""));
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

        connect(ui->symbolsTreeView->itemDelegate(), Q_SIGNAL(&QAbstractItemDelegate::closeEditor), this, &historicalWindow::treeViewSubDirectoryCreated);

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

        connect(ui->symbolsTreeView->itemDelegate(), Q_SIGNAL(&QAbstractItemDelegate::closeEditor),
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

    if (!folderItemsTable->item(currentRow, 0)) {
        qDebug() << "historicalWindow::folderItemSelected: cell in row " << currentRow << " not valid maybe click on table without selected folder";
        return;
    }
    currentFolderItem = folderItemsTable->item(currentRow, 0)->data(ItemDataPath).toString();

    if (QFile::exists(currentFolderItem.split('.').first() + ".data")) {

        ui->tabWidget->setTabEnabled(1, true);
        ui->tabWidget->setTabEnabled(2, true);
    }else {

        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, false);
    }

    historicalDataManager->setCurrentSymbol(currentFolderItem);
    bSymbolDataNeedLoad = true;
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

    const QTableWidgetItem *item = folderItemsTable->itemAt(pos);
    if (!item) return;

    QMenu contextMenu(this);
    contextMenu.setStyleSheet("color: rgb(255, 255, 255);");

    const QAction *importCSVAction = contextMenu.addAction("Import csv");
    const QAction *exportCSVAction = contextMenu.addAction("Export csv");
    const QAction *deleteCSVAction = contextMenu.addAction("Delete csv");
    const QAction *downloadCSVAction = contextMenu.addAction("Download csv");
    const QAction *deleteAction = contextMenu.addAction("Delete");
    const QAction *selectedAction = contextMenu.exec(folderItemsTable->mapToGlobal(pos));

    currentFolderItem = item->data(ItemDataPath).toString();

    if (selectedAction == deleteAction) {

        const QString pathToData = item->data(ItemDataPath).toString();
        QFile::remove(pathToData);

        folderItemsTable->removeRow( folderItemsTable->currentRow());
    } else if (selectedAction == importCSVAction) {

        importFileClicked();
    } else if (selectedAction == exportCSVAction) {

        exportFileClicked();
    } else if (selectedAction == deleteCSVAction) {

        if (QFile::exists(currentFolderItem.split('.').first() + ".data")) {

            QFile::remove(currentFolderItem.split('.').first() + ".data");
        }
    } else if (selectedAction == downloadCSVAction) {
        downloadCSVWindow* downloadWindow = new downloadCSVWindow(currentFolderItem.split('.').first() + ".csv");
        downloadWindow->show();
        downloadWindow->setModal(true);
    }
}

void historicalWindow::tabBarClicked(int index) {

    if (index == 0) {

        disconnect(ui->symbolDataTableWidget, Q_SIGNAL(&QTableWidget::itemChanged), this, &historicalWindow::currentTableDataChanged);
    }else if (index == 1) {

        if (bSymbolDataNeedLoad) {

            bSymbolDataNeedLoad = false;
            historicalDataManager->loadCurrentSymbolData();
        }

        if (ui->symbolDataTableWidget->rowCount() != 0) return;

        const symbolData* symbolData = historicalDataManager->getSymbolData();
        for (historicalCSVStroke stroke : symbolData->getData()) {

            const QString dateFormat = "yyyy-MM-dd HH:mm:ss";

            const int rowCount = ui->symbolDataTableWidget->rowCount();
            ui->symbolDataTableWidget->setRowCount(rowCount + 1);

            if (!stroke.isValid()) {
                qDebug() << "Found not valid stroke or it first stroke";
                return;
            }

            ui->symbolDataTableWidget->setItem(rowCount, 0, new QTableWidgetItem(stroke.getDate().toString(dateFormat)));
            ui->symbolDataTableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::number(stroke.open, 'g', 17)));
            ui->symbolDataTableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(stroke.high, 'g', 17)));
            ui->symbolDataTableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(stroke.low, 'g', 17)));
            ui->symbolDataTableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::number(stroke.close, 'g', 17)));
            ui->symbolDataTableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::number(stroke.volume)));
        }

        connect(ui->symbolDataTableWidget, Q_SIGNAL(&QTableWidget::itemChanged), this, &historicalWindow::currentTableDataChanged);
    }else if (index == 2) {

        if (bSymbolDataNeedLoad) {

            bSymbolDataNeedLoad = false;
            historicalDataManager->loadCurrentSymbolData();
        }

        if (!series->sets().isEmpty()) return;

        const symbolData* symbolData = historicalDataManager->getSymbolData();
        for (historicalCSVStroke stroke : symbolData->getData()) {

            if (!stroke.isValid()) {
                qDebug() << "Found not valid stroke or it first stroke";
                return;
            }

            QCandlestickSet *set = new QCandlestickSet(stroke.open, stroke.high, stroke.low, stroke.close, stroke.timestamp * 1000);
            if (stroke.close >= stroke.open) {
                set->setPen(QPen(QColor("#2ecc71"), 0.5));
            } else {
                set->setPen(QPen(QColor("#e74c3c"), 0.5));
            }

            series->append(set);
        }
    }
}

void historicalWindow::drawSymbolData() {

    if (bSymbolDataNeedLoad) {

        bSymbolDataNeedLoad = false;

        if (!historicalDataManager->getSymbolData()) {

            historicalDataManager->loadCurrentSymbolData();
        }

        const symbolData* symbolData = historicalDataManager->getSymbolData();
        for (historicalCSVStroke stroke : symbolData->getData()) {

            const QString dateFormat = "yyyy-MM-dd HH:mm:ss";

            const int rowCount = ui->symbolDataTableWidget->rowCount();
            ui->symbolDataTableWidget->setRowCount(rowCount + 1);

            if (!stroke.isValid()) {
                qDebug() << "Found not valid stroke or it first stroke";
                return;
            }

            ui->symbolDataTableWidget->setItem(rowCount, 0, new QTableWidgetItem(stroke.getDate().toString(dateFormat)));
            ui->symbolDataTableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::number(stroke.open, 'g', 17)));
            ui->symbolDataTableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(stroke.high, 'g', 17)));
            ui->symbolDataTableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(stroke.low, 'g', 17)));
            ui->symbolDataTableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::number(stroke.close, 'g', 17)));
            ui->symbolDataTableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::number(stroke.volume)));

            QCandlestickSet *set = new QCandlestickSet(stroke.open, stroke.high, stroke.low, stroke.close, stroke.timestamp * 1000);
            if (stroke.close >= stroke.open) {
                set->setPen(QPen(QColor("#2ecc71"), 0.5));
            } else {
                set->setPen(QPen(QColor("#e74c3c"), 0.5));
            }

            series->append(set);
        }
    }
}

void historicalWindow::currentTableDataChanged(const QTableWidgetItem *item) {

    const int currentColumn = ui->symbolDataTableWidget->column(item);
    const int currentRow = ui->symbolDataTableWidget->row(item);

    historicalCSVStroke stroke = currentTable[currentRow];

    const QString dateFormat = "yyyy-MM-dd HH:mm:ss";
    QDateTime date;
    double numberDouble;
    qlonglong numberLongLong;
    bool numberValid;

    switch (currentColumn) {
        case 0:

            date = QDateTime::fromString(ui->symbolDataTableWidget->item(currentRow, 0)->text(), dateFormat);

            if (date.isValid()) {

                stroke.timestamp = date.toSecsSinceEpoch();
            }else {

                ui->symbolDataTableWidget->item(currentRow, 0)->setText(stroke.getDate().toString(dateFormat));
            }
            break;
        case 1:

            numberDouble = ui->symbolDataTableWidget->item(currentRow, 1)->text().toDouble(&numberValid);

            if (numberValid) {

                stroke.open = numberDouble;
            }else {

                ui->symbolDataTableWidget->item(currentRow, 1)->setText(QString::number(stroke.open, 'g', 17));
            }
            break;
        case 2:

            numberDouble = ui->symbolDataTableWidget->item(currentRow, 2)->text().toDouble(&numberValid);
            if (numberValid) {

                stroke.high = numberDouble;
            }else {

                ui->symbolDataTableWidget->item(currentRow, 2)->setText(QString::number(stroke.high, 'g', 17));
            }
            break;
        case 3:

            numberDouble = ui->symbolDataTableWidget->item(currentRow, 3)->text().toDouble(&numberValid);

            if (numberValid) {

                stroke.low = numberDouble;
            }else {

                ui->symbolDataTableWidget->item(currentRow, 3)->setText(QString::number(stroke.low, 'g', 17));
            }
            break;
        case 4:

            numberDouble = ui->symbolDataTableWidget->item(currentRow, 4)->text().toDouble(&numberValid);

            if (numberValid) {

                stroke.close = numberDouble;
            }else {

                ui->symbolDataTableWidget->item(currentRow, 4)->setText(QString::number(stroke.close, 'g', 17));
            }
            break;
        case 5:

            numberLongLong = ui->symbolDataTableWidget->item(currentRow, 5)->text().toLongLong(&numberValid);

            if (numberValid) {

                stroke.volume = numberLongLong;
            }else {

                ui->symbolDataTableWidget->item(currentRow, 5)->setText(QString::number(stroke.volume));
            }
            break;
        default:

            qDebug() << "ERROR historicalWindow::currentTableDataChanged: changed not valid column or column number 6 or higher";
    }

    if (!(numberValid or date.isValid())) {
        return;
    }

    QFile historicalData(currentFolderItem.split('.').first() + ".data");

    if (!historicalData.open(QIODevice::WriteOnly)) {
        qDebug() << "historicalWindow::currentTableDataChanged write failed: " << historicalData.errorString();
        return;
    }

    currentTable[currentRow] = stroke;

    for (historicalCSVStroke writeStroke : currentTable) {

        historicalData.write(reinterpret_cast<const char*>(&writeStroke), sizeof(historicalCSVStroke));
    }

    historicalData.close();
}

void historicalWindow::showMessageBox(eMessageBoxType messageBoxType) {

    switch (messageBoxType) {
        case startImport:

            if (waitWindow) return;
            waitWindow = new customMessageBox(this);
            waitWindow->setWindowModality(Qt::ApplicationModal);
            waitWindow->setWindowTitle("Please, wait");
            waitWindow->show();

            connect(historicalDataManager, &dataManager::importDone, this, &historicalWindow::removeMessageBox);

            break;
        case startExport:

            if (waitWindow) return;
            waitWindow = new customMessageBox(this);
            waitWindow->setWindowModality(Qt::ApplicationModal);
            waitWindow->setWindowTitle("Please, wait");
            waitWindow->show();

            connect(historicalDataManager, &dataManager::exportDone, this, &historicalWindow::removeMessageBox);

            break;
        case startDownload:

            if (waitWindow) return;

            waitWindow = new customMessageBox(this);
            waitWindow->setWindowModality(Qt::ApplicationModal);
            waitWindow->setWindowTitle("Please, wait");
            waitWindow->show();

            connect(historicalDataManager, &dataManager::yahooDataDownloaded, this, &historicalWindow::removeMessageBox);

            break;
        default:

            qDebug() << "Warning historicalWindow::showMessageBox: update showMessageBox switch default code called";
    }
}

void historicalWindow::removeMessageBox() {
    if (waitWindow) {

        waitWindow->close();
        waitWindow = nullptr;

        disconnect(historicalDataManager, &dataManager::importDone, this, &historicalWindow::removeMessageBox);
        disconnect(historicalDataManager, &dataManager::exportDone, this, &historicalWindow::removeMessageBox);
        disconnect(historicalDataManager, &dataManager::yahooDataDownloaded, this, &historicalWindow::removeMessageBox);
    }
}

void historicalWindow::onSymbolHistoricalDataUpdated(const QString &symbolPath) {

    if (symbolPath == currentFolderItem) {

        if (QFile::exists(currentFolderItem.split('.').first() + ".data")) {

            ui->tabWidget->setTabEnabled(1, true);
            ui->tabWidget->setTabEnabled(2, true);
        }else {

            ui->tabWidget->setTabEnabled(1, false);
            ui->tabWidget->setTabEnabled(2, false);
        }
    }
}

void historicalWindow::onSymbolChanged(const QString& symbol) {

    ui->symbolDataTableWidget->setRowCount(0);
    series->clear();
}