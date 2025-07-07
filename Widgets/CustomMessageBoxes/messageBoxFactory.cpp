//
// Created by Stalker7274 on 06.07.2025.
//

#include "messageBoxFactory.h"

#include <qabstractbutton.h>
#include <QMessageBox>

QProgressDialog* messageBoxFactory::progressDialog = nullptr;

void messageBoxFactory::showInfo(QWidget *parent, const QString &title, const QString &message) {

    if (progressDialog) {
        return;
    }
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.button(QMessageBox::Ok)->setText("ОК");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

void messageBoxFactory::showWarning(QWidget *parent, const QString &title, const QString &message) {
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.button(QMessageBox::Ok)->setText("ОК");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
}

void messageBoxFactory::showError(QWidget *parent, const QString &title, const QString &message) {
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.button(QMessageBox::Ok)->setText("ОК");
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
}

QProgressDialog * messageBoxFactory::showProgressWindow(QWidget *parent, const QString &title, const QString &message) {
    if (progressDialog) {
        return nullptr;
    }
    progressDialog = new QProgressDialog(message, QString(), 0, 1000000, parent);
    progressDialog->setWindowTitle(title);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setCancelButton(nullptr);
    progressDialog->setMinimumDuration(0);
    progressDialog->setValue(0);
    progressDialog->show();
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

bool messageBoxFactory::showAcceptWindow(QWidget *parent, const QString &fileName, const QString &toolName) {

    QMessageBox msgBox(parent);
    msgBox.setWindowTitle("Подтверждение действия");
    msgBox.setText(QString("Вы уверены, что хотите импортировать файл \"%1\" в инструмент \"%2\"?").arg(fileName).arg(toolName));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setIcon(QMessageBox::Question);

    msgBox.button(QMessageBox::Yes)->setText("Да");
    msgBox.button(QMessageBox::No)->setText("Нет");

    return msgBox.exec() == QMessageBox::Yes;
}
