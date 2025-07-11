//
// Created by Stalker7274 on 10.07.2025.
//

#ifndef CUSTOMACCEPTMESSAGEBOX_H
#define CUSTOMACCEPTMESSAGEBOX_H

#include <QDialog>


class QLabel;
QT_BEGIN_NAMESPACE
namespace Ui { class customAcceptMessageBox; }
QT_END_NAMESPACE

class customAcceptMessageBox : public QDialog {
Q_OBJECT

public:
    explicit customAcceptMessageBox(QString text, QWidget *parent = nullptr);
    ~customAcceptMessageBox() override;

public slots:
    void titleBarClose();

private:
    Ui::customAcceptMessageBox *ui;

    QLabel* textLabel;
    QPushButton* okButton;
    QPushButton* cancelButton;
};


#endif //CUSTOMACCEPTMESSAGEBOX_H
