#ifndef PROJECTION_H
#define PROJECTION_H

#include <cmath>
#include <vertex.h>

#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>

class Projection
{
public:
    Projection();

    void normalize(QVector3D &point);
    QVector3D crossProduct(QVector3D point1, QVector3D point2);
    float dot(QVector3D point1, QVector3D point2);

    QMatrix4x4 get3DTranslation(float tx, float ty, float tz);
    QMatrix4x4 get3DXRotation(float alpha);
    QMatrix4x4 get3DYRotation(float alpha);
    QMatrix4x4 get3DZRotation(float alpha);
    QMatrix4x4 get3DScaling(float scx, float scy, float scz);
    QMatrix4x4 get3DXShearing(float shy, float shz);
    QMatrix4x4 get3DYShearing(float shx, float shz);
    QMatrix4x4 get3DZShearing(float shx, float shy);
    QMatrix4x4 get3DPerspective(float fov, float aspectRatio, float near, float far);
    QMatrix4x4 get3DLookAt(QVector3D eye, QVector3D target, QVector3D up);
    //QMatrix4x4 get3DLookAt(QVector3D eye, float pitch, float yaw);

    QMatrix4x4 get3DRotationYawPitchRoll(float yaw, float pitch, float roll);
    QMatrix4x4 get3DShearing(float shx, float shy, float shz);

    QVector3D transformCoordinate(QVector3D coordinate, QMatrix4x4 transform);
    Vertex project(Vertex coord, QMatrix4x4 transMatrix, QMatrix4x4 world, float imgWidth, float imgHeight);
};

#endif // PROJECTION_H
