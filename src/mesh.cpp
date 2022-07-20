#include "mesh.h"

Mesh::Mesh(QString name)
    : id(1),
      name(name),
      position(QVector3D(0, 0, 0)),
      rotation(QVector3D(0, 0, 0)),
      scale(QVector3D(1, 1, 1)),
      skew(QVector3D(0, 0, 0))

{

}
