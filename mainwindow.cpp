#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);

    QList<QSerialPortInfo> availiblePorts = QSerialPortInfo::availablePorts(); // Получаем список COM портов

    ui->Connection->setReadOnly(true);
    ui->Recived_RawData->setReadOnly(true);

    ui->Recived_RawData->append("Подключенные COM порта:");
    for(const QSerialPortInfo &portInfo : availiblePorts){
        ui->ComPort->addItem(portInfo.portName()); // Забираем только номера портов для подключения в QComboBox

        // Выводим данные о подключенных COM портах в QTextEdit
        QString portStr = QString("Порт: %1 %2")
                              .arg(portInfo.portName())
                              .arg(portInfo.description());

        ui->Recived_RawData->append(portStr);
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
void MainWindow::on_pushButton_3_clicked()
{
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



    if (serial->isOpen()){
        ui->Connection->setText("Serial port подключен");
        ui->statusbar->showMessage(ui->ComPort->currentText());
        // ui->statusbar->showMessage("Baud 115200");
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

