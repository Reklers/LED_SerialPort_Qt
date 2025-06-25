#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QMessageBox>
#include <QByteArray>
#include <QDataStream>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);

    ui->horizontalSliderPWM->setMinimum(0);
    ui->horizontalSliderPWM->setMaximum(100);
    ui->Connection->setReadOnly(true);
    ui->textEdit->setReadOnly(true);

    QList<QSerialPortInfo> availiblePorts = QSerialPortInfo::availablePorts(); // Получаем список COM портов
    ui->textEdit->append("Подключенные COM порта:");
    for(const QSerialPortInfo &portInfo : availiblePorts){
        ui->ComPort->addItem(portInfo.portName()); // Забираем только номера портов для подключения в QComboBox

        // Выводим данные о подключенных COM портах в QTextEdit
        QString portStr = QString("Порт: %1 %2")
                              .arg(portInfo.portName())
                              .arg(portInfo.description());

        ui->textEdit->append(portStr);
    }


}

MainWindow::~MainWindow()
{
    delete ui;
    serial->close();
    delete serial;
}

void MainWindow::on_pushButtonLedON_clicked()
{
    if (serial->isOpen()){
        stateLed = true;
        QString stateLedChar = "1";
        indexPagepushButtonLedON = ui->stackedWidget->currentIndex();
        dataNumPage = QString::number(indexPagepushButtonLedON);
        QByteArray byteArray = dataNumPage.toUtf8() + sep.toUtf8() + stateLedChar.toUtf8() + sep.toUtf8() + "-1" ;
        serial->write(byteArray);
        ui->label->setText("Светодиод включен");
    }
    else{
        QMessageBox::warning(this, "Ошибка подключения", "COM port не подключен");
    }
}


void MainWindow::on_pushButtonLedOFF_clicked()
{
    if (serial->isOpen()){
        stateLed = false;
        QString stateLedChar = "0";
        indexPagepushButtonLedOFF = ui->stackedWidget->currentIndex();
        dataNumPage = QString::number(indexPagepushButtonLedOFF);
        QByteArray byteArray = dataNumPage.toUtf8() + sep.toUtf8() + stateLedChar.toUtf8() + sep.toUtf8() + "-1" ;
        serial->write(byteArray);
        ui->label->setText("Светодиод выключен");
    }
    else{
        QMessageBox::warning(this, "Ошибка подключения", "COM port не подключен");
    }

}


// Кнопка перехода на страницу первой части тестового задания
void MainWindow::on_pushButtonSecondPart_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButtonConnectCOMport_clicked()
{

    serial->setPortName(ui->ComPort->currentText()); // выбор ком порта
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);

    connect(serial,&QSerialPort::readyRead,this, &MainWindow::receive_Data); // Без этой строчки не работает прием данных
    ui->pushButtonConnectCOMport->setText("Connect");

    if (serial->isOpen()) {
        ui->Connection->setText("Serial port подключен");
        ui->statusbar->showMessage(ui->ComPort->currentText());
        ui->textEdit->clear();
        ui->pushButtonConnectCOMport->setText("Disconnect");
        clickConnectButton += 1;
    }
    else{
        ui->Connection->setText("Serial port не подключен");
        ui->statusbar->clearMessage();
    }

    if (clickConnectButton % 2 == 0){
        serial->close();
        ui->ComPort->clear();
        QList<QSerialPortInfo> availiblePorts = QSerialPortInfo::availablePorts(); // Получаем список COM портов
        for(const QSerialPortInfo &portInfo : availiblePorts){
            ui->ComPort->addItem(portInfo.portName()); // Забираем только номера портов для подключения в QComboBox

            // Выводим данные о подключенных COM портах в QTextEdit
            QString portStr = QString("Порт: %1 %2")
                                  .arg(portInfo.portName())
                                  .arg(portInfo.description());

            ui->textEdit->append(portStr);
        }
        ui->pushButtonConnectCOMport->setText("Connect");
        ui->Connection->setText("Serial port отключен");
        ui->statusbar->clearMessage();
    }
}


// void MainWindow::on_pushButtonDisconnectCOMport_clicked()
// {
//     serial->close();
//     ui->Connection->setText("Serial port отключен");
//     ui->statusbar->clearMessage();
// }




// Кнопка очистки поля вывода информации
void MainWindow::on_pushButtonClearTextEdit_clicked()
{
    if (serial->isOpen()){
        ui->textEdit->clear();
    }
}


//---------------Вторая страница(ч.2 задания)---------------//


long mapValue(long value, long in_min, long in_max, long out_min, long out_max){
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void MainWindow::receive_Data()
{
    // TODO добавить в запись разработки сравнение двух вариантов функции чтения данных из COM порта
    // if (serial->isOpen()){
    //     QByteArray data = serial->readAll();
    //     QString receivedData = QString::fromUtf8(data);
    //     ui->textEdit->append(receivedData);
    // }


    // Вариант 2 (лучше)
    if (serial->isOpen()){
        while (serial->bytesAvailable()){
            dataFromSerial += serial->readAll();
            if (dataFromSerial.at(dataFromSerial.length() - 1) == char(10)){
                IsDataReceived = true;
            }
            if (IsDataReceived == true){
                ui->textEdit->append(dataFromSerial);
                dataFromSerial = "";
                IsDataReceived = false;
            }
        }
    }

}

// TODO переделать кнопку под 2-ю часть отправки данных
void MainWindow::on_pushButtonSendData_clicked()
{
    if (serial->isOpen()){
        indexPageButtonSendData = ui->stackedWidget->currentIndex();
        dataNumPage = QString::number(indexPageButtonSendData);
        if (stateLed){
            QString dataPwm = QString::number(pwm);
            QByteArray byteArray = dataNumPage.toUtf8() + ";" + stateLedStr.toUtf8() + ";" + dataPwm.toUtf8() + "-1";
            serial->write(byteArray);
            serial->waitForBytesWritten(-1);
        }
        else {
            QByteArray byteArray = dataNumPage.toUtf8() + ";" + stateLedStr.toUtf8() + ";" + "-1";
            serial->write(byteArray);
        }


    }
}


// Слайдер для установки PWM
void MainWindow::on_horizontalSliderPWM_valueChanged(int value)
{
    pwm = mapValue(value,0,100,0,255);
    // qDebug() << pwm;
}


void MainWindow::on_pushButtonFirstPart_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButtonLedON_2_clicked()
{
    stateLed = true;
    stateLedStr = "1";
    ui->labelStateLed2->setText("Светодиод включен");
}


void MainWindow::on_pushButtonLedOFF_2_clicked()
{
    stateLed = false;
    stateLedStr = "0";
    ui->labelStateLed2->setText("Светодиод выключен");
}

