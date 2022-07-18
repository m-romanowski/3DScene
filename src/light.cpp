#include "light.h"

Light::Light()
{
    light.setX(0);
    light.setY(0);
    light.setZ(0);
}

Light::Light(QVector3D light)
{
    this->light = light;
}

QString Light::toString()
{
    return QString::number(this->light.x()) + " " + QString::number(this->light.y()) + " " + QString::number(this->light.z());
}
