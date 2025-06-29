//
// Created by Stalker7274 on 29.06.2025.
//

#ifndef FILETOIMPORT_H
#define FILETOIMPORT_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class fileToImport; }
QT_END_NAMESPACE

class fileToImport : public QWidget {
Q_OBJECT

public:
    explicit fileToImport(QString Path = "", QWidget *parent = nullptr);
    ~fileToImport() override;

    QString filePath;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void showInExplorer();
    void deleteFile();

private:
    Ui::fileToImport *ui;
};


#endif //FILETOIMPORT_H
