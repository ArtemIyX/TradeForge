//
// Created by Stalker7274 on 02.07.2025.
//

#ifndef CUSTOMMESSAGEBOX_H
#define CUSTOMMESSAGEBOX_H

#include <QDialog>

class customMessageBox : public QDialog {
Q_OBJECT

public:
    explicit customMessageBox(QWidget *parent = nullptr);
    ~customMessageBox() override;

private:

};


#endif //CUSTOMMESSAGEBOX_H
