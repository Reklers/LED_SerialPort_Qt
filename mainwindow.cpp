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

    QList<QSerialPortInfo> availiblePorts = QSerialPortInfo::availablePorts(); // Получаем список COM портов

    ui->Connection->setReadOnly(true);
    ui->textEdit->setReadOnly(true);

    ui->textEdit->append("Подключенные COM порта:");
    for(const QSerialPortInfo &portInfo : availiblePorts){
        ui->ComPort->addItem(portInfo.portName()); // Забираем только номера портов для подключения в QComboBox

        // Выводим данные о подключенных COM портах в QTextEdit
        QString portStr = QString("Порт: %1 %2")
                              .arg(portInfo.portName())
                              .arg(portInfo.description());

        ui->textEdit->append(portStr);

        ui->horizontalSliderPWM->setMinimum(0);
        ui->horizontalSliderPWM->setMaximum(255);
        // ui->labelPWM->
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
        serial->write("1");
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
        serial->write("0");
        ui->label->setText("Светодиод выключен");
        ui->statusbar->clearMessage();
    }
    else{
        QMessageBox::warning(this, "Ошибка подключения", "COM port не подключен");
    }

}

// TODO переделать кнопку под 2-ю часть отправки данных
void MainWindow::on_pushButtonSendData_clicked()
{
    if (serial->isOpen()){

        QString dataToSend = QString::number(pwm);
        QByteArray byteArray = dataToSend.toUtf8();
        serial->write(byteArray);
        serial->waitForBytesWritten(-1);

    }
    // if (stateLed){
    //     serial->write("1");
    //     ui->label->setText("Светодиод включен");
    // }
    // else{
    //     serial->write("0");
    //     ui->label->setText("Светодиод выключен");
    // }
}

// Кнопка перехода на страницу первой части тестового задания
void MainWindow::on_pushButtonSecondPart_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButtonFirstPart_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
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

    if (serial->isOpen()){
        ui->Connection->setText("Serial port подключен");
        ui->statusbar->showMessage(ui->ComPort->currentText());
        ui->textEdit->clear();
    }
    else{
        ui->Connection->setText("Serial port не подключен");
        ui->statusbar->clearMessage();
    }
}


void MainWindow::on_pushButtonDisconnectCOMport_clicked()
{
    serial->close();
    ui->Connection->setText("Serial port отключен");
    ui->statusbar->clearMessage();
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

// Слайдер для установки PWM
void MainWindow::on_horizontalSliderPWM_valueChanged(int value)
{
    pwm = value;
}

//---------------Вторая страница(ч.2 задания)---------------//

// Кнопка очистки поля вывода информации
void MainWindow::on_pushButtonClearTextEdit_clicked()
{
    if (serial->isOpen()){
        ui->textEdit->clear();
    }
}


void MainWindow::on_pushButtonLedON_2_clicked()
{
    stateLed = true;
}


void MainWindow::on_pushButtonLedOFF_2_clicked()
{
    stateLed = false;
}

