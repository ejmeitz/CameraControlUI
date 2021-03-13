#include "cameracontrolui.h"
#include "ui_cameracontrolui.h"
#include "enums.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtWidgets>
#include <QDebug>
#include <QByteArray>

#include <string>

//constructor
CameraControlUI::CameraControlUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CameraControlUI)
{
    ui->setupUi(this);

    arduino_is_available = false;
    arduino_port_name = "";
    arduino = new QSerialPort;
    serialBuffer = "";


//    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();
//    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
//        qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
//        if(serialPortInfo.hasVendorIdentifier()){
//            qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
//        }
//        qDebug() << "Has Product ID: " << serialPortInfo.hasProductIdentifier();
//        if(serialPortInfo.hasProductIdentifier()){
//            qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
//        }
//    }


    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            if(serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id){
                if(serialPortInfo.productIdentifier() == arduino_uno_product_id){
                    arduino_port_name = serialPortInfo.portName();
                    arduino_is_available = true;
                }
            }
        }
    }

    if(arduino_is_available){
        // open and configure the serialport
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        QObject::connect(arduino,SIGNAL(readyRead()), this, SLOT(readSerialPort()));
    }else{
        // give error message if not available and stop construction of object
        throw(std::runtime_error("Could not find correct Arduino. Killing program."));
    }

}

CameraControlUI::~CameraControlUI()
{
    if(arduino->isOpen()){
           arduino->close();
    }
    delete ui;
}



void CameraControlUI::on_moveUp10_clicked(){
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("+10");
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveDown10_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("-10");
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveUp1_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("+1");
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveDown1_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("-1");
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveUpPoint1_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("+01");
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveDownPoint1_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("-01");
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::readSerialPort(){

    QByteArray data = arduino->readAll();
    serialBuffer += QString::fromStdString(data.toStdString()); //need to do a bit more here, also not sure if return values are even useful?
    QStringList values = serialBuffer.split(',');
    qDebug() << data;

    if(values[0] == "Success"){
        enableAllButtons();
        serialBuffer = "";
        qDebug() << "Full value found, reseting";
    }

}

void CameraControlUI::disableAllButtons(){
    ui->moveDown1->setDisabled(true);
    ui->moveUp1->setDisabled(true);
    ui->moveDown10 ->setDisabled(true);
    ui->moveUp10->setDisabled(true);
    ui->moveDownPoint1->setDisabled(true);
    ui->moveUpPoint1->setDisabled(true);
}

void CameraControlUI::enableAllButtons(){
    ui->moveDown1->setDisabled(false);
    ui->moveUp1->setDisabled(false);
    ui->moveDown10 ->setDisabled(false);
    ui->moveUp10->setDisabled(false);
    ui->moveDownPoint1->setDisabled(false);
    ui->moveUpPoint1->setDisabled(false);
}
