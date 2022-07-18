#ifndef COLOR_H
#define COLOR_H

#include <QImage>
#include <QString>

class Color
{
private:
    float r, g, b, a;

public:
    Color();
    Color(float r, float g, float b, float a);

    Color getColor(QImage *img, int x, int y);

    float getRed();
    float getGreen();
    float getBlue();
    float getAlpha();

    void setRed(float rValue);
    void setGreen(float gValue);
    void setBlue(float bValue);
    void setAlpha(float alphaValue);

    QString toString();
};

#endif // COLOR_H
