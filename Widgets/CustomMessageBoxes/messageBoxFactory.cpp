//
// Created by Stalker7274 on 06.07.2025.
//

#include "messageBoxFactory.h"

#include <qabstractbutton.h>
#include <QMessageBox>

#include "Windows/AcceptMessageBox/customacceptmessagebox.h"
#include "Windows/InfoMessageBox/custominfomessagebox.h"
#include "Windows/ProgressMessageBox/customprogressmessagebox.h"

customProgressMessageBox* messageBoxFactory::progressDialog = nullptr;

void messageBoxFactory::showInfo(QWidget *parent, const QString &title, const QString &message) {
    if (progressDialog) {
        return;
    }
    customInfoMessageBox msgBox(message);
    msgBox.exec();
}

void messageBoxFactory::showWarning(QWidget *parent, const QString &title, const QString &message) {
    customInfoMessageBox msgBox(message);
    msgBox.exec();
}

void messageBoxFactory::showError(QWidget *parent, const QString &title, const QString &message) {
    customInfoMessageBox msgBox(message);
    msgBox.exec();
}

customProgressMessageBox* messageBoxFactory::showProgressWindow(const QString &message) {
    if (progressDialog) {
        return nullptr;
    }

    progressDialog = new customProgressMessageBox(message);
    return progressDialog;
}

void messageBoxFactory::hideProgressWindow() {
    if (!progressDialog) {
        qWarning() << "Fail to hide progress window";
        return;
    }
    progressDialog->close();
    progressDialog->deleteLater();
    progressDialog = nullptr;
}

bool messageBoxFactory::showAcceptWindow(QWidget *parent, const QString &text) {

    customAcceptMessageBox msgBox(text);
    return msgBox.exec() == QDialog::Accepted;
}
