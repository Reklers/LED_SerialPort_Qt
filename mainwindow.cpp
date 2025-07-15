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

    // Настройка диапазона слайдера PWM
    ui->horizontalSliderPWM->setMinimum(0);
    ui->horizontalSliderPWM->setMaximum(100);
    ui->horizontalSliderPWM->setSingleStep(10);

    // Настройка диапазона слайдера Delay
    ui->horizontalSliderDelay->setMinimum(0);
    ui->horizontalSliderDelay->setMaximum(1000);
    ui->horizontalSliderDelay->setSingleStep(10);

    // Компоненты только для чтения
    ui->Connection->setReadOnly(true);
    ui->textEdit->setReadOnly(true);

    // Настройка видимости компонентов
    ui->pushButtonClearTextEdit->hide();
    ui->pushButtonSendData->hide();
    ui->horizontalSliderPWM->hide();
    ui->label_pwmLED->hide();
    ui->lcdNumber->hide();
    ui->labelDelay->hide();
    ui->lcdNumberDelay->hide();
    ui->horizontalSliderDelay->hide();

    // ToolTips
    ui->horizontalSliderDelay->setToolTip("Частота мигания в мкс");
    ui->secondPart->setToolTip("Вторая часть");
    ui->updateComPorts->setToolTip("Обновить список доступных COM-портов");

    // Размеры компонентов
    ui->pushButtonConnectCOMport->setFixedSize(82,24);

    printComPorts();
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
        if(ui->secondPart->isChecked()){
            stateLed = true;
            stateLedStr = "1";
        }else{ // Отправляем данные сразу (первая часть)
            dataNumPage = "0";
            QString stateLedChar = "1";
            QByteArray byteArray = dataNumPage.toUtf8() + sep.toUtf8() + stateLedChar.toUtf8() + sep.toUtf8() + "\r\n";
            serial->write(byteArray);
        }
        ui->label->setText("Светодиод включен");
    }
    else{
        QMessageBox::warning(this, "Ошибка подключения", "COM port не подключен");
    }
}


void MainWindow::on_pushButtonLedOFF_clicked()
{
    if (serial->isOpen()){
        if(ui->secondPart->isChecked()){
            stateLed = false;
            stateLedStr = "0";
        }else{
            dataNumPage = "0";
            QString stateLedChar = "0";
            QByteArray byteArray = dataNumPage.toUtf8() + sep.toUtf8() + stateLedChar.toUtf8() + sep.toUtf8() + "\r\n";
            serial->write(byteArray);
        }
        ui->label->setText("Светодиод выключен");
    }
    else{
        QMessageBox::warning(this, "Ошибка подключения", "COM port не подключен");
    }

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

    if (serial->isOpen()) {
        ui->Connection->setText("Serial port подключен");
        ui->statusbar->showMessage(ui->ComPort->currentText());
        ui->textEdit->clear();
        ui->textEdit->setText("Ответы от bluepill:");
        ui->pushButtonConnectCOMport->setText("Отключить");
        ui->pushButtonClearTextEdit->show();
        clickConnectButton += 1;
        ui->updateComPorts->hide();
    }
    else{
        ui->Connection->setText("Serial port не подключен");
        ui->statusbar->clearMessage();
        QMessageBox::warning(this, "Ошибка подключения", "COM port не подключен");
    }

    if (clickConnectButton % 2 == 0){
        serial->close();
        ui->ComPort->clear();
        printComPorts();
        ui->pushButtonConnectCOMport->setText("Подключить");
        ui->Connection->setText("Serial port отключен");
        ui->pushButtonClearTextEdit->hide();
        ui->statusbar->clearMessage();
        ui->updateComPorts->show();
        clickConnectButton = 0;
    }
}

// Кнопка очистки поля вывода информации
void MainWindow::on_pushButtonClearTextEdit_clicked()
{
    if (serial->isOpen()){
        ui->textEdit->clear();
    }
}


//---------------Вторая часть)---------------//
long mapValue(long value, long in_min, long in_max, long out_min, long out_max){
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void MainWindow::receive_Data()
{
    // Вариант 2 (лучше)
    if (serial->isOpen()){
        while (serial->bytesAvailable()){
            dataFromSerial += serial->readAll();
            if (dataFromSerial.at(dataFromSerial.length() - 1) == char(13) ||
                dataFromSerial.at(dataFromSerial.length() - 1) == char(10))
            {
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


void MainWindow::on_pushButtonSendData_clicked()
{
    if (serial->isOpen()){
        dataNumPage = "1";
        if (stateLed){
            QString dataPwm = QString::number(pwm);
            QString dataDelay = QString::number(delay);
            QByteArray byteArray = dataNumPage.toUtf8() + sep.toUtf8() + stateLedStr.toUtf8() +
                                   sep.toUtf8() + dataPwm.toUtf8() + sep.toUtf8() + dataDelay.toUtf8() + "\r\n";
            serial->write(byteArray);
        }
        else {
            QByteArray byteArray = dataNumPage.toUtf8() + sep.toUtf8() + stateLedStr.toUtf8() + sep.toUtf8() + "\r\n";
            serial->write(byteArray);
        }
    }
}


// Слайдер для установки PWM
void MainWindow::on_horizontalSliderPWM_valueChanged(int value)
{
    pwm = mapValue(value,0,100,0,999);
    ui->lcdNumber->display(value);
}

// Вывод информации о подключенных COM-портах
void MainWindow::printComPorts(){
    ui->textEdit->clear();
    ui->ComPort->clear();
    ui->textEdit->append("Подключенные COM порта:");
    QList<QSerialPortInfo> availiblePorts = QSerialPortInfo::availablePorts(); // Получаем список COM портов
    for(const QSerialPortInfo &portInfo : availiblePorts){
        ui->ComPort->addItem(portInfo.portName()); // Забираем только номера портов для подключения в QComboBox
        // Выводим данные о подключенных COM портах в QTextEdit
        QString portStr = QString("Порт: %1 %2")
                              .arg(portInfo.portName())
                              .arg(portInfo.description());
        ui->textEdit->append(portStr);
    }
}


void MainWindow::on_secondPart_clicked(bool checked)
{
    if(checked){
        ui->pushButtonSendData->show();
        ui->horizontalSliderPWM->show();
        ui->label_pwmLED->show();
        ui->lcdNumber->show();
        ui->labelDelay->show();
        ui->lcdNumberDelay->show();
        ui->horizontalSliderDelay->show();
        ui->label_2->setText("Вторая часть");
    }else{
        ui->pushButtonSendData->hide();
        ui->horizontalSliderPWM->hide();
        ui->label_pwmLED->hide();
        ui->lcdNumber->hide();
        ui->labelDelay->hide();
        ui->lcdNumberDelay->hide();
        ui->horizontalSliderDelay->hide();
        ui->label_2->setText("Первая часть");
    }
}


void MainWindow::on_updateComPorts_clicked()
{
    printComPorts();
}


void MainWindow::on_horizontalSliderDelay_valueChanged(int value)
{
    delay = mapValue(value,0,1000,10,1000);
    ui->lcdNumberDelay->display(value);
}

