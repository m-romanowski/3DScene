#ifndef TEXTURE_H
#define TEXTURE_H

#include <color.h>

#include <QDebug>

class Texture
{
public:
    Texture();
    Texture(QString filename, int width, int height);
    Texture(Color color, int width, int height);

    QString filename;
    int width, height;
    Color color;
    QImage *img;

    void loadTexture();
    Color mapTexture(float tu, float tv);
};

#endif // TEXTURE_H
