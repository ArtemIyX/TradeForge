//
// Created by Stalker7274 on 23.06.2025.
//

#include "customStyledItemDelegate.h"

#include <QLineEdit>

void customStyledItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {

    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (lineEdit) {
        lineEdit->setText(index.data().toString());
        lineEdit->setStyleSheet("color: rgb(255, 255, 255); border: none; background: #333333;");
    }
}

void customStyledItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {

    const QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (!lineEdit) {
        return;
    }

    if (lineEdit->text().isEmpty()) {

        model->removeRow(index.row());
    }else {

        model->setData(index, lineEdit->text());
        const_cast<customStyledItemDelegate*>(this)->callItemDataChanged(index);
    }
}

void customStyledItemDelegate::callItemDataChanged(const QModelIndex &index) {
    emit itemDataChanged(index);
}
