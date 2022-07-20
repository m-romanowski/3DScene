#include "mainwindow.h"
#include "ui_mainwindow.h"

const QString MainWindow::LIGHT_OBJECT_NAME = "Light";
const QString MainWindow::CAMERA_OBJECT_NAME = "Camera";
const QString MainWindow::OBJECT_TEXTURE_NAME = "objTexture";
const QString MainWindow::SCENE_TEXTURE_NAME = "sceneTexture";

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

    ui->objSettingsLabel->setVisible(false);
    ui->objNameLabel->setVisible(false);
    ui->objNameValue->setVisible(false);
    ui->objTransformationLabel->setVisible(false);
    ui->translationGroupBox->setVisible(false);
    ui->rotationGroupBox->setVisible(false);
    ui->scaleGroupBox->setVisible(false);
    ui->skewGroupBox->setVisible(false);
    ui->textureLabel->setVisible(false);
    ui->textureGroupBox->setVisible(false);

    // Remove scene objects
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::prepareMenu);

    // Animation
    animationStarted = false;

    // Current project name reference
    currentProject = "";

    renderer = std::make_unique<Renderer>(0, 0, this->width(), this->height());
    ui->sceneContainer->addWidget(renderer.get());

    // Scene texture miniature
    QPixmap pixmap(ui->sceneTexture->width(), ui->sceneTexture->height());
    pixmap.fill(QColor(getScene()->color.getRed(), getScene()->color.getGreen(), getScene()->color.getBlue(), getScene()->color.getAlpha()));
    ui->sceneTexture->setPixmap(pixmap);
}

MainWindow::~MainWindow()
{
    delete ui;
}

Scene* MainWindow::getScene()
{
    return renderer->getScene();
}

std::vector<Mesh>& MainWindow::getMeshes()
{
    return getScene()->meshes;
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

    if(label->objectName() == OBJECT_TEXTURE_NAME)
    {
        QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());
        index = getSelectedItem(currentItem);
    }

    if(label->objectName() == SCENE_TEXTURE_NAME && !getScene()->texture.filename.isEmpty())
        filename = getScene()->texture.filename;
    if(label->objectName() == SCENE_TEXTURE_NAME && getScene()->texture.filename.isEmpty())
        color = getScene()->color;

    if(label->objectName() == OBJECT_TEXTURE_NAME && !getMeshes()[index].texture.filename.isEmpty() && index >= 0)
        filename = getMeshes()[index].texture.filename;
    if(label->objectName() == OBJECT_TEXTURE_NAME && getMeshes()[index].texture.filename.isEmpty() && index >= 0)
        color = getMeshes()[index].texture.color;

    TextureDialog *textureDialog = new TextureDialog(filename, color, this);
    textureDialog->setModal(true);
    textureDialog->exec();

    if(label->objectName() == SCENE_TEXTURE_NAME && !textureDialog->getFilename().isEmpty())
    {
        getScene()->texture.filename = textureDialog->getFilename();

        // Change texture miniature
        ui->sceneTexture->setPixmap(textureDialog->getTexture().scaled(ui->sceneTexture->width(), ui->sceneTexture->height()));

        // Reload texture widget
        getScene()->texture.loadTexture();
    }

    if(label->objectName() == SCENE_TEXTURE_NAME && textureDialog->getFilename().isEmpty())
    {
        getScene()->color = textureDialog->getColor();
        getScene()->texture.filename = textureDialog->getFilename();

        // Change texture miniature
        ui->sceneTexture->setPixmap(textureDialog->getTexture().scaled(ui->sceneTexture->width(), ui->sceneTexture->height()));

        // Reload texture widget
        getScene()->texture.loadTexture();
    }

    if(label->objectName() == OBJECT_TEXTURE_NAME && !textureDialog->getFilename().isEmpty() && index >= 0)
    {
        getMeshes()[index].texture.filename = textureDialog->getFilename();
        getMeshes()[index].texture.loadTexture();

        // Change texture miniature
        ui->objTexture->setPixmap(textureDialog->getTexture().scaled(ui->sceneTexture->width(), ui->sceneTexture->height()));
    }

    if(label->objectName() == OBJECT_TEXTURE_NAME && textureDialog->getFilename().isEmpty() && index >= 0)
    {
        getMeshes()[index].texture.color = textureDialog->getColor();
        getMeshes()[index].texture.filename = textureDialog->getFilename();

        // Change texture miniature
        ui->objTexture->setPixmap(textureDialog->getTexture().scaled(ui->sceneTexture->width(), ui->sceneTexture->height()));

        // Reload texture widget
        getMeshes()[index].texture.loadTexture();
    }

    // Reload scene
    renderer->refreshScene();
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

    AnimationDialog *animationDialog = new AnimationDialog(getMeshes()[index].name ,this);
    animationDialog->setModal(true);
    animationDialog->exec();

    getMeshes()[index].animation = animationDialog->getAnimation();
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

void MainWindow::animation()
{
    renderer->clearScene();

    for(uint i = 0; i < getMeshes().size(); i++)
    {
        getMeshes()[i].position = QVector3D(getMeshes()[i].position.x() + getMeshes()[i].animation.translation.x(),
                                            getMeshes()[i].position.y() + getMeshes()[i].animation.translation.y(),
                                            getMeshes()[i].position.z() + getMeshes()[i].animation.translation.z());

        getMeshes()[i].rotation = QVector3D(getMeshes()[i].rotation.x() + getMeshes()[i].animation.rotation.x(),
                                            getMeshes()[i].rotation.y() + getMeshes()[i].animation.rotation.y(),
                                            getMeshes()[i].rotation.z() + getMeshes()[i].animation.rotation.z());

        renderer->refreshScene();
    }
}

void MainWindow::on_newProjectButton_clicked()
{
    if(!currentProject.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      tr("Information"),
                                      tr("Are you sure you want to open a new project?"),
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::No)
            return;
    }

    // Reload objects on the tree
    clearTree();
    getScene()->clearMeshes();
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
    renderer->setScene(new Scene(newScene));
    refreshTree();

    currentProject = "NewProject";

    // Reload scene
    renderer->refreshScene();
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
                                       tr("Information"),
                                       "This project has already been opened.",
                                       QMessageBox::Ok);
        if(message == QMessageBox::Ok)
            return;
    }

    // Clear all objects on the scene
    clearTree();
    getScene()->clearMeshes();

    XMLParser *xml = new XMLParser();
    xml->readXML(filename);

    Scene scene = xml->getScene();

    for(uint i = 0; i < getMeshes().size(); i++)
        getMeshes()[i].id = i + 1;

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

    currentProject = filename;
    renderer->setScene(new Scene(scene));
    renderer->refreshScene();
    refreshTree();

    delete xml;
}

void MainWindow::on_saveProjectButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save a project file"),
                                                    "C://",
                                                    "XML File(*.xml)");
    XMLParser *xml = new XMLParser();
    xml->writeXML(getMeshes(), *getScene(), filename);
    delete xml;
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    int index = -1;

    for(uint i = 0; i < getMeshes().size(); i++)
    {
        if(item->text(column) == getMeshes()[i].name)
        {
            index = i;
            break;
        }
    }

    if((index == -1 && item->text(column) == LIGHT_OBJECT_NAME) || (index == -1 && item->text(column) == CAMERA_OBJECT_NAME))
    {
        ui->objNameValue->setText(item->text(column));
        ui->objNameValue->setEnabled(false);
        ui->translationGroupBox->setVisible(true);
        ui->rotationGroupBox->setVisible(false);
        ui->scaleGroupBox->setVisible(false);
        ui->skewGroupBox->setVisible(false);
        ui->objTextureLabel->setVisible(false);
        ui->objTexture->setVisible(false);

        if(item->text(column) == LIGHT_OBJECT_NAME)
        {
            ui->transXValue->setText(QString::number(getScene()->light.light.x()));
            ui->transYValue->setText(QString::number(getScene()->light.light.y()));
            ui->transZValue->setText(QString::number(getScene()->light.light.z()));
        }

        if(item->text(column) == CAMERA_OBJECT_NAME)
        {
            ui->transXValue->setText(QString::number(getScene()->camera.position.x()));
            ui->transYValue->setText(QString::number(getScene()->camera.position.y()));
            ui->transZValue->setText(QString::number(getScene()->camera.position.z()));
        }
    }
    else
    {
        ui->objNameValue->setEnabled(true);
        ui->objNameValue->setText(getMeshes()[index].name);
        ui->translationGroupBox->setVisible(true);
        ui->rotationGroupBox->setVisible(true);
        ui->scaleGroupBox->setVisible(true);
        ui->skewGroupBox->setVisible(true);
        ui->objTextureLabel->setVisible(true);
        ui->objTexture->setVisible(true);

        ui->transXValue->setText(QString::number(getMeshes()[index].position.x()));
        ui->transYValue->setText(QString::number(getMeshes()[index].position.y()));
        ui->transZValue->setText(QString::number(getMeshes()[index].position.z()));

        ui->rotXValue->setText(QString::number(getMeshes()[index].rotation.x()));
        ui->rotYValue->setText(QString::number(getMeshes()[index].rotation.y()));
        ui->rotZValue->setText(QString::number(getMeshes()[index].rotation.z()));

        ui->scXValue->setText(QString::number(getMeshes()[index].scale.x()));
        ui->scYValue->setText(QString::number(getMeshes()[index].scale.y()));
        ui->scZValue->setText(QString::number(getMeshes()[index].scale.z()));

        ui->shXValue->setText(QString::number(getMeshes()[index].skew.x()));
        ui->shYValue->setText(QString::number(getMeshes()[index].skew.y()));
        ui->shZValue->setText(QString::number(getMeshes()[index].skew.z()));

        if(!getMeshes()[index].texture.filename.isEmpty())
        {
            QPixmap pixmap(getMeshes()[index].texture.filename);
            ui->objTexture->setPixmap(pixmap.scaled(ui->objTexture->width(), ui->objTexture->height()));
        }
        else
        {
            QPixmap pixmap(ui->objTexture->width(), ui->objTexture->height());
            pixmap.fill(QColor(getMeshes()[index].texture.color.getRed(),
                               getMeshes()[index].texture.color.getGreen(),
                               getMeshes()[index].texture.color.getBlue(),
                               getMeshes()[index].texture.color.getAlpha()));
            ui->objTexture->setPixmap(pixmap);
        }
    }

    ui->objSettingsLabel->setVisible(true);
    ui->objNameLabel->setVisible(true);
    ui->objNameValue->setVisible(true);
    ui->objTransformationLabel->setVisible(true);
    ui->textureLabel->setVisible(true);
    ui->textureGroupBox->setVisible(true);
}

int MainWindow::getSelectedItem(QString itemName)
{
    int index = -1;

    for(uint i = 0; i < getMeshes().size(); i++)
    {
        if(itemName == getMeshes()[i].name)
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
    for(uint i = 0; i < getMeshes().size(); i++)
    {
        getMeshes()[i].texture.loadTexture();

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, getMeshes()[i].name);
        item->setIcon(0, QIcon(":/qss_icons/icons/model.png"));
        ui->treeWidget->addTopLevelItem(item);
    }

    QTreeWidgetItem *light = new QTreeWidgetItem();
    light->setText(0, LIGHT_OBJECT_NAME);
    light->setIcon(0, QIcon(":/qss_icons/icons/light.png"));
    ui->treeWidget->addTopLevelItem(light);

    QTreeWidgetItem *camera = new QTreeWidgetItem();
    camera->setText(0, CAMERA_OBJECT_NAME);
    camera->setIcon(0, QIcon(":/qss_icons/icons/camera.png"));
    ui->treeWidget->addTopLevelItem(camera);
}

void MainWindow::prepareMenu(const QPoint &pos)
{
    QTreeWidgetItem *nd = ui->treeWidget->itemAt(pos);
    QString name = nd->text(0);

    if(nd->text(0) == LIGHT_OBJECT_NAME || nd->text(0) == CAMERA_OBJECT_NAME)
        return;

    QAction *newAct = new QAction(QIcon(":/qss_icons/icons/plus.png"), tr("&Delete"), this);
    connect(newAct, &QAction::triggered, this, [this, name]{ deleteItem(name); });

    QMenu menu(this);
    menu.addAction(newAct);
    menu.exec(ui->treeWidget->mapToGlobal(pos));
}

void MainWindow::deleteItem(QString name)
{
    auto foundMesh = std::find_if(getMeshes().begin(), getMeshes().end(), [&name](const Mesh& mesh) {
        return mesh.name == name;
    });

    if (foundMesh != getMeshes().end())
    {
        getMeshes().erase(getMeshes().begin() + std::distance(getMeshes().begin(), foundMesh));
        clearTree();
        refreshTree();
        renderer->refreshScene();
    }
}

void MainWindow::on_objNameValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());
    uint index = getSelectedItem(currentItem);

    if(ui->objNameValue->text().isEmpty())
    {
        QToolTip::showText(ui->objNameValue->mapToGlobal(QPoint(0, 0)), "This field cannot be empty!");
        ui->objNameValue->setText(getMeshes()[index].name);
        return;
    }
    else
    {
        for(uint i = 0; i < getMeshes().size(); i++)
        {
            if(ui->objNameValue->text() == getMeshes()[i].name && i != index)
            {
                QToolTip::showText(ui->objNameValue->mapToGlobal(QPoint(0, 0)), "That name already exists!");
                ui->objNameValue->setText(getMeshes()[index].name);
                return;
            }
        }

        QString name = ui->objNameValue->text();
        getMeshes()[index].name = name;

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

    if(currentItem == LIGHT_OBJECT_NAME)
    {
        getScene()->light.light.setX(getScene()->light.light.x() + 1);
        ui->transXValue->setText(QString::number(getScene()->light.light.x()));
    }
    else if(currentItem == CAMERA_OBJECT_NAME)
    {
        getScene()->camera.position.setX(getScene()->camera.position.x() + 1);
        ui->transXValue->setText(QString::number(getScene()->camera.position.x()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            getMeshes()[index].position.setX(getMeshes()[index].position.x() + 1);
            ui->transXValue->setText(QString::number(getMeshes()[index].position.x()));
        }
    }

    renderer->refreshScene();
}

void MainWindow::on_subTransXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == LIGHT_OBJECT_NAME)
    {
        getScene()->light.light.setX(getScene()->light.light.x() - 1);
        ui->transXValue->setText(QString::number(getScene()->light.light.x()));
    }
    else if(currentItem == CAMERA_OBJECT_NAME)
    {
        getScene()->camera.position.setX(getScene()->camera.position.x() - 1);
        ui->transXValue->setText(QString::number(getScene()->camera.position.x()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            getMeshes()[index].position.setX(getMeshes()[index].position.x() - 1);
            ui->transXValue->setText(QString::number(getMeshes()[index].position.x()));
        }
    }

    renderer->refreshScene();
}

void MainWindow::on_addTransYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == LIGHT_OBJECT_NAME)
    {
        getScene()->light.light.setY(getScene()->light.light.y() + 1);
        ui->transYValue->setText(QString::number(getScene()->light.light.y()));
    }
    else if(currentItem == CAMERA_OBJECT_NAME)
    {
        getScene()->camera.position.setY(getScene()->camera.position.y() + 1);
        ui->transYValue->setText(QString::number(getScene()->camera.position.y()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            getMeshes()[index].position.setY(getMeshes()[index].position.y() + 1);
            ui->transYValue->setText(QString::number(getMeshes()[index].position.y()));
        }
    }

    renderer->refreshScene();
}

void MainWindow::on_subTransYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == LIGHT_OBJECT_NAME)
    {
        getScene()->light.light.setY(getScene()->light.light.y() - 1);
        ui->transYValue->setText(QString::number(getScene()->light.light.y()));
    }
    else if(currentItem == CAMERA_OBJECT_NAME)
    {
        getScene()->camera.position.setY(getScene()->camera.position.y() - 1);
        ui->transYValue->setText(QString::number(getScene()->camera.position.y()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            getMeshes()[index].position.setY(getMeshes()[index].position.y() - 1);
            ui->transYValue->setText(QString::number(getMeshes()[index].position.y()));
        }
    }

    renderer->refreshScene();
}

void MainWindow::on_addTransZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == LIGHT_OBJECT_NAME)
    {
        getScene()->light.light.setZ(getScene()->light.light.z() + 1);
        ui->transZValue->setText(QString::number(getScene()->light.light.z()));
    }
    else if(currentItem == CAMERA_OBJECT_NAME)
    {
        getScene()->camera.position.setZ(getScene()->camera.position.z() + 1);
        ui->transZValue->setText(QString::number(getScene()->camera.position.z()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            getMeshes()[index].position.setZ(getMeshes()[index].position.z() + 1);
            ui->transZValue->setText(QString::number(getMeshes()[index].position.z()));
        }
    }

    renderer->refreshScene();
}

void MainWindow::on_subTransZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == LIGHT_OBJECT_NAME)
    {
        getScene()->light.light.setZ(getScene()->light.light.z() - 1);
        ui->transZValue->setText(QString::number(getScene()->light.light.z()));
    }
    else if(currentItem == CAMERA_OBJECT_NAME)
    {
        getScene()->camera.position.setZ(getScene()->camera.position.z() - 1);
        ui->transZValue->setText(QString::number(getScene()->camera.position.z()));
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            getMeshes()[index].position.setZ(getMeshes()[index].position.z() - 1);
            ui->transZValue->setText(QString::number(getMeshes()[index].position.z()));
        }
    }

    renderer->refreshScene();
}

void MainWindow::on_addRotXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].rotation.setX(getMeshes()[index].rotation.x() + 1);
        ui->rotXValue->setText(QString::number(getMeshes()[index].rotation.x()));
    }

    renderer->refreshScene();
}

void MainWindow::on_subRotXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].rotation.setX(getMeshes()[index].rotation.x() - 1);
        ui->rotXValue->setText(QString::number(getMeshes()[index].rotation.x()));
    }

    renderer->refreshScene();
}

void MainWindow::on_addRotYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].rotation.setY(getMeshes()[index].rotation.y() + 1);
        ui->rotYValue->setText(QString::number(getMeshes()[index].rotation.y()));
    }

    renderer->refreshScene();
}

void MainWindow::on_subRotYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].rotation.setY(getMeshes()[index].rotation.y() - 1);
        ui->rotYValue->setText(QString::number(getMeshes()[index].rotation.y()));
    }

    renderer->refreshScene();
}

void MainWindow::on_addRotZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].rotation.setZ(getMeshes()[index].rotation.z() + 1);
        ui->rotZValue->setText(QString::number(getMeshes()[index].rotation.z()));
    }

    renderer->refreshScene();
}

void MainWindow::on_subRotZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].rotation.setZ(getMeshes()[index].rotation.z() - 1);
        ui->rotZValue->setText(QString::number(getMeshes()[index].rotation.z()));
    }

    renderer->refreshScene();
}

void MainWindow::on_addScXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].scale.setX(getMeshes()[index].scale.x() + 1);
        ui->scXValue->setText(QString::number(getMeshes()[index].scale.x()));
    }

    renderer->refreshScene();
}

void MainWindow::on_subScXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].scale.setX(getMeshes()[index].scale.x() - 1);
        ui->scXValue->setText(QString::number(getMeshes()[index].scale.x()));
    }

    renderer->refreshScene();
}

void MainWindow::on_addScYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].scale.setY(getMeshes()[index].scale.y() + 1);
        ui->scYValue->setText(QString::number(getMeshes()[index].scale.y()));
    }

    renderer->refreshScene();
}

void MainWindow::on_subScYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].scale.setY(getMeshes()[index].scale.y() - 1);
        ui->scYValue->setText(QString::number(getMeshes()[index].scale.y()));
    }

    renderer->refreshScene();
}

void MainWindow::on_addScZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].scale.setZ(getMeshes()[index].scale.z() + 1);
        ui->scZValue->setText(QString::number(getMeshes()[index].scale.z()));
    }

    renderer->refreshScene();
}

void MainWindow::on_subScZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].scale.setZ(getMeshes()[index].scale.z() - 1);
        ui->scZValue->setText(QString::number(getMeshes()[index].scale.z()));
    }

    renderer->refreshScene();
}

void MainWindow::on_addShXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].skew.setX(getMeshes()[index].skew.x() + 1);
        ui->shXValue->setText(QString::number(getMeshes()[index].skew.x()));
    }

    renderer->refreshScene();
}

void MainWindow::on_subShXButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].skew.setX(getMeshes()[index].skew.x() - 1);
        ui->shXValue->setText(QString::number(getMeshes()[index].skew.x()));
    }

    renderer->refreshScene();
}

void MainWindow::on_addShYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].skew.setY(getMeshes()[index].skew.y() + 1);
        ui->shYValue->setText(QString::number(getMeshes()[index].skew.y()));
    }

    renderer->refreshScene();
}

void MainWindow::on_subShYButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].skew.setY(getMeshes()[index].skew.y() - 1);
        ui->shYValue->setText(QString::number(getMeshes()[index].skew.y()));
    }

    renderer->refreshScene();
}

void MainWindow::on_addShZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].skew.setZ(getMeshes()[index].skew.z() + 1);
        ui->shZValue->setText(QString::number(getMeshes()[index].skew.z()));
    }

    renderer->refreshScene();
}

void MainWindow::on_subShZButton_clicked()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].skew.setZ(getMeshes()[index].skew.z() - 1);
        ui->shZValue->setText(QString::number(getMeshes()[index].skew.z()));
    }

    renderer->refreshScene();
}

void MainWindow::on_transXValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == LIGHT_OBJECT_NAME)
    {
        getScene()->light.light.setX(ui->transXValue->text().toInt());
    }
    else if(currentItem == CAMERA_OBJECT_NAME)
    {
        getScene()->camera.position.setX(ui->transXValue->text().toInt());
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            getMeshes()[index].position.setX(ui->transXValue->text().toInt());
        }
    }

    renderer->refreshScene();
}

void MainWindow::on_transYValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == LIGHT_OBJECT_NAME)
    {
        getScene()->light.light.setY(ui->transYValue->text().toInt());
    }
    else if(currentItem == CAMERA_OBJECT_NAME)
    {
        getScene()->camera.position.setY(ui->transYValue->text().toInt());
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            getMeshes()[index].position.setY(ui->transYValue->text().toInt());
        }
    }

    renderer->refreshScene();
}

void MainWindow::on_transZValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    if(currentItem == LIGHT_OBJECT_NAME)
    {
        getScene()->light.light.setZ(ui->transZValue->text().toInt());
    }
    else if(currentItem == CAMERA_OBJECT_NAME)
    {
        getScene()->camera.position.setZ(ui->transZValue->text().toInt());
    }
    else
    {
        int index = getSelectedItem(currentItem);

        if(index >= 0)
        {
            getMeshes()[index].position.setZ(ui->transZValue->text().toInt());
        }
    }

    renderer->refreshScene();
}

void MainWindow::on_rotXValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].rotation.setX(ui->rotXValue->text().toInt());
    }

    renderer->refreshScene();
}

void MainWindow::on_rotYValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].rotation.setY(ui->rotYValue->text().toInt());
    }

    renderer->refreshScene();
}

void MainWindow::on_rotZValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].rotation.setZ(ui->rotZValue->text().toInt());
    }

    renderer->refreshScene();
}

void MainWindow::on_scXValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].scale.setX(ui->scXValue->text().toInt());
    }

    renderer->refreshScene();
}

void MainWindow::on_scYValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].scale.setY(ui->scYValue->text().toInt());
    }

    renderer->refreshScene();
}

void MainWindow::on_scZValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].scale.setZ(ui->scZValue->text().toInt());
    }

    renderer->refreshScene();
}

void MainWindow::on_shXValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].skew.setX(ui->shXValue->text().toInt());
    }

    renderer->refreshScene();
}

void MainWindow::on_shYValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].skew.setY(ui->shYValue->text().toInt());
    }

    renderer->refreshScene();
}

void MainWindow::on_shZValue_editingFinished()
{
    QString currentItem = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

    int index = getSelectedItem(currentItem);
    if(index >= 0)
    {
        getMeshes()[index].skew.setZ(ui->shZValue->text().toInt());
    }

    renderer->refreshScene();
}

void MainWindow::loadModel(QString resourceName)
{
    if(currentProject.isEmpty())
        return;

    XMLParser *xml = new XMLParser();
    xml->readModel(getMeshes(), resourceName);

    clearTree();
    refreshTree();
    renderer->refreshScene();

    delete xml;
}

void MainWindow::on_cubeButton_clicked()
{    
    loadModel(":/models/models/cube.xml");
}

void MainWindow::on_cylinderButton_clicked()
{    
    loadModel(":/models/models/cylinder.xml");
}

void MainWindow::on_pyramidButton_clicked()
{    
    loadModel(":/models/models/cone.xml");
}

void MainWindow::on_sphereButton_clicked()
{
    loadModel(":/models/models/sphere.xml");
}
