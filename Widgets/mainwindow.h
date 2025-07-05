#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    QPointer<QDialog> historicalMenuWidget = nullptr;
    QPointer<QWidget> terminalWidget = nullptr;

    QList<QObject*> subsystems = {};
};
#endif // MAINWINDOW_H
