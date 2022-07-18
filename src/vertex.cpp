#include "vertex.h"

Vertex::Vertex()
{
}

Vertex::Vertex(QVector3D coords, QVector3D normal, QVector2D textureCoords)
{
    this->coords = coords;
    this->normal = normal;
    this->textureCoords = textureCoords;
}
