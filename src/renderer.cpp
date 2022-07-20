#include "renderer.h"

Renderer::Renderer(int x, int y, int width, int height, QWidget* parent)
    : QWidget(parent),
      drawArea(std::make_unique<QImage>(width, height, QImage::Format_RGB32)),
      scene(std::make_unique<Scene>()),
      projection(std::make_unique<Projection>()),
      frameCount(0)
{
    setGeometry(x, y, width, height);
}

int Renderer::getSceneX()
{
    return this->x();
}

int Renderer::getSceneY()
{
    return this->y();
}

int Renderer::getSceneWidth()
{
    return this->width();
}

int Renderer::getSceneHeight()
{
    return this->height();
}

Scene* Renderer::getScene()
{
    return scene.get();
}

void Renderer::setScene(Scene* newScene)
{
    scene.reset(newScene);
}

void Renderer::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    // TODO: fix fps counter
//    if(frameCount == 0)
//    {
//        frameCounterTimer.start();
//    }
//    else
//    {
//        const int currentFps = std::floor(frameCount / (float(frameCounterTimer.elapsed()) / 1000.0f));
//        painter.setPen(QPen(Qt::red));
//        painter.setFont(QFont("Times", 12, QFont::Bold));
//        painter.drawText(getSceneX(), getSceneY(), QString::number(currentFps) + " fps");
//    }

//    frameCount++;
    painter.drawImage(getSceneX(), getSceneY(), *drawArea);
}

void Renderer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    const int newSceneWidth = event->size().width();
    const int newSceneHeight = event->size().height();
    drawArea.reset(new QImage(newSceneWidth, newSceneHeight, QImage::Format_RGB32));

    setGeometry(getSceneX(), getSceneY(), newSceneWidth, newSceneHeight);
    depthBuffer.resize((newSceneWidth * newSceneHeight), std::numeric_limits<float>::infinity());
    refreshScene();
}

void Renderer::putPixel(int x, int y, float z, Color color)
{
    int sceneWidth = getSceneWidth();
    unsigned char *ptr = drawArea->bits();
    int index = (x + y * sceneWidth); // Draw only visible faces

    if (depthBuffer[index] < z)
    {
        return;
    }

    depthBuffer[index] = z;

    ptr[sceneWidth * 4 * y + 4 * x] = color.getBlue() * 255;
    ptr[sceneWidth * 4 * y + 4 * x + 1] = color.getGreen() * 255;
    ptr[sceneWidth * 4 * y + 4 * x + 2] = color.getRed() * 255;
    ptr[sceneWidth * 4 * y + 4 * x + 3] = color.getAlpha() * 255;
}

void Renderer::drawPoint(QVector3D point, Color color)
{
    const int sceneWidth = getSceneWidth();
    const int sceneHeight = getSceneHeight();
    if(point.x() >= 0 && point.y() >= 0 && point.x() < sceneWidth && point.y() < sceneHeight)
    {
        putPixel((int)point.x(), (int)point.y(), point.z(), color);
    }
}

float Renderer::clamp(float value, float min = 0, float max = 1)
{
    return std::max(min, std::min(value, max));
}

float Renderer::interpolate(float min, float max, float gradient)
{
    return min + (max - min) * clamp(gradient);
}

void Renderer::drawLine(Mesh::Edge edge, Vertex va, Vertex vb, Vertex vc, Vertex vd, Color color, Texture texture)
{
    QVector3D pa = va.coords;
    QVector3D pb = vb.coords;
    QVector3D pc = vc.coords;
    QVector3D pd = vd.coords;

    // Needed to calculate the ends of the lines, sx ---- ex
    float  gradient1 = pa.y() != pb.y() ? (edge.currentY - pa.y()) / (pb.y() - pa.y()) : 1;
    float gradient2 = pc.y() != pd.y() ? (edge.currentY - pc.y()) / (pd.y() - pc.y()) : 1;

    int sx = (int)interpolate(pa.x(), pb.x(), gradient1);
    int ex = (int)interpolate(pc.x(), pd.x(), gradient2);

    // Start and end Z coordinate
    float z1 = interpolate(pa.z(), pb.z(), gradient1);
    float z2 = interpolate(pc.z(), pd.z(), gradient2);

    float snl = interpolate(edge.ndotla, edge.ndotlb, gradient1);
    float enl = interpolate(edge.ndotlc, edge.ndotld, gradient2);

    // Interpolating texture coordinates on Y coordinate
    float su = interpolate(edge.ua, edge.ub, gradient1);
    float eu = interpolate(edge.uc, edge.ud, gradient2);
    float sv = interpolate(edge.va, edge.vb, gradient1);
    float ev = interpolate(edge.vc, edge.vd, gradient2);

    float gradient, z, ndotl;
    float u, v;
    Color textureColor;

    // Draw a line from sx to ex
    for (int x = sx; x < ex; x++)
    {
        gradient = (x - sx) / (float)(ex - sx);

        // Z coordinate, texture coordinate interpolation with respect to X coordinate
        z = interpolate(z1, z2, gradient);
        ndotl = interpolate(snl, enl, gradient);
        u = interpolate(su, eu, gradient);
        v = interpolate(sv, ev, gradient);

        // Current texture color
        textureColor = texture.mapTexture(u, v);

        // New color for the point, calculated from the cosinus of the slope:
        drawPoint(QVector3D(x, edge.currentY, z), Color(color.getRed() * ndotl * textureColor.getRed(),
                                                        color.getGreen() * ndotl * textureColor.getGreen(),
                                                        color.getBlue() * ndotl * textureColor.getBlue(),
                                                        1));
    }
}

float Renderer::computeAngle(QVector3D vertex, QVector3D normal, QVector3D lightPosition)
{
    // Light direction vector
    QVector3D lightDirection = lightPosition - vertex;

    // Normalize the normal vector and the lights
    projection->normalize(normal);
    projection->normalize(lightDirection);

    // a number in the range (0, 1), calculate the dot product
    return std::max(0.0f, projection->dot(normal, lightDirection));
}

void Renderer::drawTriangle(Vertex v1, Vertex v2, Vertex v3, Color color, Texture texture)
{
    Vertex temp;
    // Here the points should be sorted in the order p1, p2, p3
    if (v1.coords.y() > v2.coords.y())
    {
        temp = v2;
        v2 = v1;
        v1 = temp;
    }

    if (v2.coords.y() > v3.coords.y())
    {
        temp = v2;
        v2 = v3;
        v3 = temp;
    }

    if (v1.coords.y() > v2.coords.y())
    {
        temp = v2;
        v2 = v1;
        v1 = temp;
    }

    // Vertex coordinates
    QVector3D p1 = v1.coords;
    QVector3D p2 = v2.coords;
    QVector3D p3 = v3.coords;

    // I calculate the cosinus of the angle between the light vector and the normal vector, so we
    // can calculate the color intensity on a given pixel
    float nl1 = computeAngle(v1.worldCoords, v1.normal, scene->light.light);
    float nl2 = computeAngle(v2.worldCoords, v2.normal, scene->light.light);
    float nl3 = computeAngle(v3.worldCoords, v3.normal, scene->light.light);

    Mesh::Edge edge;

    // Line directions
    float dP1P2, dP1P3;

    // http://en.wikipedia.org/wiki/Slope
    // Calculate the slope of the line
    if (p2.y() - p1.y() > 0) dP1P2 = (p2.x() - p1.x()) / (p2.y() - p1.y());
    else dP1P2 = 0;

    if (p3.y() - p1.y() > 0) dP1P3 = (p3.x() - p1.x()) / (p3.y() - p1.y());
    else dP1P3 = 0;

    // Case where P2 is on the right
    if (dP1P2 > dP1P3)
    {
        for (int y = (int)p1.y(); y <= (int)p3.y(); y++)
        {
            edge.currentY = y;

            if (y < p2.y())
            {
                edge.ndotla = nl1;
                edge.ndotlb = nl3;
                edge.ndotlc = nl1;
                edge.ndotld = nl2;

                edge.ua = v1.textureCoords.x();
                edge.ub = v3.textureCoords.x();
                edge.uc = v1.textureCoords.x();
                edge.ud = v2.textureCoords.x();

                edge.va = v1.textureCoords.y();
                edge.vb = v3.textureCoords.y();
                edge.vc = v1.textureCoords.y();
                edge.vd = v2.textureCoords.y();

                drawLine(edge, v1, v3, v1, v2, color, texture);
            }
            else
            {
                edge.ndotla = nl1;
                edge.ndotlb = nl3;
                edge.ndotlc = nl2;
                edge.ndotld = nl3;

                edge.ua = v1.textureCoords.x();
                edge.ub = v3.textureCoords.x();
                edge.uc = v2.textureCoords.x();
                edge.ud = v3.textureCoords.x();

                edge.va = v1.textureCoords.y();
                edge.vb = v3.textureCoords.y();
                edge.vc = v2.textureCoords.y();
                edge.vd = v3.textureCoords.y();

                drawLine(edge, v1, v3, v2, v3, color, texture);
            }
        }
    }
    // Case where P2 is on the left
    else
    {
        for (int y = (int)p1.y(); y <= (int)p3.y(); y++)
        {
            edge.currentY = y;

            if (y < p2.y())
            {
                edge.ndotla = nl1;
                edge.ndotlb = nl2;
                edge.ndotlc = nl1;
                edge.ndotld = nl3;

                edge.ua = v1.textureCoords.x();
                edge.ub = v2.textureCoords.x();
                edge.uc = v1.textureCoords.x();
                edge.ud = v3.textureCoords.x();

                edge.va = v1.textureCoords.y();
                edge.vb = v2.textureCoords.y();
                edge.vc = v1.textureCoords.y();
                edge.vd = v3.textureCoords.y();

                drawLine(edge, v1, v2, v1, v3, color, texture);
            }
            else
            {
                edge.ndotla = nl2;
                edge.ndotlb = nl3;
                edge.ndotlc = nl1;
                edge.ndotld = nl3;

                edge.ua = v2.textureCoords.x();
                edge.ub = v3.textureCoords.x();
                edge.uc = v1.textureCoords.x();
                edge.ud = v3.textureCoords.x();

                edge.va = v2.textureCoords.y();
                edge.vb = v3.textureCoords.y();
                edge.vc = v1.textureCoords.y();
                edge.vd = v3.textureCoords.y();

                drawLine(edge, v2, v3, v1, v3, color, texture);
            }
        }
    }
}

void Renderer::clearScene()
{
    clearScene(scene->color);
}

void Renderer::clearScene(Color color)
{
    const int sceneWidth = getSceneWidth();
    const int sceneHeight = getSceneHeight();

    if(!scene->texture.filename.isEmpty())
    {
        drawArea->load(scene->texture.filename);
        const QImage scaledDrawArea = drawArea->scaled(sceneWidth, sceneHeight);
        drawArea.reset(new QImage(scaledDrawArea));
    }
    else
    {
        unsigned char *ptr = drawArea->bits();
        // Set default color for the all pixels
        for(int y = 0; y < sceneHeight; y++)
        {
            for(int x = 0; x < sceneWidth; x++)
            {
                ptr[sceneWidth * 4 * y + 4 * x] = color.getBlue();
                ptr[sceneWidth * 4 * y + 4 * x + 1] = color.getGreen();
                ptr[sceneWidth * 4 * y + 4 * x + 2] = color.getRed();
                ptr[sceneWidth * 4 * y + 4 * x + 3] = color.getAlpha();
            }
        }
    }

    // Clear the Z-buffer (depth)
    for (uint i = 0; i < depthBuffer.size(); i++)
    {
        depthBuffer[i] = std::numeric_limits<float>::infinity();
    }
}

void Renderer::render()
{
    const std::vector<Mesh> meshes = scene->meshes;
    const Camera camera = scene->camera;

    Vertex pixelA, pixelB, pixelC;
    Vertex vertexA, vertexB, vertexC;
    QMatrix4x4 result, view, model, proj;
    Color color(1, 1, 1, 1);
    int sceneWidth = getSceneWidth();
    int sceneHeight = getSceneHeight();

    // View matrix
    view = projection->get3DLookAt(camera.position, camera.target, camera.up);

    // Perspective projection matrix
    proj = projection->get3DPerspective(0.78f, (float)(sceneWidth / sceneHeight), 0.01f, 1.0f);

    // Specify points for all solids
    for(uint i = 0; i < meshes.size(); i++)
    {
        // Object transformation matrix
        model = projection->get3DTranslation(meshes[i].position.x(), meshes[i].position.y(), meshes[i].position.z()) *
                projection->get3DRotationYawPitchRoll(meshes[i].rotation.y(), meshes[i].rotation.x(), meshes[i].rotation.z()) *
                projection->get3DScaling(meshes[i].scale.x(), meshes[i].scale.y(), meshes[i].scale.z()) *
                projection->get3DShearing(meshes[i].skew.x(), meshes[i].skew.y(), meshes[i].skew.z());

        result = proj * view * model;

        // Go along all the edges of the solid
        for(uint j = 0; j < meshes[i].faces.size(); j++)
        {
            vertexA = meshes[i].vertices[meshes[i].faces[j].A];
            vertexB = meshes[i].vertices[meshes[i].faces[j].B];
            vertexC = meshes[i].vertices[meshes[i].faces[j].C];

            pixelA = projection->project(vertexA, result, model, (float)sceneWidth, (float)sceneHeight);
            pixelB = projection->project(vertexB, result, model, (float)sceneWidth, (float)sceneHeight);
            pixelC = projection->project(vertexC, result, model, (float)sceneWidth, (float)sceneHeight);

            drawTriangle(pixelA, pixelB, pixelC, color, meshes[i].texture);
        }
    }
}

void Renderer::refreshScene()
{
    clearScene();
    render();
    update();
}
