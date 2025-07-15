#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStackedWidget>
#include <QDebug>
#include <QMessageBox>
// #include <QDataStream>


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
    void on_pushButtonLedON_clicked();

    void on_pushButtonLedOFF_clicked();

    void on_pushButtonSendData_clicked();

    void on_pushButtonConnectCOMport_clicked();

    void printComPorts();

    void on_horizontalSliderPWM_valueChanged(int value);

    void receive_Data();

    void on_pushButtonClearTextEdit_clicked();

    void on_secondPart_clicked(bool checked);

    void on_updateComPorts_clicked();

    // bool checkComPort();

    void on_horizontalSliderDelay_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    bool stateLed = false;
    int pwm = 0;
    int delay = 0;
    int indexPageButtonSendData;
    int indexPagepushButtonLedON;
    int indexPagepushButtonLedOFF;
    int clickConnectButton = 0;
    bool IsDataReceived;
    bool flagConnect;
    QStackedWidget *stackedWidget;
    QString dataFromSerial;
    QString dataNumPage;
    QString sep = ";";
    QString stateLedStr;

};
#endif // MAINWINDOW_H
