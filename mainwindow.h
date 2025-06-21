#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStackedWidget>
#include <QDebug>
#include <QMessageBox>


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

private slots:
    // void serialRecieve(); //получение данных

    void on_pushButtonLedON_clicked();

    void on_pushButtonLedOFF_clicked();

    void on_pushButton_3_clicked();

    void on_pushButtonSecondPart_clicked();

    void on_pushButtonFirstPart_clicked();

    void on_pushButtonConnectCOMport_clicked();

    void on_pushButtonDisconnectCOMport_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    bool stateLed;
    QStackedWidget *stackedWidget;
};
#endif // MAINWINDOW_H
