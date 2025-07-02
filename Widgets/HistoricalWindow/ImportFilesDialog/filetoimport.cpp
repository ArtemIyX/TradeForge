//
// Created by Stalker7274 on 29.06.2025.
//

#include "filetoimport.h"

#include <qdesktopservices.h>
#include <qevent.h>
#include <qfileinfo.h>
#include <qmenu.h>
#include <qmessagebox.h>

#include "ui_fileToImport.h"


fileToImport::fileToImport(QString Path, QWidget *parent) :
    QWidget(parent), ui(new Ui::fileToImport), filePath(Path) {
    ui->setupUi(this);

    ui->fileNameLabel->setText(filePath.split('/').last().split('.').first());
    setToolTip(filePath);
}

fileToImport::~fileToImport() {
    delete ui;
}

void fileToImport::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);

    QAction *showAction = menu.addAction("Show in Explorer");
    QAction *deleteAction = menu.addAction("Remove from import");

    connect(showAction, &QAction::triggered, this, &fileToImport::showInExplorer);
    connect(deleteAction, &QAction::triggered, this, &fileToImport::deleteFile);

    menu.exec(event->globalPos());
}

void fileToImport::showInExplorer() {
    QFileInfo info(filePath);
    if (info.exists()) {
#ifdef Q_OS_WIN
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
#else
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
#endif
    } else {
        QMessageBox::warning(this, "File Not Found", "The file no longer exists.");
    }
}

void fileToImport::deleteFile() {

    emit preDestroy(this);
    this->deleteLater();
}
