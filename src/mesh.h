#ifndef MESH_H
#define MESH_H

#include <QVector3D>

#include <face.h>
#include <texture.h>
#include <vertex.h>
#include <animation.h>

// Structure for storing values ​​when drawing lines of individual triangles solid object faces
struct lineData
{
    int currentY;
    float ndotla;
    float ndotlb;
    float ndotlc;
    float ndotld;

    float ua;
    float ub;
    float uc;
    float ud;

    float va;
    float vb;
    float vc;
    float vd;
};

class Mesh
{
public:
    Mesh(QString Name);

    int id;
    QString name;
    QVector3D position;
    QVector3D rotation;
    QVector3D scaling;
    QVector3D shearing;

    Animation animation;
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    Texture texture;
};

#endif // MESH_H
