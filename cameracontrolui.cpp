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

    ui->doubleSpinBox->setDecimals(2);
    ui->doubleSpinBox->setSingleStep(5);
    ui->doubleSpinBox->setRange(0,300);


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
        if(arduino->open(QSerialPort::ReadWrite)){
            arduino->setBaudRate(QSerialPort::Baud9600);
            arduino->setDataBits(QSerialPort::Data8);
            arduino->setParity(QSerialPort::NoParity);
            arduino->setStopBits(QSerialPort::OneStop);
            arduino->setFlowControl(QSerialPort::HardwareControl);
            QObject::connect(arduino,SIGNAL(readyRead()), this, SLOT(readSerialPort()));
            qDebug() << arduino->error();

            //no clue why but close port and re-open to fix some boot issues with arduino
            Sleep(10);
            arduino->close();
            Sleep(10);

            arduino->open(QSerialPort::ReadWrite);
            arduino->setBaudRate(QSerialPort::Baud9600);
            arduino->setDataBits(QSerialPort::Data8);
            arduino->setParity(QSerialPort::NoParity);
            arduino->setStopBits(QSerialPort::OneStop);
            arduino->setFlowControl(QSerialPort::HardwareControl);
        }else{
            throw(std::runtime_error("Could not open serial port. Killing program."));
        }

    }else{
        throw(std::runtime_error("Could not find correct Arduino. Killing program."));
    }

    disableAllButtons();
    ui->calibrateButton->setEnabled(true); //we want to force calibration
    ui->currentPosValue->setText("UNKNOWN");
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
            currentPosition += 10;
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveDown10_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("-10");
            currentPosition -= 10;
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveUp1_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("+01");
            currentPosition += 1;
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveDown1_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("-01");
            currentPosition -= 1;
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveUpPoint1_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("+11");
            currentPosition += 0.12;
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveDownPoint1_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("-11");
            currentPosition -= 0.12;
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}


void CameraControlUI::on_singleStepUpButton_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("+04");
            currentPosition += 0.04;
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_singleStepDownButton_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("-04");
            currentPosition -= 0.04;
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::readSerialPort(){
    QByteArray data = arduino->readAll();
    serialBuffer += QString::fromStdString(data.toStdString()); //need to do a bit more here, also not sure if return values are even useful?
    QStringList values = serialBuffer.split(','); //hard coded arduino has to print "Success,"
    qDebug() << values;

    if(values.contains("Success")){
        enableAllButtons();
        serialBuffer = "";
        ui->currentPosValue->setText(QString::number(currentPosition));
        qDebug() << "Full value found, reseting";
    }else if(values.contains("Invalid")){
        QMessageBox Msgbox;
            Msgbox.setText("Position out of bounds. Either re-calibrate or pick a valid position. \n"
            " Max: 250, Min: 5.");
            Msgbox.setIcon(QMessageBox::Icon::Critical);
            Msgbox.exec();
        enableAllButtons();
        serialBuffer = "";
    }else if(values.contains("Unknown")){
        QMessageBox Msgbox;
            Msgbox.setText("Arduino failed to parse. Try again");
            Msgbox.setIcon(QMessageBox::Icon::Critical);
            Msgbox.exec();
        enableAllButtons();
        serialBuffer = "";
    }else{
        QMessageBox Msgbox;
            Msgbox.setText("Unknown error do you wish to continue?");
            Msgbox.setIcon(QMessageBox::Icon::Critical);
            Msgbox.exec();
        enableAllButtons();
        serialBuffer = "";
    }

}

void CameraControlUI::disableAllButtons(){
    ui->moveDown1->setDisabled(true);
    ui->moveUp1->setDisabled(true);
    ui->moveDown10 ->setDisabled(true);
    ui->moveUp10->setDisabled(true);
    ui->moveDownPoint1->setDisabled(true);
    ui->moveUpPoint1->setDisabled(true);
    ui->doubleSpinBox->setDisabled(true);
    ui->moveButton->setDisabled(true);
    ui->calibrateButton->setDisabled(true);
}

void CameraControlUI::enableAllButtons(){
    ui->moveDown1->setDisabled(false);
    ui->moveUp1->setDisabled(false);
    ui->moveDown10 ->setDisabled(false);
    ui->moveUp10->setDisabled(false);
    ui->moveDownPoint1->setDisabled(false);
    ui->moveUpPoint1->setDisabled(false);
    ui->doubleSpinBox->setDisabled(false);
    ui->moveButton->setDisabled(false);
    ui->calibrateButton->setDisabled(false);
}

void CameraControlUI::on_calibrateButton_clicked()
{
    disableAllButtons();
    if(arduino->isWritable()){
            arduino->write("Cal");
            currentPosition = 5; //Arduino sets it to 5 will change if value changes there.
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}

void CameraControlUI::on_moveButton_clicked()
{
    float desiredPosition = ui->doubleSpinBox->value();
    float movement = desiredPosition - currentPosition;
    QString valueToWrite = "";

    if(movement <= 0){
        valueToWrite += "-";
        valueToWrite += QString::number(qAbs(movement));
    }else{
        valueToWrite += "+";
        valueToWrite += QString::number(qAbs(movement));
    }
    qDebug() << valueToWrite;

    if(arduino->isWritable()){
            arduino->write(valueToWrite.toLocal8Bit());
        }else{
            qDebug() << "Couldn't write to serial!";
        }
}



