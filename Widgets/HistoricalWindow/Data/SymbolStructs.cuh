//
// Created by Stalker7274 on 25.06.2025.
//

#ifndef SYMBOLSTRUCTS_CUH
#define SYMBOLSTRUCTS_CUH
#include <QString>
#include <QVariant>

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

#endif //SYMBOLSTRUCTS_CUH
