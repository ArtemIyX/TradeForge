//
// Created by Stalker7274 on 29.06.2025.
//

#ifndef IMPORTFILESWINDOW_H
#define IMPORTFILESWINDOW_H

#include <QDialog>
#include <QSpacerItem>


class fileToImport;
class QVBoxLayout;
class QTableWidgetItem;
struct symbolSettings;
QT_BEGIN_NAMESPACE
namespace Ui { class importFilesWIndow; }
QT_END_NAMESPACE

class importFilesWIndow : public QDialog {
Q_OBJECT

public:
    explicit importFilesWIndow(QWidget *parent = nullptr);
    ~importFilesWIndow() override;

    QString currentFolder;

public slots:

    void addFilesToImport(bool checked);
    void startImport(bool checked);
    void cancelImport(bool checked);

    void importFileDeleted(const fileToImport* widget);

    void settingsTableChanged(const QTableWidgetItem *item);

private:
    Ui::importFilesWIndow *ui;

    QList<symbolSettings> currentFilesSettings;
    QList<QString> currentFiles;

    QVBoxLayout *filesToImportScrollLayout;

    QSpacerItem *spacer;
};


#endif //IMPORTFILESWINDOW_H
