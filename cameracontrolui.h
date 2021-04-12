#ifndef CAMERACONTROLUI_H
#define CAMERACONTROLUI_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class CameraControlUI; }
QT_END_NAMESPACE

class CameraControlUI : public QMainWindow
{
    Q_OBJECT

public:
    CameraControlUI(QWidget *parent = nullptr);
    ~CameraControlUI();

private slots:
    void on_moveDownPoint1_clicked();

    void on_moveUp10_clicked();

    void on_moveDown10_clicked();

    void on_moveUp1_clicked();

    void on_moveDown1_clicked();

    void on_moveUpPoint1_clicked();

    void readSerialPort();

    void on_calibrateButton_clicked();

    void on_moveButton_clicked();

private:
    Ui::CameraControlUI *ui;

    QSerialPort *arduino;
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_product_id = 32822;
    QString arduino_port_name;
    bool arduino_is_available;

    QString serialBuffer;

    float currentPosition = 0;

    void disableAllButtons();
    void enableAllButtons();
};
#endif // CAMERACONTROLUI_H
