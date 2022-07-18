#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QMouseEvent>
#include <QScrollArea>
#include <QFileDialog>
#include <QPixmap>
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolTip>
#include <QAction>
#include <QMenu>
#include <QDebug>

#include <clickablelabel.h>
#include <texturedialog.h>
#include <animationdialog.h>
#include <animation.h>
#include <color.h>
#include <mesh.h>
#include <projection.h>
#include <face.h>
#include <camera.h>
#include <light.h>
#include <scene.h>
#include <xmlparser.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void clearTree();
    void refreshTree();
    void clearMeshes();
    int getSelectedItem(QString itemName);
    void refreshScene();
    void clearScene(Color color);
    void animation();
    void render(std::vector<Mesh> &meshes, Camera cam);

private:
    Ui::MainWindow *ui;

    QPoint lastMousePosition;
    QScrollArea *scrollArea;
    bool isMoving;

    // Scene
    QImage *img;
    int sceneX, sceneY, sceneWidth, sceneHeight;

    // Scene object
    Scene scene;
    Projection *proc;
    std::vector<Mesh> meshes;
    std::vector<float> depthBuffer;

    // Animations
    QTimer *timer;
    bool animationStarted;

    // Fps counter
    QTime m_timer;
    float target_time;
    int m_frameCount;

    // Project management
    QString currentProject;

    void putPixel(int x, int y, float z, Color color);
    void drawPoint(QVector3D point, Color color);
    float clamp(float value, float min, float max);
    float interpolate(float min, float max, float gradient);
    float computeAngle(QVector3D vertex, QVector3D normal, QVector3D lightPosition);
    void drawLine(lineData data, Vertex va, Vertex vb, Vertex vc, Vertex vd, Color color, Texture texture);
    void drawTriangle(Vertex v1, Vertex v2, Vertex v3, Color color, Texture texture);

private slots:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void on_exitButton_clicked();
    void on_maximizeButton_clicked();
    void on_minimizeButton_clicked();
    void loadTexture(ClickableLabel *label);
    void on_openProjectButton_clicked();
    void on_saveProjectButton_clicked();
    void on_subTransXButton_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_addTransXButton_clicked();
    void on_addTransYButton_clicked();
    void on_subTransYButton_clicked();
    void on_addTransZButton_clicked();
    void on_subTransZButton_clicked();
    void on_addRotXButton_clicked();
    void on_subRotXButton_clicked();
    void on_addRotYButton_clicked();
    void on_subRotYButton_clicked();
    void on_addRotZButton_clicked();
    void on_subRotZButton_clicked();
    void on_addScXButton_clicked();
    void on_subScXButton_clicked();
    void on_addScYButton_clicked();
    void on_subScYButton_clicked();
    void on_addScZButton_clicked();
    void on_subScZButton_clicked();
    void on_addShXButton_clicked();
    void on_subShXButton_clicked();
    void on_addShYButton_clicked();
    void on_subShYButton_clicked();
    void on_addShZButton_clicked();
    void on_subShZButton_clicked();
    void on_objNameValue_editingFinished();
    void on_cubeButton_clicked();
    void on_newProjectButton_clicked();
    void on_sphereButton_clicked();
    void on_cylinderButton_clicked();
    void on_pyramidButton_clicked();
    void deleteItem(QString name);
    void prepareMenu(const QPoint &pos);
    void on_transXValue_editingFinished();
    void on_transYValue_editingFinished();
    void on_transZValue_editingFinished();
    void on_rotXValue_editingFinished();
    void on_rotYValue_editingFinished();
    void on_rotZValue_editingFinished();
    void on_scXValue_editingFinished();
    void on_scYValue_editingFinished();
    void on_scZValue_editingFinished();
    void on_shXValue_editingFinished();
    void on_shYValue_editingFinished();
    void on_shZValue_editingFinished();
    void on_settingsAnimationButton_clicked();
    void on_startAnimationButton_clicked();
    void on_stopAnimationButton_clicked();
};

#endif // MAINWINDOW_H
