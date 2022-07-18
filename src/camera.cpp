#include "camera.h"

Camera::Camera()
{
}

Camera::Camera(QVector3D position, QVector3D target, QVector3D up)
{
    this->position = position;
    this->target = target;
    this->up = up;
}

QString Camera::toString(QVector3D vector)
{
    return QString::number(vector.x()) + " " + QString::number(vector.y()) + " " + QString::number(vector.z());
}
