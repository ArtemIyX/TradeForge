//
// Created by Stalker7274 on 06.07.2025.
//

#ifndef MESSAGEBOXFACTORY_H
#define MESSAGEBOXFACTORY_H
#include <qprogressdialog.h>
#include <QWidget>

class customProgressMessageBox;

class messageBoxFactory {

public:

    static void showInfo(QWidget* parent, const QString& title, const QString& message);

    static void showWarning(QWidget* parent, const QString& title, const QString& message);

    static void showError(QWidget* parent, const QString& title, const QString& message);

    static bool showAcceptWindow(QWidget *parent, const QString &text);

    static customProgressMessageBox* showProgressWindow(const QString& text);
    static void hideProgressWindow();

    static customProgressMessageBox* progressDialog;
};

#endif //MESSAGEBOXFACTORY_H
