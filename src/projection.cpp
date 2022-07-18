#include "projection.h"

Projection::Projection()
{
}

void Projection::normalize(QVector3D &point)
{
    float length = sqrt(point.x() * point.x() + point.y() * point.y() + point.z() * point.z());

    if(length == 0)
        return;

    float num = 1.0f / length;
    point.setX(point.x() * num);
    point.setY(point.y() * num);
    point.setZ(point.z() * num);
}

QVector3D Projection::crossProduct(QVector3D point1, QVector3D point2)
{
    float x = point1.y() * point2.z() - point1.z() * point2.y();
    float y = point1.z() * point2.x() - point1.x() * point2.z();
    float z = point1.x() * point2.y() - point1.y() * point2.x();

    return QVector3D(x, y, z);
}

float Projection::dot(QVector3D point1, QVector3D point2)
{
    return (point1.x() * point2.x() + point1.y() * point2.y() + point1.z() * point2.z());
}

QMatrix4x4 Projection::get3DTranslation(float tx, float ty, float tz)
{
    QMatrix4x4 result;
    result(0, 0) = 1;   result(0, 1) = 0;   result(0, 2) = 0;   result(0, 3) = tx;
    result(1, 0) = 0;   result(1, 1) = 1;   result(1, 2) = 0;   result(1, 3) = ty;
    result(2, 0) = 0;   result(2, 1) = 0;   result(2, 2) = 1;   result(2, 3) = tz;
    result(3, 0) = 0;   result(3, 1) = 0;   result(3, 2) = 0;   result(3, 3) = 1;

    return result;
}

QMatrix4x4 Projection::get3DXRotation(float alpha)
{
    float cs = cos(alpha);
    float sn = sin(alpha);

    QMatrix4x4 result;
    result(0, 0) = 1;   result(1, 0) = 0;   result(2, 0) = 0;   result(3, 0) = 0;
    result(0, 1) = 0;   result(1, 1) = cs;  result(2, 1) = -sn; result(3, 1) = 0;
    result(0, 2) = 0;   result(1, 2) = sn;  result(2, 2) = cs;  result(3, 2) = 0;
    result(0, 3) = 0;   result(1, 3) = 0;   result(2, 3) = 0;   result(3, 3) = 1;

    return result;
}

QMatrix4x4 Projection::get3DYRotation(float alpha)
{
    float cs = cos(alpha);
    float sn = sin(alpha);

    QMatrix4x4 result;
    result(0, 0) = cs;  result(1, 0) = 0;   result(2, 0) = sn;  result(3, 0) = 0;
    result(0, 1) = 0;   result(1, 1) = 1;   result(2, 1) = 0;   result(3, 1) = 0;
    result(0, 2) = -sn; result(1, 2) = 0;   result(2, 2) = cs;  result(3, 2) = 0;
    result(0, 3) = 0;   result(1, 3) = 0;   result(2, 3) = 0;   result(3, 3) = 1;

    return result;
}

QMatrix4x4 Projection::get3DZRotation(float alpha)
{
    float cs = cos(alpha);
    float sn = sin(alpha);

    QMatrix4x4 result;
    result(0, 0) = cs;  result(1, 0) = -sn; result(2, 0) = 0;   result(3, 0) = 0;
    result(0, 1) = sn;  result(1, 1) = cs;  result(2, 1) = 0;   result(3, 1) = 0;
    result(0, 2) = 0;   result(1, 2) = 0;   result(2, 2) = 1;   result(3, 2) = 0;
    result(0, 3) = 0;   result(1, 3) = 0;   result(2, 3) = 0;   result(3, 3) = 1;

    return result;
}

QMatrix4x4 Projection::get3DRotationYawPitchRoll(float yaw, float pitch, float roll)
{
    return get3DZRotation(roll) * get3DYRotation(yaw) * get3DXRotation(pitch);
}

QMatrix4x4 Projection::get3DScaling(float scx, float scy, float scz)
{
    QMatrix4x4 result;
    result(0, 0) = scx; result(1, 0) = 0;   result(2, 0) = 0;   result(3, 0) = 0;
    result(0, 1) = 0;   result(1, 1) = scy; result(2, 1) = 0;   result(3, 1) = 0;
    result(0, 2) = 0;   result(1, 2) = 0;   result(2, 2) = scz; result(3, 2) = 0;
    result(0, 3) = 0;   result(1, 3) = 0;   result(2, 3) = 0;   result(3, 3) = 1;

    return result;
}

QMatrix4x4 Projection::get3DXShearing(float shy, float shz)
{
    QMatrix4x4 result;
    result(0, 0) = 1;   result(1, 0) = 0;   result(2, 0) = 0;   result(3, 0) = 0;
    result(0, 1) = shy; result(1, 1) = 1;   result(2, 1) = 0;   result(3, 1) = 0;
    result(0, 2) = shz; result(1, 2) = 0;   result(2, 2) = 1;   result(3, 2) = 0;
    result(0, 3) = 0;   result(1, 3) = 0;   result(2, 3) = 0;   result(3, 3) = 1;

    return result;
}

QMatrix4x4 Projection::get3DYShearing(float shx, float shz)
{
    QMatrix4x4 result;
    result(0, 0) = 1;   result(1, 0) = shx; result(2, 0) = 0;   result(3, 0) = 0;
    result(0, 1) = 0;   result(1, 1) = 1;   result(2, 1) = 0;   result(3, 1) = 0;
    result(0, 2) = 0;   result(1, 2) = shz; result(2, 2) = 1;   result(3, 2) = 0;
    result(0, 3) = 0;   result(1, 3) = 0;   result(2, 3) = 0;   result(3, 3) = 1;

    return result;
}

QMatrix4x4 Projection::get3DZShearing(float shx, float shy)
{
    QMatrix4x4 result;
    result(0, 0) = 1;   result(1, 0) = 0;   result(2, 0) = shx; result(3, 0) = 0;
    result(0, 1) = 0;   result(1, 1) = 1;   result(2, 1) = shy; result(3, 1) = 0;
    result(0, 2) = 0;   result(1, 2) = 0;   result(2, 2) = 1;   result(3, 2) = 0;
    result(0, 3) = 0;   result(1, 3) = 0;   result(2, 3) = 0;   result(3, 3) = 1;

    return result;
}

QMatrix4x4 Projection::get3DShearing(float shx, float shy, float shz)
{
    return get3DZShearing(shx, shy) * get3DYShearing(shx, shz) * get3DXShearing(shy, shz);
}

QMatrix4x4 Projection::get3DPerspective(float fov, float aspectRatio, float near, float far)
{
    float yScale = 1.0f / tan(fov * 0.5f);
    float q = -far / (near - far);

    QMatrix4x4 result;
    result(0, 0) = yScale / aspectRatio; result(1, 0) = 0.0f;   result(2, 0) = 0.0f;      result(3, 0) = 0.0f;
    result(0, 1) = 0.0f;                 result(1, 1) = yScale; result(2, 1) = 0.0f;      result(3, 1) = 0.0f;
    result(0, 2) = 0.0f;                 result(1, 2) = 0.0f;   result(2, 2) = q;         result(3, 2) = 1.0f;
    result(0, 3) = 0.0f;                 result(1, 3) = 0.0f;   result(2, 3) = -q * near; result(3, 3) = 0.0f;

    return result;
}

QMatrix4x4 Projection::get3DLookAt(QVector3D eye, QVector3D target, QVector3D up)
{
    QVector3D zaxis = target - eye;
    normalize(zaxis);

    QVector3D xaxis = crossProduct(up, zaxis);
    normalize(xaxis);

    QVector3D yaxis = crossProduct(zaxis, xaxis);

    QMatrix4x4 result;
    result(0, 0) = xaxis.x(); result(1, 0) = yaxis.x(); result(2, 0) = zaxis.x(); result(3, 0) = 0;
    result(0, 1) = xaxis.y(); result(1, 1) = yaxis.y(); result(2, 1) = zaxis.y(); result(3, 1) = 0;
    result(0, 2) = xaxis.z(); result(1, 2) = yaxis.z(); result(2, 2) = zaxis.z(); result(3, 2) = 0;
    result(0, 3) = -dot(xaxis, eye); result(1, 3) = -dot(yaxis, eye); result(2, 3) = -dot(zaxis, eye); result(3, 3) = 1.0f;

    return result;
}

/*QMatrix4x4 Projection::get3DLookAt(QVector3D eye, float pitch, float yaw)
{
    float cosPitch = cos(pitch);
    float sinPitch = sin(pitch);
    float cosYaw = cos(yaw);
    float sinYaw = sin(yaw);

    QVector3D zaxis(cosYaw, 0, -sinYaw);
    QVector3D xaxis(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
    QVector3D yaxis(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

    QMatrix4x4 result;
    result(0, 0) = xaxis.x();   result(0, 1) = yaxis.x();   result(0, 2) = zaxis.x();   result(0, 3) = 0;
    result(1, 0) = xaxis.y();   result(1, 1) = yaxis.y();   result(1, 2) = zaxis.y();   result(1, 3) = 0;
    result(2, 0) = xaxis.z();   result(2, 1) = yaxis.z();   result(2, 2) = zaxis.z();   result(2, 3) = 0;
    result(3, 0) = -QVector3D::dotProduct(xaxis, eye);   result(3, 1) = -QVector3D::dotProduct(yaxis, eye);   result(3, 2) = -QVector3D::dotProduct(zaxis, eye);   result(3, 3) = 1;

    return result;
}*/

QVector3D Projection::transformCoordinate(QVector3D coordinate, QMatrix4x4 transform)
{
    // Multiply the resulting matrix (after multiplying all matricec: model * view * project) by the coordinates of the 3D vector
    float x = (coordinate.x() * transform(0, 0)) + (coordinate.y() * transform(0, 1)) + (coordinate.z() * transform(0, 2)) + transform(0, 3);
    float y = (coordinate.x() * transform(1, 0)) + (coordinate.y() * transform(1, 1)) + (coordinate.z() * transform(1, 2)) + transform(1, 3);
    float z = (coordinate.x() * transform(2, 0)) + (coordinate.y() * transform(2, 1)) + (coordinate.z() * transform(2, 2)) + transform(2, 3);
    float w = (coordinate.x() * transform(3, 0)) + (coordinate.y() * transform(3, 1)) + (coordinate.z() * transform(3, 2)) + transform(3, 3);

    // A new 3D vector after dividing by 'w' [x, y, z, w]
    return QVector3D(x / w, y / w, z / w);
}

Vertex Projection::project(Vertex vertex, QMatrix4x4 transMatrix, QMatrix4x4 worldMatrix, float imgWidth, float imgHeight)
{
    // Transform the vertices into a 2D plan
    QVector3D point = transformCoordinate(vertex.coords, transMatrix);

    // Convert coordinates (world and normals) to vertices in a 3D world
    QVector3D worldPoint = transformCoordinate(vertex.coords, worldMatrix);
    QVector3D normalPoint = transformCoordinate(vertex.normal, worldMatrix);

    // We start drawing from the top left corner. Below, I transform the coordinates to make a solid
    // was in the middle of the scene
    float x = point.x() * imgWidth + imgWidth / 2.0f;
    float y = -point.y() * imgHeight + imgHeight / 2.0f;

    // New vertex coordinates
    Vertex newVertex;
    newVertex.coords = QVector3D(x, y, point.z());
    newVertex.normal = normalPoint;
    newVertex.worldCoords = worldPoint;
    newVertex.textureCoords = vertex.textureCoords;

    return newVertex;
}
