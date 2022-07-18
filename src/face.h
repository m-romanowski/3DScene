#ifndef FACE_H
#define FACE_H

#include <QVector3D>

class Face
{
public:
    Face();
    Face(int AValue, int BValue, int CValue);

    int A, B, C;
};

#endif // FACE_H
