//
// Created by Stalker7274 on 02.07.2025.
//

#include "custommessagebox.h"

#include <QLabel>
#include <QVBoxLayout>

customMessageBox::customMessageBox(QWidget *parent) :
    QDialog(parent) {

    setLayout(new QVBoxLayout());

    layout()->addWidget(new QLabel("Wait please"));
}

customMessageBox::~customMessageBox() {

}
