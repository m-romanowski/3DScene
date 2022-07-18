#include "color.h"

Color::Color()
{
    r = g = b = a = 0;
}

Color::Color(float r, float g, float b, float a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

float Color::getRed()
{
    return r;
}

float Color::getGreen()
{
    return g;
}

float Color::getBlue()
{
    return b;
}

float Color::getAlpha()
{
    return a;
}

void Color::setRed(float r)
{
    this->r = r;
}

void Color::setGreen(float g)
{
    this->g = g;
}

void Color::setBlue(float b)
{
    this->b = b;
}

void Color::setAlpha(float a)
{
    this->a = a;
}

Color Color::getColor(QImage *img, int x, int y)
{
    unsigned char *ptr = img->bits();
    Color res;

    if(x > 0 && x < img->width() && y > 0 && y < img->height())
    {
        res.setBlue(ptr[img->width() * 4 * y + 4 * x]);
        res.setGreen(ptr[img->width() * 4 * y + 4 * x + 1]);
        res.setRed(ptr[img->width() * 4 * y + 4 * x + 2]);
        res.setAlpha(ptr[img->width() * 4 * y + 4 * x + 3]);
    }

    return res;
}

QString Color::toString()
{
    return QString::number(this->r) + " " + QString::number(this->g) + " " + QString::number(this->b) + " " + QString::number(this->a);
}
