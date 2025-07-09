//
// Created by Stalker7274 on 05.07.2025.
//

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <qtextedit.h>

inline QTextEdit *globalTextEdit = nullptr;
inline QString logPath = QDir::currentPath() + "/logs/TradeForgeLog.log";

inline void createLog() {

    QDir().mkpath(QDir::currentPath() + "/logs");
    QFile file(logPath);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.close();
}

inline void customMessageHandler(const QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString formattedMsg;
    switch (type) {
        case QtDebugMsg:
            formattedMsg = QString("[%1] DEBUG: %2").arg(timestamp, msg);
        break;
        case QtWarningMsg:
            formattedMsg = QString("[%1] WARNING: %2").arg(timestamp, msg);
        break;
        case QtCriticalMsg:
            formattedMsg = QString("[%1] CRITICAL: %2").arg(timestamp, msg);
        break;
        case QtFatalMsg:
            formattedMsg = QString("[%1] FATAL: %2").arg(timestamp, msg);
        break;
        default:
            formattedMsg = QString("[%1] UNKNOWN: %2").arg(timestamp, msg);
    }

    fprintf(stderr, "%s\n", qPrintable(formattedMsg));

    if (globalTextEdit) {
        globalTextEdit->append(formattedMsg);
    }

    QDir().mkdir(logPath);

    QFile logFile(logPath);
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << formattedMsg << "\n";
        logFile.close();
    }
}

inline void createLogBackup() {

    const QString timestamp = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss");

    const QFileInfo fileInfo(logPath);
    const QString backupFilePath = QString("%1/%2-%3.%4")
                            .arg(fileInfo.absolutePath())
                            .arg(fileInfo.baseName())
                            .arg(timestamp)
                            .arg(fileInfo.completeSuffix());

    const QFile originalFile(logPath);
    if (originalFile.exists()) {
        if (!QFile::copy(logPath, backupFilePath)) {
            qDebug() << "Failed to create backup of" << logPath << "to" << backupFilePath;
        } else {
            qDebug() << "Backup created:" << backupFilePath;
        }
    } else {
        qDebug() << "Original file does not exist:" << logPath;
    }
    QFile::remove(logPath);
}

#endif
