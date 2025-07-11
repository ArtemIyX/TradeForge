//
// Created by Stalker7274 on 10.07.2025.
//

#ifndef CUSTOMINFOMESSAGEBOX_H
#define CUSTOMINFOMESSAGEBOX_H

#include <QDialog>


class QLabel;
QT_BEGIN_NAMESPACE
namespace Ui { class customInfoMessageBox; }
QT_END_NAMESPACE

class customInfoMessageBox : public QDialog {
Q_OBJECT

public:
    explicit customInfoMessageBox(QString text, QWidget *parent = nullptr);
    ~customInfoMessageBox() override;

public slots:
    void titleBarClose();

private:
    Ui::customInfoMessageBox *ui;

    QLabel* textLabel;
    QPushButton* okButton;
};


#endif //CUSTOMINFOMESSAGEBOX_H
