#ifndef RENDERER_H
#define RENDERER_H

#include <QWidget>
#include <QElapsedTimer>
#include <QPainter>
#include <QResizeEvent>

#include <scene.h>
#include <projection.h>

class Renderer : public QWidget
{
    Q_OBJECT

public:
    explicit Renderer(int x, int y, int width, int height, QWidget* parent = nullptr);

    int getSceneX();
    int getSceneY();
    int getSceneWidth();
    int getSceneHeight();
    Scene* getScene();
    void setScene(Scene* newScene);
    void clearScene();
    void clearScene(Color color);
    void refreshScene();

protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);

private:
    void putPixel(int x, int y, float z, Color color);
    void drawPoint(QVector3D point, Color color);
    float clamp(float value, float min, float max);
    float interpolate(float min, float max, float gradient);
    float computeAngle(QVector3D vertex, QVector3D normal, QVector3D lightPosition);
    void drawLine(Mesh::Edge edge, Vertex va, Vertex vb, Vertex vc, Vertex vd, Color color, Texture texture);
    void drawTriangle(Vertex v1, Vertex v2, Vertex v3, Color color, Texture texture);
    void render();

    std::unique_ptr<QImage> drawArea;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Projection> projection;
    std::vector<float> depthBuffer;
    QElapsedTimer frameCounterTimer;
    int frameCount;
};

#endif // RENDERER_H
