//
// Created by Stalker7274 on 25.06.2025.
//

#ifndef TABLESYMBOLSSTYLEDDELEGATE_H
#define TABLESYMBOLSSTYLEDDELEGATE_H
#include <QStyledItemDelegate>


class tableSymbolsStyledDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;
};



#endif //TABLESYMBOLSSTYLEDDELEGATE_H
