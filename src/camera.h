#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <QVector3D>
#include <QString>

class Camera
{
public:
    Camera();
    Camera(QVector3D position, QVector3D target, QVector3D up);

    QString toString(QVector3D vector);

    QVector3D position;
    QVector3D target;
    QVector3D up;
};

#endif // CAMERA_H
