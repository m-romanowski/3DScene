#ifndef VERTEX_H
#define VERTEX_H

#include <QVector2D>
#include <QVector3D>

class Vertex
{
public:
    Vertex();
    Vertex(QVector3D coords, QVector3D normal, QVector2D textureCoords);

    QVector3D normal;
    QVector3D coords;
    QVector3D worldCoords;
    QVector2D textureCoords;
};

#endif // VERTEX_H
