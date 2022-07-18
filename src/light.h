#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>
#include <QString>

class Light
{
public:
    Light();
    Light(QVector3D light);

    QVector3D light;

    QString toString();
};

#endif // LIGHT_H
