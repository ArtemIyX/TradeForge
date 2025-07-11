//
// Created by Stalker7274 on 23.06.2025.
//

#ifndef CUSTOMSTYLEDITEMDELEGATE_H
#define CUSTOMSTYLEDITEMDELEGATE_H

#include <QStyledItemDelegate>


class customStyledItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

signals:

    void itemDataChanged(const QModelIndex &index);

private:

    void callItemDataChanged(const QModelIndex &index);
};



#endif //CUSTOMSTYLEDITEMDELEGATE_H
