//
// Created by Stalker7274 on 25.06.2025.
//

#include "tableSymbolsStyledDelegate.h"

#include <QLineEdit>

QWidget * tableSymbolsStyledDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                                   const QModelIndex &index) const {

    QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);

    if (auto* lineEdit = qobject_cast<QLineEdit*>(editor)) {
        lineEdit->setStyleSheet("color: white; background-color: #2e2e2e; border: none; ");
    }

    return editor;
}
