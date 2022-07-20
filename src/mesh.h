#ifndef MESH_H
#define MESH_H

#include <QVector3D>

#include <face.h>
#include <texture.h>
#include <vertex.h>
#include <animation.h>

class Mesh
{
public:
    // Structure for storing values when drawing lines of individual triangles solid object faces
    struct Edge
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

    Mesh(QString name);

    int id;
    QString name;
    QVector3D position;
    QVector3D rotation;
    QVector3D scale;
    QVector3D skew;
    Animation animation;
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    Texture texture;
};

#endif // MESH_H
