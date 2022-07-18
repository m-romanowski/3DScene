#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Main window without default Qt's buttons (close, minimize, etc)
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->statusBar->setSizeGripEnabled(true);

    connect(ui->sceneTexture, &ClickableLabel::clicked, this, [this]{ loadTexture(ui->sceneTexture); });
    connect(ui->objTexture, &ClickableLabel::clicked, this, [this]{ loadTexture(ui->objTexture); });

    // Translation
    ui->transXLabel->setVisible(false);
    ui->transXValue->setVisible(false);
    ui->subTransXButton->setVisible(false);
    ui->addTransXButton->setVisible(false);

    ui->transYLabel->setVisible(false);
    ui->transYValue->setVisible(false);
    ui->subTransYButton->setVisible(false);
    ui->addTransYButton->setVisible(false);

    ui->transZLabel->setVisible(false);
    ui->transZValue->setVisible(false);
    ui->subTransZButton->setVisible(false);
    ui->addTransZButton->setVisible(false);

    // Rotation
    ui->rotXLabel->setVisible(false);
    ui->rotXValue->setVisible(false);
    ui->subRotXButton->setVisible(false);
    ui->addRotXButton->setVisible(false);

    ui->rotYLabel->setVisible(false);
    ui->rotYValue->setVisible(false);
    ui->subRotYButton->setVisible(false);
    ui->addRotYButton->setVisible(false);

    ui->rotZLabel->setVisible(false);
    ui->rotZValue->setVisible(false);
    ui->subRotZButton->setVisible(false);
    ui->addRotZButton->setVisible(false);

    // Scale
    ui->scXLabel->setVisible(false);
    ui->scXValue->setVisible(false);
    ui->subScXButton->setVisible(false);
    ui->addScXButton->setVisible(false);

    ui->scYLabel->setVisible(false);
    ui->scYValue->setVisible(false);
    ui->subScYButton->setVisible(false);
    ui->addScYButton->setVisible(false);

    ui->scZLabel->setVisible(false);
    ui->scZValue->setVisible(false);
    ui->subScZButton->setVisible(false);
    ui->addScZButton->setVisible(false);

    // Skew
    ui->shXLabel->setVisible(false);
    ui->shXValue->setVisible(false);
    ui->subShXButton->setVisible(false);
    ui->addShXButton->setVisible(false);

    ui->shYLabel->setVisible(false);
    ui->shYValue->setVisible(false);
    ui->subShYButton->setVisible(false);
    ui->addShYButton->setVisible(false);

    ui->shZLabel->setVisible(false);
    ui->shZValue->setVisible(false);
    ui->subShZButton->setVisible(false);
    ui->addShZButton->setVisible(false);

    // Other widgets
    ui->objTransformationLabel->setVisible(false);
    ui->objNameLabel->setVisible(false);
    ui->objNameValue->setVisible(false);
    ui->objSettingsLabel->setVisible(false);
    ui->objTextureLabel->setVisible(false);
    ui->objTexture->setVisible(false);

    // Remove scene objects
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::prepareMenu);

    // Animation
    animationStarted = false;

    // Fps counter
    m_frameCount = 0;

    // Current project name reference
    currentProject = "";

    // Scene texture miniature
    QPixmap pixmap(ui->sceneTexture->width(), ui->sceneTexture->height());
    pixmap.fill(QColor(scene.color.getRed(), scene.color.getGreen(), scene.color.getBlue(), scene.color.getAlpha()));
    ui->sceneTexture->setPixmap(pixmap);

    proc = new Projection();

    // Default scene settings
    ui->scene->setFixedSize(this->width() - 340, this->height() - 120);
    ui->scene->setGeometry(15, 93, this->width() - 340, this->height() - 120);

    sceneX = ui->scene->x();
    sceneY = ui->scene->y();
    sceneWidth = ui->scene->width();
    sceneHeight = ui->scene->height();

    img = new QImage(sceneWidth, sceneHeight, QImage::Format_RGB32);
}

MainWindow::~MainWindow()
{
    delete img;
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    if(m_frameCount == 0)
        m_timer.start();
    else
        ui->currentFps->setText(QString::number(floor(m_frameCount / (float(m_timer.elapsed()) / 1000.0f))) + " fps");

    m_frameCount++;

    QPainter p(this);
    p.drawImage(sceneX, sceneY, *img);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    delete img;

    ui->scene->setFixedSize(event->size().width() - 340, event->size().height() - 120);
    ui->scene->setGeometry(ui->scene->x(), ui->scene->y(), event->size().width() - 340, event->size().height() - 120);
    sceneWidth = ui->scene->width();
    sceneHeight = ui->scene->height();

    img = new QImage(sceneWidth, sceneHeight, QImage::Format_RGB32);

    // Z-buffer (depth)
    depthBuffer.resize((sceneWidth * sceneHeight), std::numeric_limits<float>::infinity());
    refreshScene();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(!ui->titleBarWidget->underMouse())
        return;

    if(event->button() == Qt::LeftButton)
    {
        isMoving = true;
        lastMousePosition = event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(!ui->titleBarWidget->underMouse())
        return;

    if(event->buttons().testFlag(Qt::LeftButton) && isMoving)
        this->move(this->pos() + (event->pos() - lastMousePosition));
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(!ui->titleBarWidget->underMouse())
        return;

    if(event->button() == Qt::LeftButton)
        isMoving = false;
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if(!ui->titleBarWidget->underMouse())
        return;

    on_maximizeButton_clicked();
}

void MainWindow::on_exitButton_clicked()
{
    this->close();
}

void MainWindow::on_maximizeButton_clicked()
{
    if(this->isMaximized())
        this->showNormal();
    else
        this->showMaximized();
}

void MainWindow::on_minimizeButton_clicked()
{
    this->showMinimized();
}

void MainWindow::loadTexture(ClickableLabel *label)
{
    QString filename = "";
    Color color;
    int index = -1;

    if(label->objectName() == "objTexture")
    {
        QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());
        index = getSelectedItem(currentItem);
    }

    if(label->objectName() == "sceneTexture" && !scene.texture.filename.isEmpty()) filename = scene.texture.filename;
    if(label->objectName() == "sceneTexture" && scene.texture.filename.isEmpty()) color = scene.color;

    if(label->objectName() == "objTexture" && !meshes[index].texture.filename.isEmpty() && index >= 0) filename = meshes[index].texture.filename;
    if(label->objectName() == "objTexture" && meshes[index].texture.filename.isEmpty() && index >= 0) color = meshes[index].texture.color;

    TextureDialog *textureDialog = new TextureDialog(filename, color, this);
    textureDialog->setModal(true);
    textureDialog->exec();

    if(label->objectName() == "sceneTexture" && !textureDialog->getFilename().isEmpty())
    {
        scene.texture.filename = textureDialog->getFilename();

        // Change texture miniature
        ui->sceneTexture->setPixmap(textureDialog->getTexture().scaled(ui->sceneTexture->width(), ui->sceneTexture->height()));

        // Reload texture widget
        scene.texture.loadTexture();
    }
    if(label->objectName() == "sceneTexture" && textureDialog->getFilename().isEmpty())
    {
        scene.color = textureDialog->getColor();
        scene.texture.filename = textureDialog->getFilename();

        // Change texture miniature
        ui->sceneTexture->setPixmap(textureDialog->getTexture().scaled(ui->sceneTexture->width(), ui->sceneTexture->height()));

        // Reload texture widget
        scene.texture.loadTexture();
    }

    if(label->objectName() == "objTexture" && !textureDialog->getFilename().isEmpty() && index >= 0)
    {
        meshes[index].texture.filename = textureDialog->getFilename();
        meshes[index].texture.loadTexture();

        // Change texture miniature
        ui->objTexture->setPixmap(textureDialog->getTexture().scaled(ui->sceneTexture->width(), ui->sceneTexture->height()));
    }
    if(label->objectName() == "objTexture" && textureDialog->getFilename().isEmpty() && index >= 0)
    {
        meshes[index].texture.color = textureDialog->getColor();
        meshes[index].texture.filename = textureDialog->getFilename();

        // Change texture miniature
        ui->objTexture->setPixmap(textureDialog->getTexture().scaled(ui->sceneTexture->width(), ui->sceneTexture->height()));

        // Reload texture widget
        meshes[index].texture.loadTexture();
    }

    // Reload scene
    refreshScene();
    delete textureDialog;
}

void MainWindow::on_settingsAnimationButton_clicked()
{
    if(currentProject.isEmpty())
        return;

    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());
    int index = getSelectedItem(currentItem);

    if(index < 0)
        return;

    AnimationDialog *animationDialog = new AnimationDialog(meshes[index].name ,this);
    animationDialog->setModal(true);
    animationDialog->exec();

    meshes[index].animation = animationDialog->getAnimation();
    delete animationDialog;
}

void MainWindow::on_startAnimationButton_clicked()
{
    if(animationStarted)
        return;

    animationStarted = true;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::animation);
    timer->start(1000.0f / 60.0f);
}

void MainWindow::on_stopAnimationButton_clicked()
{
    if(animationStarted)
    {
        timer->stop();
        animationStarted = false;
        delete timer;
    }
}

void MainWindow::putPixel(int x, int y, float z, Color color)
{
    unsigned char *ptr = img->bits();

    // Draw only visible faces
    int index = (x + y * sceneWidth);

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

void MainWindow::drawPoint(QVector3D point, Color color)
{
    if(point.x() >= 0 && point.y() >= 0 && point.x() < sceneWidth && point.y() < sceneHeight)
    {
        putPixel((int)point.x(), (int)point.y(), point.z(), color);
    }
}

void MainWindow::clearScene(Color color)
{
    unsigned char *ptr = img->bits();

    if(!scene.texture.filename.isEmpty())
    {
        img->load(scene.texture.filename);
        *img = img->scaled(sceneWidth, sceneHeight);
    }
    else
    {
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

float MainWindow::clamp(float value, float min = 0, float max = 1)
{
    return std::max(min, std::min(value, max));
}

float MainWindow::interpolate(float min, float max, float gradient)
{
    return min + (max - min) * clamp(gradient);
}

float MainWindow::computeAngle(QVector3D vertex, QVector3D normal, QVector3D lightPosition)
{
    // Light direction vector
    QVector3D lightDirection = lightPosition - vertex;

    // Normalize the normal vector and the lights
    proc->normalize(normal);
    proc->normalize(lightDirection);

    // a number in the range (0, 1), calculate the dot product
    return std::max(0.0f, proc->dot(normal, lightDirection));
}

void MainWindow::drawLine(lineData data, Vertex va, Vertex vb, Vertex vc, Vertex vd, Color color, Texture texture)
{
    QVector3D pa = va.coords;
    QVector3D pb = vb.coords;
    QVector3D pc = vc.coords;
    QVector3D pd = vd.coords;

    // Needed to calculate the ends of the lines, sx ---- ex
    float  gradient1 = pa.y() != pb.y() ? (data.currentY - pa.y()) / (pb.y() - pa.y()) : 1;
    float gradient2 = pc.y() != pd.y() ? (data.currentY - pc.y()) / (pd.y() - pc.y()) : 1;

    int sx = (int)interpolate(pa.x(), pb.x(), gradient1);
    int ex = (int)interpolate(pc.x(), pd.x(), gradient2);

    // Start and end Z coordinate
    float z1 = interpolate(pa.z(), pb.z(), gradient1);
    float z2 = interpolate(pc.z(), pd.z(), gradient2);

    float snl = interpolate(data.ndotla, data.ndotlb, gradient1);
    float enl = interpolate(data.ndotlc, data.ndotld, gradient2);

    // Interpolating texture coordinates on Y coordinate
    float su = interpolate(data.ua, data.ub, gradient1);
    float eu = interpolate(data.uc, data.ud, gradient2);
    float sv = interpolate(data.va, data.vb, gradient1);
    float ev = interpolate(data.vc, data.vd, gradient2);

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
        drawPoint(QVector3D(x, data.currentY, z), Color(color.getRed() * ndotl * textureColor.getRed(),
                                                        color.getGreen() * ndotl * textureColor.getGreen(),
                                                        color.getBlue() * ndotl * textureColor.getBlue(),
                                                        1));
    }
}

void MainWindow::drawTriangle(Vertex v1, Vertex v2, Vertex v3, Color color, Texture texture)
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
    float nl1 = computeAngle(v1.worldCoords, v1.normal, scene.light.light);
    float nl2 = computeAngle(v2.worldCoords, v2.normal, scene.light.light);
    float nl3 = computeAngle(v3.worldCoords, v3.normal, scene.light.light);

    lineData data;

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
            data.currentY = y;

            if (y < p2.y())
            {
                data.ndotla = nl1;
                data.ndotlb = nl3;
                data.ndotlc = nl1;
                data.ndotld = nl2;

                data.ua = v1.textureCoords.x();
                data.ub = v3.textureCoords.x();
                data.uc = v1.textureCoords.x();
                data.ud = v2.textureCoords.x();

                data.va = v1.textureCoords.y();
                data.vb = v3.textureCoords.y();
                data.vc = v1.textureCoords.y();
                data.vd = v2.textureCoords.y();

                drawLine(data, v1, v3, v1, v2, color, texture);
            }
            else
            {
                data.ndotla = nl1;
                data.ndotlb = nl3;
                data.ndotlc = nl2;
                data.ndotld = nl3;

                data.ua = v1.textureCoords.x();
                data.ub = v3.textureCoords.x();
                data.uc = v2.textureCoords.x();
                data.ud = v3.textureCoords.x();

                data.va = v1.textureCoords.y();
                data.vb = v3.textureCoords.y();
                data.vc = v2.textureCoords.y();
                data.vd = v3.textureCoords.y();

                drawLine(data, v1, v3, v2, v3, color, texture);
            }
        }
    }
    // Case where P2 is on the left
    else
    {
        for (int y = (int)p1.y(); y <= (int)p3.y(); y++)
        {
            data.currentY = y;

            if (y < p2.y())
            {
                data.ndotla = nl1;
                data.ndotlb = nl2;
                data.ndotlc = nl1;
                data.ndotld = nl3;

                data.ua = v1.textureCoords.x();
                data.ub = v2.textureCoords.x();
                data.uc = v1.textureCoords.x();
                data.ud = v3.textureCoords.x();

                data.va = v1.textureCoords.y();
                data.vb = v2.textureCoords.y();
                data.vc = v1.textureCoords.y();
                data.vd = v3.textureCoords.y();

                drawLine(data, v1, v2, v1, v3, color, texture);
            }
            else
            {
                data.ndotla = nl2;
                data.ndotlb = nl3;
                data.ndotlc = nl1;
                data.ndotld = nl3;

                data.ua = v2.textureCoords.x();
                data.ub = v3.textureCoords.x();
                data.uc = v1.textureCoords.x();
                data.ud = v3.textureCoords.x();

                data.va = v2.textureCoords.y();
                data.vb = v3.textureCoords.y();
                data.vc = v1.textureCoords.y();
                data.vd = v3.textureCoords.y();

                drawLine(data, v2, v3, v1, v3, color, texture);
            }
        }
    }
}

void MainWindow::render(std::vector<Mesh> &meshes, Camera cam)
{
    Vertex pixelA, pixelB, pixelC;
    Vertex vertexA, vertexB, vertexC;
    QMatrix4x4 result, view, model, proj;
    Color color(1, 1, 1, 1);

    // View matrix
    view = proc->get3DLookAt(cam.position, cam.target, cam.up);

    // Perspective projection matrix
    proj = proc->get3DPerspective(0.78f, (float)(sceneWidth / sceneHeight), 0.01f, 1.0f);

    // Specify points for all solids
    for(uint i = 0; i < meshes.size(); i++)
    {
        // Object transformation matrix
        model = proc->get3DTranslation(meshes[i].position.x(), meshes[i].position.y(), meshes[i].position.z()) *
                proc->get3DRotationYawPitchRoll(meshes[i].rotation.y(), meshes[i].rotation.x(), meshes[i].rotation.z()) *
                proc->get3DScaling(meshes[i].scaling.x(), meshes[i].scaling.y(), meshes[i].scaling.z()) *
                proc->get3DShearing(meshes[i].shearing.x(), meshes[i].shearing.y(), meshes[i].shearing.z());

        result = proj * view * model;

        // Go along all the edges of the solid
        for(uint j = 0; j < meshes[i].faces.size(); j++)
        {
            vertexA = meshes[i].vertices[meshes[i].faces[j].A];
            vertexB = meshes[i].vertices[meshes[i].faces[j].B];
            vertexC = meshes[i].vertices[meshes[i].faces[j].C];

            pixelA = proc->project(vertexA, result, model, (float)sceneWidth, (float)sceneHeight);
            pixelB = proc->project(vertexB, result, model, (float)sceneWidth, (float)sceneHeight);
            pixelC = proc->project(vertexC, result, model, (float)sceneWidth, (float)sceneHeight);

            drawTriangle(pixelA, pixelB, pixelC, color, meshes[i].texture);
        }
    }

    update();
}

void MainWindow::animation()
{
    clearScene(scene.color);

    for(uint i = 0; i < meshes.size(); i++)
    {
        meshes[i].position = QVector3D(meshes[i].position.x() + meshes[i].animation.translation.x(),
                                       meshes[i].position.y() + meshes[i].animation.translation.y(),
                                       meshes[i].position.z() + meshes[i].animation.translation.z());

        meshes[i].rotation = QVector3D(meshes[i].rotation.x() + meshes[i].animation.rotation.x(),
                                       meshes[i].rotation.y() + meshes[i].animation.rotation.y(),
                                       meshes[i].rotation.z() + meshes[i].animation.rotation.z());

        render(meshes, scene.camera);
    }

    update();
}

void MainWindow::refreshScene()
{
    clearScene(scene.color);
    render(meshes, scene.camera);
    update();
}

void MainWindow::clearMeshes()
{
    if(meshes.size() > 0)
        meshes.clear();
}

void MainWindow::on_newProjectButton_clicked()
{
    if(!currentProject.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Information"), tr("Are you sure you want to open a new project?"),
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::No)
            return;
    }

    // Reload objects on the tree
    clearTree();
    clearMeshes();
    currentProject = "";

    Scene newScene;

    // Camera
    newScene.camera.position = QVector3D(0, 0, 10);
    newScene.camera.target = QVector3D(0, 0, 0);
    newScene.camera.up = QVector3D(0, 1, 0);

    // Scene background
    newScene.color = Color(0, 0, 0, 255);

    // Light source
    newScene.light.light = QVector3D(3, 0, -50);

    // Reload scene tree
    scene = newScene;
    refreshTree();

    currentProject = "NewProject";

    // Reload scene
    refreshScene();
}

void MainWindow::on_openProjectButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                   tr("Open a project"),
                                                   "C://",
                                                   "XML File(*.xml)");

    if(filename == currentProject)
    {
        QMessageBox::StandardButton message;
        message = QMessageBox::warning(this,
                                       "Information",
                                       "This project has already been opened.",
                                       QMessageBox::Ok);
        if(message == QMessageBox::Ok)
            return;
    }

    // Clear all objects on the scene
    clearTree();
    clearMeshes();

    XMLParser *xml = new XMLParser();
    xml->readXML(filename);

    scene = xml->getScene();
    meshes = xml->getMeshes();

    for(uint i = 0; i < meshes.size(); i++)
        meshes[i].id = i + 1;

    refreshTree();

    if(!scene.texture.filename.isEmpty())
    {
        QPixmap pixmap(scene.texture.filename);
        ui->sceneTexture->setPixmap(pixmap.scaled(ui->sceneTexture->width(), ui->sceneTexture->height()));
    }
    else
    {
        QPixmap pixmap(ui->sceneTexture->width(), ui->sceneTexture->height());
        pixmap.fill(QColor(scene.color.getRed(), scene.color.getGreen(), scene.color.getBlue(), scene.color.getAlpha()));
        ui->sceneTexture->setPixmap(pixmap);
    }

    refreshScene();
    currentProject = filename;
    delete xml;
}

void MainWindow::on_saveProjectButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save a project file"),
                                                    "C://",
                                                    "XML File(*.xml)");
    XMLParser *xml = new XMLParser();
    xml->writeXML(meshes, scene, filename);
    delete xml;
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    int index = -1;

    for(uint i = 0; i < meshes.size(); i++)
    {
        if(item->text(column) == meshes[i].name)
        {
            index = i;
            break;
        }
    }

    if((index == -1 && item->text(column) == "Light") || (index == -1 && item->text(column) == "Camera"))
    {
        // Translation
        ui->transXLabel->setVisible(true);
        ui->transXValue->setVisible(true);
        ui->subTransXButton->setVisible(true);
        ui->addTransXButton->setVisible(true);

        // Rotation
        ui->rotXLabel->setVisible(false);
        ui->rotXValue->setVisible(false);
        ui->subRotXButton->setVisible(false);
        ui->addRotXButton->setVisible(false);

        ui->rotYLabel->setVisible(false);
        ui->rotYValue->setVisible(false);
        ui->subRotYButton->setVisible(false);
        ui->addRotYButton->setVisible(false);

        ui->rotZLabel->setVisible(false);
        ui->rotZValue->setVisible(false);
        ui->subRotZButton->setVisible(false);
        ui->addRotZButton->setVisible(false);

        // Scale
        ui->scXLabel->setVisible(false);
        ui->scXValue->setVisible(false);
        ui->subScXButton->setVisible(false);
        ui->addScXButton->setVisible(false);

        ui->scYLabel->setVisible(false);
        ui->scYValue->setVisible(false);
        ui->subScYButton->setVisible(false);
        ui->addScYButton->setVisible(false);

        ui->scZLabel->setVisible(false);
        ui->scZValue->setVisible(false);
        ui->subScZButton->setVisible(false);
        ui->addScZButton->setVisible(false);

        // Skew
        ui->shXLabel->setVisible(false);
        ui->shXValue->setVisible(false);
        ui->subShXButton->setVisible(false);
        ui->addShXButton->setVisible(false);

        ui->shYLabel->setVisible(false);
        ui->shYValue->setVisible(false);
        ui->subShYButton->setVisible(false);
        ui->addShYButton->setVisible(false);

        ui->shZLabel->setVisible(false);
        ui->shZValue->setVisible(false);
        ui->subShZButton->setVisible(false);
        ui->addShZButton->setVisible(false);

        // Object texture
        ui->objTextureLabel->setVisible(false);
        ui->objTexture->setVisible(false);

        // Object name
        ui->objNameValue->setText(item->text(column));
        ui->objNameValue->setEnabled(false);

        if(item->text(column) == "Light")
        {
            // Object position
            ui->transXValue->setText(QString::number(scene.light.light.x()));
            ui->transYValue->setText(QString::number(scene.light.light.y()));
            ui->transZValue->setText(QString::number(scene.light.light.z()));
        }

        if(item->text(column) == "Camera")
        {
            // Object position
            ui->transXValue->setText(QString::number(scene.camera.position.x()));
            ui->transYValue->setText(QString::number(scene.camera.position.y()));
            ui->transZValue->setText(QString::number(scene.camera.position.z()));
        }
    }
    else
    {
        // Translation
        ui->transXLabel->setVisible(true);
        ui->transXValue->setVisible(true);
        ui->subTransXButton->setVisible(true);
        ui->addTransXButton->setVisible(true);

        ui->transYLabel->setVisible(true);
        ui->transYValue->setVisible(true);
        ui->subTransYButton->setVisible(true);
        ui->addTransYButton->setVisible(true);

        ui->transZLabel->setVisible(true);
        ui->transZValue->setVisible(true);
        ui->subTransZButton->setVisible(true);
        ui->addTransZButton->setVisible(true);

        // Rotation
        ui->rotXLabel->setVisible(true);
        ui->rotXValue->setVisible(true);
        ui->subRotXButton->setVisible(true);
        ui->addRotXButton->setVisible(true);

        ui->rotYLabel->setVisible(true);
        ui->rotYValue->setVisible(true);
        ui->subRotYButton->setVisible(true);
        ui->addRotYButton->setVisible(true);

        ui->rotZLabel->setVisible(true);
        ui->rotZValue->setVisible(true);
        ui->subRotZButton->setVisible(true);
        ui->addRotZButton->setVisible(true);

        // Scale
        ui->scXLabel->setVisible(true);
        ui->scXValue->setVisible(true);
        ui->subScXButton->setVisible(true);
        ui->addScXButton->setVisible(true);

        ui->scYLabel->setVisible(true);
        ui->scYValue->setVisible(true);
        ui->subScYButton->setVisible(true);
        ui->addScYButton->setVisible(true);

        ui->scZLabel->setVisible(true);
        ui->scZValue->setVisible(true);
        ui->subScZButton->setVisible(true);
        ui->addScZButton->setVisible(true);

        // Skew
        ui->shXLabel->setVisible(true);
        ui->shXValue->setVisible(true);
        ui->subShXButton->setVisible(true);
        ui->addShXButton->setVisible(true);

        ui->shYLabel->setVisible(true);
        ui->shYValue->setVisible(true);
        ui->subShYButton->setVisible(true);
        ui->addShYButton->setVisible(true);

        ui->shZLabel->setVisible(true);
        ui->shZValue->setVisible(true);
        ui->subShZButton->setVisible(true);
        ui->addShZButton->setVisible(true);

        // Object texture
        ui->objTextureLabel->setVisible(true);
        ui->objTexture->setVisible(true);

        // Object name
        ui->objNameValue->setEnabled(true);
        ui->objNameValue->setText(meshes[index].name);

        // Object position
        ui->transXValue->setText(QString::number(meshes[index].position.x()));
        ui->transYValue->setText(QString::number(meshes[index].position.y()));
        ui->transZValue->setText(QString::number(meshes[index].position.z()));

        // Object roation
        ui->rotXValue->setText(QString::number(meshes[index].rotation.x()));
        ui->rotYValue->setText(QString::number(meshes[index].rotation.y()));
        ui->rotZValue->setText(QString::number(meshes[index].rotation.z()));

        // Object scale
        ui->scXValue->setText(QString::number(meshes[index].scaling.x()));
        ui->scYValue->setText(QString::number(meshes[index].scaling.y()));
        ui->scZValue->setText(QString::number(meshes[index].scaling.z()));

        // Object skew
        ui->shXValue->setText(QString::number(meshes[index].shearing.x()));
        ui->shYValue->setText(QString::number(meshes[index].shearing.y()));
        ui->shZValue->setText(QString::number(meshes[index].shearing.z()));

        // Object texture
        if(!meshes[index].texture.filename.isEmpty())
        {
            QPixmap pixmap(meshes[index].texture.filename);
            ui->objTexture->setPixmap(pixmap.scaled(ui->objTexture->width(), ui->objTexture->height()));
        }
        else
        {
            QPixmap pixmap(ui->objTexture->width(), ui->objTexture->height());
            pixmap.fill(QColor(meshes[index].texture.color.getRed(),
                               meshes[index].texture.color.getGreen(),
                               meshes[index].texture.color.getBlue(),
                               meshes[index].texture.color.getAlpha()));
            ui->objTexture->setPixmap(pixmap);
        }
    }

    // Other widgets
    ui->objTransformationLabel->setVisible(true);
    ui->objNameLabel->setVisible(true);
    ui->objNameValue->setVisible(true);
    ui->objSettingsLabel->setVisible(true);
    ui->textureLabel->setVisible(true);
    ui->sceneTextureLabel->setVisible(true);
    ui->sceneTexture->setVisible(true);
}

int MainWindow::getSelectedItem(QString itemName)
{
    int index = -1;

    for(uint i = 0; i < meshes.size(); i++)
    {
        if(itemName == meshes[i].name)
        {
            index = i;
            break;
        }
    }

    return index;
}

void MainWindow::clearTree()
{
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
        delete ui->treeWidget->topLevelItem(i);

    ui->treeWidget->clear();
}

void MainWindow::refreshTree()
{
    // Reload texture
    for(uint i = 0; i < meshes.size(); i++)
    {
        meshes[i].texture.loadTexture();

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, meshes[i].name);
        item->setIcon(0, QIcon(":/qss_icons/icons/model.png"));
        ui->treeWidget->addTopLevelItem(item);
    }

    QTreeWidgetItem *light = new QTreeWidgetItem();
    light->setText(0, "Light");
    light->setIcon(0, QIcon(":/qss_icons/icons/light.png"));
    ui->treeWidget->addTopLevelItem(light);

    QTreeWidgetItem *camera = new QTreeWidgetItem();
    camera->setText(0, "Camera");
    camera->setIcon(0, QIcon(":/qss_icons/icons/camera.png"));
    ui->treeWidget->addTopLevelItem(camera);
}

void MainWindow::prepareMenu(const QPoint &pos)
{
    QTreeWidgetItem *nd = ui->treeWidget->itemAt(pos);
    QString name = nd->text(0);

    if(nd->text(0) == "Light" || nd->text(0) == "Camera")
        return;

    QAction *newAct = new QAction(QIcon(":/qss_icons/icons/plus.png"), tr("&Delete"), this);

    connect(newAct, &QAction::triggered, [this, name]{ deleteItem(name); });

    QMenu menu(this);
    menu.addAction(newAct);

    QPoint pt(pos);
    menu.exec(ui->treeWidget->mapToGlobal(pos));
}

void MainWindow::deleteItem(QString name)
{
    int index;
    for(uint i = 0; i < meshes.size(); i++)
    {
        if(meshes[i].name == name)
        {
            index = i;
            break;
        }
    }

    meshes.erase(meshes.begin() + index);

    clearTree();
    refreshTree();
    refreshScene();
}

void MainWindow::on_objNameValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());
    uint index = getSelectedItem(currentItem);

    if(ui->objNameValue->text().isEmpty())
    {
        QToolTip::showText(ui->objNameValue->mapToGlobal(QPoint(0, 0)), "This field cannot be empty!");
        ui->objNameValue->setText(meshes[index].name);
        return;
    }
    else
    {
        for(uint i = 0; i < meshes.size(); i++)
        {
            if(ui->objNameValue->text() == meshes[i].name && i != index)
            {
                QToolTip::showText(ui->objNameValue->mapToGlobal(QPoint(0, 0)), "That name already exists!");
                ui->objNameValue->setText(meshes[index].name);
                return;
            }
        }

        QString name = ui->objNameValue->text();
        meshes[index].name = name;

        QList<QTreeWidgetItem *> itemList;
        itemList = ui->treeWidget->selectedItems();
        foreach(QTreeWidgetItem *item, itemList)
        {
           item->setText(0, name);
        }
    }
}

void MainWindow::on_addTransXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == "Light")
    {
        scene.light.light.setX(scene.light.light.x() + 1);
        ui->transXValue->setText(QString::number(scene.light.light.x()));
    }
    else if(currentItem == "Camera")
    {
        scene.camera.position.setX(scene.camera.position.x() + 1);
        ui->transXValue->setText(QString::number(scene.camera.position.x()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            meshes[index].position.setX(meshes[index].position.x() + 1);
            ui->transXValue->setText(QString::number(meshes[index].position.x()));
        }
    }

    refreshScene();
}

void MainWindow::on_subTransXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == "Light")
    {
        scene.light.light.setX(scene.light.light.x() - 1);
        ui->transXValue->setText(QString::number(scene.light.light.x()));
    }
    else if(currentItem == "Camera")
    {
        scene.camera.position.setX(scene.camera.position.x() - 1);
        ui->transXValue->setText(QString::number(scene.camera.position.x()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            meshes[index].position.setX(meshes[index].position.x() - 1);
            ui->transXValue->setText(QString::number(meshes[index].position.x()));
        }
    }

    refreshScene();
}

void MainWindow::on_addTransYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == "Light")
    {
        scene.light.light.setY(scene.light.light.y() + 1);
        ui->transYValue->setText(QString::number(scene.light.light.y()));
    }
    else if(currentItem == "Camera")
    {
        scene.camera.position.setY(scene.camera.position.y() + 1);
        ui->transYValue->setText(QString::number(scene.camera.position.y()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            meshes[index].position.setY(meshes[index].position.y() + 1);
            ui->transYValue->setText(QString::number(meshes[index].position.y()));
        }
    }

    refreshScene();
}

void MainWindow::on_subTransYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == "Light")
    {
        scene.light.light.setY(scene.light.light.y() - 1);
        ui->transYValue->setText(QString::number(scene.light.light.y()));
    }
    else if(currentItem == "Camera")
    {
        scene.camera.position.setY(scene.camera.position.y() - 1);
        ui->transYValue->setText(QString::number(scene.camera.position.y()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            meshes[index].position.setY(meshes[index].position.y() - 1);
            ui->transYValue->setText(QString::number(meshes[index].position.y()));
        }
    }

    refreshScene();
}

void MainWindow::on_addTransZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == "Light")
    {
        scene.light.light.setZ(scene.light.light.z() + 1);
        ui->transZValue->setText(QString::number(scene.light.light.z()));
    }
    else if(currentItem == "Camera")
    {
        scene.camera.position.setZ(scene.camera.position.z() + 1);
        ui->transZValue->setText(QString::number(scene.camera.position.z()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            meshes[index].position.setZ(meshes[index].position.z() + 1);
            ui->transZValue->setText(QString::number(meshes[index].position.z()));
        }
    }

    refreshScene();
}

void MainWindow::on_subTransZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == "Light")
    {
        scene.light.light.setZ(scene.light.light.z() - 1);
        ui->transZValue->setText(QString::number(scene.light.light.z()));
    }
    else if(currentItem == "Camera")
    {
        scene.camera.position.setZ(scene.camera.position.z() - 1);
        ui->transZValue->setText(QString::number(scene.camera.position.z()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            meshes[index].position.setZ(meshes[index].position.z() - 1);
            ui->transZValue->setText(QString::number(meshes[index].position.z()));
        }
    }

    refreshScene();
}

void MainWindow::on_addRotXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].rotation.setX(meshes[index].rotation.x() + 1);
        ui->rotXValue->setText(QString::number(meshes[index].rotation.x()));
    }

    refreshScene();
}

void MainWindow::on_subRotXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].rotation.setX(meshes[index].rotation.x() - 1);
        ui->rotXValue->setText(QString::number(meshes[index].rotation.x()));
    }

    refreshScene();
}

void MainWindow::on_addRotYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].rotation.setY(meshes[index].rotation.y() + 1);
        ui->rotYValue->setText(QString::number(meshes[index].rotation.y()));
    }

    refreshScene();
}

void MainWindow::on_subRotYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].rotation.setY(meshes[index].rotation.y() - 1);
        ui->rotYValue->setText(QString::number(meshes[index].rotation.y()));
    }

    refreshScene();
}

void MainWindow::on_addRotZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].rotation.setZ(meshes[index].rotation.z() + 1);
        ui->rotZValue->setText(QString::number(meshes[index].rotation.z()));
    }

    refreshScene();
}

void MainWindow::on_subRotZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].rotation.setZ(meshes[index].rotation.z() - 1);
        ui->rotZValue->setText(QString::number(meshes[index].rotation.z()));
    }

    refreshScene();
}

void MainWindow::on_addScXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].scaling.setX(meshes[index].scaling.x() + 1);
        ui->scXValue->setText(QString::number(meshes[index].scaling.x()));
    }

    refreshScene();
}

void MainWindow::on_subScXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].scaling.setX(meshes[index].scaling.x() - 1);
        ui->scXValue->setText(QString::number(meshes[index].scaling.x()));
    }

    refreshScene();
}

void MainWindow::on_addScYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].scaling.setY(meshes[index].scaling.y() + 1);
        ui->scYValue->setText(QString::number(meshes[index].scaling.y()));
    }

    refreshScene();
}

void MainWindow::on_subScYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].scaling.setY(meshes[index].scaling.y() - 1);
        ui->scYValue->setText(QString::number(meshes[index].scaling.y()));
    }

    refreshScene();
}

void MainWindow::on_addScZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].scaling.setZ(meshes[index].scaling.z() + 1);
        ui->scZValue->setText(QString::number(meshes[index].scaling.z()));
    }

    refreshScene();
}

void MainWindow::on_subScZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].scaling.setZ(meshes[index].scaling.z() - 1);
        ui->scZValue->setText(QString::number(meshes[index].scaling.z()));
    }

    refreshScene();
}

void MainWindow::on_addShXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].shearing.setX(meshes[index].shearing.x() + 1);
        ui->shXValue->setText(QString::number(meshes[index].shearing.x()));
    }

    refreshScene();
}

void MainWindow::on_subShXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].shearing.setX(meshes[index].shearing.x() - 1);
        ui->shXValue->setText(QString::number(meshes[index].shearing.x()));
    }

    refreshScene();
}

void MainWindow::on_addShYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].shearing.setY(meshes[index].shearing.y() + 1);
        ui->shYValue->setText(QString::number(meshes[index].shearing.y()));
    }

    refreshScene();
}

void MainWindow::on_subShYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].shearing.setY(meshes[index].shearing.y() - 1);
        ui->shYValue->setText(QString::number(meshes[index].shearing.y()));
    }

    refreshScene();
}

void MainWindow::on_addShZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].shearing.setZ(meshes[index].shearing.z() + 1);
        ui->shZValue->setText(QString::number(meshes[index].shearing.z()));
    }

    refreshScene();
}

void MainWindow::on_subShZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].shearing.setZ(meshes[index].shearing.z() - 1);
        ui->shZValue->setText(QString::number(meshes[index].shearing.z()));
    }

    refreshScene();
}

void MainWindow::on_transXValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == "Light")
    {
        scene.light.light.setX(ui->transXValue->text().toInt());
    }
    else if(currentItem == "Camera")
    {
        scene.camera.position.setX(ui->transXValue->text().toInt());
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            meshes[index].position.setX(ui->transXValue->text().toInt());
        }
    }

    refreshScene();
}

void MainWindow::on_transYValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == "Light")
    {
        scene.light.light.setY(ui->transYValue->text().toInt());
    }
    else if(currentItem == "Camera")
    {
        scene.camera.position.setY(ui->transYValue->text().toInt());
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            meshes[index].position.setY(ui->transYValue->text().toInt());
        }
    }

    refreshScene();
}

void MainWindow::on_transZValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == "Light")
    {
        scene.light.light.setZ(ui->transZValue->text().toInt());
    }
    else if(currentItem == "Camera")
    {
        scene.camera.position.setZ(ui->transZValue->text().toInt());
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            meshes[index].position.setZ(ui->transZValue->text().toInt());
        }
    }

    refreshScene();
}

void MainWindow::on_rotXValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].rotation.setX(ui->rotXValue->text().toInt());
    }

    refreshScene();
}

void MainWindow::on_rotYValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].rotation.setY(ui->rotYValue->text().toInt());
    }

    refreshScene();
}

void MainWindow::on_rotZValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].rotation.setZ(ui->rotZValue->text().toInt());
    }

    refreshScene();
}

void MainWindow::on_scXValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].scaling.setX(ui->scXValue->text().toInt());
    }

    refreshScene();
}

void MainWindow::on_scYValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].scaling.setY(ui->scYValue->text().toInt());
    }

    refreshScene();
}

void MainWindow::on_scZValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].scaling.setZ(ui->scZValue->text().toInt());
    }

    refreshScene();
}

void MainWindow::on_shXValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].shearing.setX(ui->shXValue->text().toInt());
    }

    refreshScene();
}

void MainWindow::on_shYValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].shearing.setY(ui->shYValue->text().toInt());
    }

    refreshScene();
}

void MainWindow::on_shZValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        meshes[index].shearing.setZ(ui->shZValue->text().toInt());
    }

    refreshScene();
}

void MainWindow::on_cubeButton_clicked()
{
    if(currentProject.isEmpty())
        return;

    XMLParser *xml = new XMLParser();
    xml->readModel(meshes, ":/models/models/cube.xml");

    clearTree();
    refreshTree();
    refreshScene();

    delete xml;
}

void MainWindow::on_cylinderButton_clicked()
{
    if(currentProject.isEmpty())
        return;

    XMLParser *xml = new XMLParser();
    xml->readModel(meshes, ":/models/models/cylinder.xml");

    clearTree();
    refreshTree();
    refreshScene();

    delete xml;
}

void MainWindow::on_pyramidButton_clicked()
{
    if(currentProject.isEmpty())
        return;

    XMLParser *xml = new XMLParser();
    xml->readModel(meshes, ":/models/models/cone.xml");

    clearTree();
    refreshTree();
    refreshScene();

    delete xml;
}

void MainWindow::on_sphereButton_clicked()
{
    if(currentProject.isEmpty())
        return;

    XMLParser *xml = new XMLParser();
    xml->readModel(meshes, ":/models/models/sphere.xml");

    clearTree();
    refreshTree();
    refreshScene();

    delete xml;
}
