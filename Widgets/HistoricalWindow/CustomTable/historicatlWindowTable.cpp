//
// Created by Stalker7274 on 25.06.2025.
//

#include "historicatlWindowTable.h"

void historicalWindowTable::commitData(QWidget *editor) {

    QModelIndex index = currentIndex();
    QTableWidget::commitData(editor);

    emit cellEditingFinished(index.row(), index.column());
}
