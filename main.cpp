#include "cameracontrolui.h"
#include "enums.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    try{
        CameraControlUI w;
    }catch(std::runtime_error& e){ //if arduino is not found this will trip
        QMessageBox Msgbox;
            Msgbox.setText(e.what());
            Msgbox.setIcon(QMessageBox::Icon::Critical);
            Msgbox.exec();
        return static_cast<int>(values::FAILURE);
    }
    //the window will not persist if w.show() is called in the try block
    //so re-create w now that we know it will be successful
    CameraControlUI w;
    w.show();
    return a.exec();
}
