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

#include <clickablelabel.h>
#include <texturedialog.h>
#include <animationdialog.h>
#include <animation.h>
#include <camera.h>
#include <light.h>
#include <xmlparser.h>
#include <renderer.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    static const QString LIGHT_OBJECT_NAME;
    static const QString CAMERA_OBJECT_NAME;
    static const QString OBJECT_TEXTURE_NAME;
    static const QString SCENE_TEXTURE_NAME;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void clearTree();
    void refreshTree();
    int getSelectedItem(QString itemName);
    void animation();

private:
    Scene* getScene();
    std::vector<Mesh>& getMeshes();
    void loadModel(QString resourceName);

    Ui::MainWindow *ui;

    QPoint lastMousePosition;
    QScrollArea* scrollArea;
    bool isMoving;

    // Animations
    QTimer* timer;
    bool animationStarted;

    // Project management
    QString currentProject;
    std::unique_ptr<Renderer> renderer;

private slots:
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
