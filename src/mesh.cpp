#include "mesh.h"

Mesh::Mesh(QString Name)
{
    id = 1;
    name = Name;

    position.setX(0);
    position.setY(0);
    position.setZ(0);

    rotation.setX(0);
    rotation.setY(0);
    rotation.setZ(0);

    scaling.setX(1);
    scaling.setY(1);
    scaling.setZ(1);

    shearing.setX(0);
    shearing.setY(0);
    shearing.setZ(0);
}
