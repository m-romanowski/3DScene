#ifndef SCENE_H
#define SCENE_H

#include <QVector3D>
#include <QString>

#include <texture.h>
#include <camera.h>
#include <light.h>
#include <color.h>
#include <mesh.h>

class Scene
{
public:
    Scene();
    Scene(Scene& other);
    Scene(Color color, Light light, Camera camera, std::vector<Mesh> meshes);
    Scene(Texture texture, Light light, Camera camera, std::vector<Mesh> meshes);

    void clearMeshes();

    Light light;
    Camera camera;
    Texture texture;
    Color color;
    std::vector<Mesh> meshes;
};

#endif // SCENE_H
