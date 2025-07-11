//
// Created by Stalker7274 on 10.07.2025.
//

#ifndef CUSTOMPROGRESSMESSAGEBOX_H
#define CUSTOMPROGRESSMESSAGEBOX_H

#include <QDialog>


class QLabel;
QT_BEGIN_NAMESPACE
namespace Ui { class customProgressMessageBox; }
QT_END_NAMESPACE

class customProgressMessageBox : public QDialog {
Q_OBJECT

public:
    explicit customProgressMessageBox(const QString &text, QWidget *parent = nullptr);
    ~customProgressMessageBox() override;

public slots:
    void titleBarClose();

private:
    Ui::customProgressMessageBox *ui;

    QLabel* textLabel;
};


#endif //CUSTOMPROGRESSMESSAGEBOX_H
