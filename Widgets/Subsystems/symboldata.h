//
// Created by Stalker7274 on 04.07.2025.
//

#ifndef SYMBOLDATA_H
#define SYMBOLDATA_H

#include <QObject>

struct historicalCSVStroke;

class symbolData : public QObject {
Q_OBJECT

public:
    explicit symbolData(QString path, QObject *parent = nullptr);
    ~symbolData() override;

    bool getState() const {return isLoaded;}

    QList<historicalCSVStroke> getData() const { return data; }

signals:
    void strokeLoaded(historicalCSVStroke stroke);

private:

    void startLoading();

    QString filePath;
    bool isLoaded = false;
    QList<historicalCSVStroke> data;
};


#endif //SYMBOLDATA_H
