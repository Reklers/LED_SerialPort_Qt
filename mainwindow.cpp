#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort(this);
    serial->setPortName("com3");
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);
}

MainWindow::~MainWindow()
{
    delete ui;

    serial->close();
    delete serial;
}

void MainWindow::on_pushButton_clicked()
{
    ui->label->setText("Светодиод включен");
    serial->write("1");
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->label->setText("Светодиод выключен");
    serial->write("0");
}

