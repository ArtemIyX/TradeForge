//
// Created by Stalker7274 on 06.07.2025.
//

#ifndef MESSAGEBOXFACTORY_H
#define MESSAGEBOXFACTORY_H
#include <qprogressdialog.h>
#include <QWidget>


class messageBoxFactory {

public:

    static void showInfo(QWidget* parent, const QString& title, const QString& message);

    // Предупреждение с кнопкой "OK"
    static void showWarning(QWidget* parent, const QString& title, const QString& message);

    // Уведомление об ошибке с кнопкой "OK"
    static void showError(QWidget* parent, const QString& title, const QString& message);

    static bool showAcceptWindow(QWidget* parent, const QString& fileName, const QString& toolName);

    static QProgressDialog* showProgressWindow(QWidget* parent, const QString& title, const QString& message);
};

#endif //MESSAGEBOXFACTORY_H
