//
// Created by Stalker7274 on 23.06.2025.
//

#include "customStyledItemDelegate.h"

#include <QLineEdit>

void customStyledItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {

    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (lineEdit)
        lineEdit->setText(index.data().toString());
}

void customStyledItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {

    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (!lineEdit) {
        return;
    }

    if (lineEdit->text().isEmpty()) {

        model->removeRow(index.row());
    }else {

        model->setData(index, lineEdit->text());
    }
}
