#include "custommovement.h"
#include <QtMath>
#include <QString>

customMovement::customMovement(float movement)
{
    if(movement < 0){
        direction = 1;
        movement *= -1; //make it positive so math works
        tenMoves = qFloor(movement / 10);
        oneMoves = qFloor(movement - (tenMoves * 10));
        smallMoves = qRound(movement - (tenMoves * 10) - (oneMoves) / 0.12); //smallest movement is .12
    }else if(movement > 0){
        direction = 0;
        tenMoves = qFloor(movement / 10);
        oneMoves = qFloor(movement - (tenMoves * 10));
        smallMoves = qRound(movement - (tenMoves * 10) - (oneMoves) / 0.12); //smallest movement is .12
    }else{
        direction = 0; tenMoves = 0; oneMoves = 0; smallMoves = 0;
    }

}

QString customMovement::createMovementString(){

    QString movementString = "";
    QString dir = "";
    if(direction == 0){
        dir = "+";
    }else {
        dir = "-";
    }
    for(int i = 0; i < tenMoves; ++i){
        movementString += (dir + "10");
    }
    for(int i = 0; i < oneMoves; ++i){
        movementString += (dir + "01");
    }
    for(int i = 0; i < smallMoves; ++i){
        movementString += (dir + "11");
    }
    return movementString;
}



