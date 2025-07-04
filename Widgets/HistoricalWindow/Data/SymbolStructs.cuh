//
// Created by Stalker7274 on 25.06.2025.
//

#ifndef SYMBOLSTRUCTS_CUH
#define SYMBOLSTRUCTS_CUH
#include <QString>
#include <QVariant>
#include <QDateTime>

struct symbolSettings {

    QString settingName = "";
    QVariant settingValue = QVariant();

    symbolSettings() = default;

    symbolSettings(const QString &name, const QVariant &value)
        : settingName(name), settingValue(value) {}

    friend QDataStream& operator<<(QDataStream& stream, const symbolSettings& setting) {
        stream << setting.settingName << setting.settingValue;
        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, symbolSettings& setting) {
        stream >> setting.settingName >> setting.settingValue;
        return stream;
    }
};

#pragma pack(push, 1)
struct historicalCSVStroke{

    qint64 timestamp;
    double open;
    double high;
    double low;
    double close;
    qint64 volume;

    historicalCSVStroke() = default;

    historicalCSVStroke(const QDateTime& date, double open, double high, double low, double close, qint64 volume)
        : timestamp(date.toSecsSinceEpoch()), open(open), high(high), low(low), close(close), volume(volume) {}

    QDateTime getDate() const {
        return QDateTime::fromSecsSinceEpoch(timestamp);
    }

    bool isValid() const {
        return timestamp != 0 and open != 0 and high != 0 and low != 0 and close != 0; ;
    }
};
#pragma pack(pop)

#endif //SYMBOLSTRUCTS_CUH
