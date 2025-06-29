//
// Created by Stalker7274 on 30.06.2025.
//

#ifndef DOWNLOADCSVWINDOW_H
#define DOWNLOADCSVWINDOW_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class downloadCSVWindow; }
QT_END_NAMESPACE

class downloadCSVWindow : public QDialog {
Q_OBJECT

public:
    explicit downloadCSVWindow(QString Path = "", QWidget *parent = nullptr);
    ~downloadCSVWindow() override;

public slots:

    void acceptClicked();
    void cancelClicked();

private:
    Ui::downloadCSVWindow *ui;

    QString symbolPath;
};


#endif //DOWNLOADCSVWINDOW_H
