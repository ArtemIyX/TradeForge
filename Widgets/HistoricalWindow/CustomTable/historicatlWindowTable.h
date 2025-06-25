//
// Created by Stalker7274 on 25.06.2025.
//

#ifndef HISTORICATLWINDOWTABLE_H
#define HISTORICATLWINDOWTABLE_H
#include <QTableWidget>


class historicalWindowTable : public QTableWidget {
    Q_OBJECT
public:
    historicalWindowTable(QWidget* parent = nullptr) : QTableWidget(parent) {}

protected:
    void commitData(QWidget* editor) override;

    signals:
        void cellEditingFinished(int row, int column);
};



#endif //HISTORICATLWINDOWTABLE_H
