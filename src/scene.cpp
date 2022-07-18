#include "scene.h"

Scene::Scene()
{
    light.light.setX(0);
    light.light.setY(0);
    light.light.setZ(0);

    camera.position.setX(0);
    camera.position.setY(0);
    camera.position.setZ(0);

    camera.target.setX(0);
    camera.target.setY(0);
    camera.target.setZ(0);

    camera.up.setX(0);
    camera.up.setY(0);
    camera.up.setZ(0);

    color = Color(45, 46, 48, 255);
}

Scene::Scene(Color color, Light light, Camera camera)
{
    this->light = light;
    this->camera = camera;
    this->color = color;
}

Scene::Scene(Texture texture, Light light, Camera camera)
{
    this->texture = texture;
    this->light = light;
    this->camera = camera;
}
