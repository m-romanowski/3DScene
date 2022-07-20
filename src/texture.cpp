#include "texture.h"

Texture::Texture()
{
    this->width = 0;
    this->height = 0;
    this->filename = "";
    this->color = Color(255, 255, 255, 255);
}

Texture::Texture(QString filename)
{
    this->filename = filename;

    loadTexture();
}

Texture::Texture(Color color, int width, int height)
{
    this->width = width;
    this->height = height;
    this->filename = "";
    this->color = color;

    loadTexture();
}

void Texture::loadTexture()
{
    if (filename.isEmpty())
    {
        img = new QImage(width, height, QImage::Format_ARGB32);
        img->fill(QColor(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()));
    }
    else
    {
        img = new QImage();
        img->load(filename);
        width = img->width();
        height = img->height();
    }
}

Color Texture::mapTexture(float tu, float tv)
{
    unsigned char *ptr = img->bits();

    // Determine the x and y coordinates from the u and v coordinates of the texture. It will
    // be used to get the color from the texture
    int u = abs((int) (tu * width) % width);
    int v = abs((int) (tv * height) % height);

    // Get the color from the texture
    float b = ptr[width * 4 * v + 4 * u];
    float g = ptr[width * 4 * v + 4 * u + 1];
    float r = ptr[width * 4 * v + 4 * u + 2];
    float a = ptr[width * 4 * v + 4 * u + 3];

    return Color(r / 255.0f , g / 255.0f, b / 255.0f, a / 255.0f);
}
