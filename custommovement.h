#ifndef CUSTOMMOVEMENT_H
#define CUSTOMMOVEMENT_H
#include <QString>

class customMovement
{
public:
    customMovement(float);
    QString createMovementString();

private:
    int direction = 0; //0 for up 1 for down
    int tenMoves = 0;
    int oneMoves = 0;
    int smallMoves = 0;
};

#endif // CUSTOMMOVEMENT_H
