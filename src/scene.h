#ifndef SCENE_H
#define SCENE_H

#include <QVector3D>
#include <QString>

#include <texture.h>
#include <camera.h>
#include <light.h>
#include <color.h>

class Scene
{
public:
    Scene();
    Scene(Color color, Light light, Camera camera);
    Scene(Texture texture, Light light, Camera camera);

    Light light;
    Camera camera;
    Texture texture;
    Color color;
};

#endif // SCENE_H
