#include "xmlparser.h"

XMLParser::XMLParser()
{
}

void XMLParser::writeXML(std::vector<Mesh> meshes, Scene scene, QString filename)
{
    QDomDocument document;

    QDomElement root = document.createElement("project");
    document.appendChild(root);

    for(uint i = 0; i < meshes.size(); i++)
    {
        QDomElement mesh = document.createElement("mesh");
        mesh.setAttribute("name", meshes[i].name);
        root.appendChild(mesh);

        // Object coordinates [x, y, z], [nx, ny, nz], [u, v]
        for(uint j = 0; j < meshes[i].vertices.size(); j++)
        {
            QDomElement vertice = document.createElement("vertice");

            vertice.setAttribute("x", QString::number(meshes[i].vertices[j].coords.x()));
            vertice.setAttribute("y", QString::number(meshes[i].vertices[j].coords.y()));
            vertice.setAttribute("z", QString::number(meshes[i].vertices[j].coords.z()));

            vertice.setAttribute("nx", QString::number(meshes[i].vertices[j].normal.x()));
            vertice.setAttribute("ny", QString::number(meshes[i].vertices[j].normal.y()));
            vertice.setAttribute("nz", QString::number(meshes[i].vertices[j].normal.z()));

            vertice.setAttribute("u", QString::number(meshes[i].vertices[j].textureCoords.x()));
            vertice.setAttribute("v", QString::number(meshes[i].vertices[j].textureCoords.y()));

            mesh.appendChild(vertice);
        }

        // Object faces
        for(uint k = 0; k < meshes[i].faces.size(); k++)
        {
            QDomElement face = document.createElement("face");

            face.setAttribute("A", QString::number(meshes[i].faces[k].A));
            face.setAttribute("B", QString::number(meshes[i].faces[k].B));
            face.setAttribute("C", QString::number(meshes[i].faces[k].C));

            mesh.appendChild(face);
        }

        QDomElement texture = document.createElement("texture");

        if(meshes[i].texture.filename.isEmpty()) texture.setAttribute("color", meshes[i].texture.color.toString());
        else texture.setAttribute("path", meshes[i].texture.filename);

        texture.setAttribute("width", QString::number(meshes[i].texture.width));
        texture.setAttribute("height", QString::number(meshes[i].texture.height));
        mesh.appendChild(texture);

        QDomElement position = document.createElement("position");
        position.setAttribute("x", QString::number(meshes[i].position.x()));
        position.setAttribute("y", QString::number(meshes[i].position.y()));
        position.setAttribute("z", QString::number(meshes[i].position.z()));
        mesh.appendChild(position);

        QDomElement rotation = document.createElement("rotation");
        rotation.setAttribute("x", QString::number(meshes[i].rotation.x()));
        rotation.setAttribute("y", QString::number(meshes[i].rotation.y()));
        rotation.setAttribute("z", QString::number(meshes[i].rotation.z()));
        mesh.appendChild(rotation);

        QDomElement scaling = document.createElement("scaling");
        scaling.setAttribute("x", QString::number(meshes[i].scale.x()));
        scaling.setAttribute("y", QString::number(meshes[i].scale.y()));
        scaling.setAttribute("z", QString::number(meshes[i].scale.z()));
        mesh.appendChild(scaling);

        QDomElement shearing = document.createElement("shearing");
        shearing.setAttribute("x", QString::number(meshes[i].skew.x()));
        shearing.setAttribute("y", QString::number(meshes[i].skew.y()));
        shearing.setAttribute("z", QString::number(meshes[i].skew.z()));
        mesh.appendChild(shearing);
    }

    // Scene settings: light, texture / color, camera:
    QDomElement sceneTexture = document.createElement("scene");

    if(scene.texture.filename.isEmpty()) sceneTexture.setAttribute("color", scene.color.toString());
    else sceneTexture.setAttribute("texture", scene.texture.filename);

    sceneTexture.setAttribute("light", scene.light.toString());
    sceneTexture.setAttribute("camera_position", scene.camera.toString(scene.camera.position));
    sceneTexture.setAttribute("camera_target", scene.camera.toString(scene.camera.target));
    sceneTexture.setAttribute("camera_up", scene.camera.toString(scene.camera.up));
    root.appendChild(sceneTexture);

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open the file";
        return;
    }
    else
    {
        QTextStream stream(&file);
        stream << document.toString();
        file.close();
    }
}

std::vector<float> XMLParser::convertValue(QString str)
{
    std::vector<float> numbers;
    QStringList list = str.split(" ", Qt::SkipEmptyParts);

    foreach (QString num, list)
        numbers.push_back(num.toFloat());

    return numbers;
}

void XMLParser::listElement(QDomElement root, QString tagname, std::vector<QString> &attributes, ushort type, int index = 0)
{
    // 0 -- verices
    // 1 -- faces
    // 2 -- mesh texture
    // 3 -- mesh position
    // 4 -- mesh rotation
    // 5 -- mesh scaling
    // 6 -- shearing
    // 7 -- scene parameters

    QDomNodeList items = root.elementsByTagName(tagname);

    for(int i = 0; i < items.count(); i++)
    {
        QDomNode itemNode = items.at(i);

        if(itemNode.isElement())
        {
            QDomElement itemElement = itemNode.toElement();
            //qDebug() << QString::number(i) << itemElement.attribute("x");

            // Veritices
            if(type == 0)
            {
                if(attributes.size() < 8)
                {
                    qDebug() << "Insufficient number of attributes";
                    return;
                }

                scene.meshes[index].vertices.push_back(Vertex(QVector3D(itemElement.attribute(attributes[0]).toFloat(),
                                                                  itemElement.attribute(attributes[1]).toFloat(),
                                                                  itemElement.attribute(attributes[2]).toFloat()),
                                                        QVector3D(itemElement.attribute(attributes[3]).toFloat(),
                                                                  itemElement.attribute(attributes[4]).toFloat(),
                                                                  itemElement.attribute(attributes[5]).toFloat()),
                                                        QVector2D(itemElement.attribute(attributes[6]).toFloat(),
                                                                  itemElement.attribute(attributes[7]).toFloat())));
            }

            // Faces
            if(type == 1)
            {
                if(attributes.size() < 3)
                {
                    qDebug() << "Insufficient number of attributes";
                    return;
                }

                scene.meshes[index].faces.push_back(Face(itemElement.attribute(attributes[0]).toInt(),
                                                   itemElement.attribute(attributes[1]).toInt(),
                                                   itemElement.attribute(attributes[2]).toInt()));
            }

            // Texture
            if(type == 2)
            {
                if(attributes.size() < 3)
                {
                    qDebug() << "Insufficient number of attributes";
                    return;
                }

                std::vector<float> convertedValue;

                // There is no color / texture (choose default)
                if(itemElement.attribute(attributes[0]).isEmpty() && itemElement.attribute(attributes[1]).isEmpty())
                {
                    scene.color.setRed(255);
                    scene.color.setGreen(255);
                    scene.color.setBlue(255);
                    scene.color.setAlpha(255);
                }
                // If there is texture, or there is color and texture, I give the texture
                else if((itemElement.attribute(attributes[1]).isEmpty() && !itemElement.attribute(attributes[0]).isEmpty()) ||
                        (!itemElement.attribute(attributes[1]).isEmpty() && !itemElement.attribute(attributes[0]).isEmpty()))
                {
                    scene.meshes[index].texture = Texture(itemElement.attribute(attributes[0]));
                }
                // Otherwise, I'm reading the color
                else
                {
                    convertedValue = convertValue(itemElement.attribute(attributes[1]));
                    scene.meshes[index].texture = Texture(
                                Color(convertedValue[0], convertedValue[1], convertedValue[2], convertedValue[3]),
                                itemElement.attribute(attributes[2]).toInt(),
                                itemElement.attribute(attributes[3]).toInt()
                            );
                }
            }

            if(type == 3)
            {
                if(attributes.size() < 3)
                {
                    qDebug() << "Insufficient number of attributes";
                    return;
                }

                scene.meshes[index].position.setX(itemElement.attribute(attributes[0]).toFloat());
                scene.meshes[index].position.setY(itemElement.attribute(attributes[1]).toFloat());
                scene.meshes[index].position.setZ(itemElement.attribute(attributes[2]).toFloat());
            }

            if(type == 4)
            {
                if(attributes.size() < 3)
                {
                    qDebug() << "Insufficient number of attributes";
                    return;
                }

                scene.meshes[index].rotation.setX(itemElement.attribute(attributes[0]).toFloat());
                scene.meshes[index].rotation.setY(itemElement.attribute(attributes[1]).toFloat());
                scene.meshes[index].rotation.setZ(itemElement.attribute(attributes[2]).toFloat());
            }

            if(type == 5)
            {
                if(attributes.size() < 3)
                {
                    qDebug() << "Insufficient number of attributes";
                    return;
                }

                scene.meshes[index].scale.setX(itemElement.attribute(attributes[0]).toFloat());
                scene.meshes[index].scale.setY(itemElement.attribute(attributes[1]).toFloat());
                scene.meshes[index].scale.setZ(itemElement.attribute(attributes[2]).toFloat());
            }

            if(type == 6)
            {
                if(attributes.size() < 3)
                {
                    qDebug() << "Insufficient number of attributesw";
                    return;
                }

                scene.meshes[index].skew.setX(itemElement.attribute(attributes[0]).toFloat());
                scene.meshes[index].skew.setY(itemElement.attribute(attributes[1]).toFloat());
                scene.meshes[index].skew.setZ(itemElement.attribute(attributes[2]).toFloat());
            }

            if(type == 7)
            {
                if(attributes.size() < 5)
                {
                    qDebug() << "Insufficient number of attributes";
                    return;
                }

                std::vector<float> convertedValue;

                // Camera position
                convertedValue = convertValue(itemElement.attribute(attributes[0]));
                scene.camera.position.setX(convertedValue[0]);
                scene.camera.position.setY(convertedValue[1]);
                scene.camera.position.setZ(convertedValue[2]);

                // Camera target
                convertedValue.clear();
                convertedValue = convertValue(itemElement.attribute(attributes[1]));
                scene.camera.target.setX(convertedValue[0]);
                scene.camera.target.setY(convertedValue[1]);
                scene.camera.target.setZ(convertedValue[2]);

                // Camera up
                convertedValue.clear();
                convertedValue = convertValue(itemElement.attribute(attributes[2]));
                scene.camera.up.setX(convertedValue[0]);
                scene.camera.up.setY(convertedValue[1]);
                scene.camera.up.setZ(convertedValue[2]);

                // Color
                convertedValue.clear();

                // There is no color / texture (choose default)
                if(itemElement.attribute(attributes[3]).isEmpty() && itemElement.attribute(attributes[5]).isEmpty())
                {
                    scene.color.setRed(0);
                    scene.color.setGreen(0);
                    scene.color.setBlue(0);
                    scene.color.setAlpha(255);
                }
                // If there is texture, or there is color and texture, I give the texture
                else if((itemElement.attribute(attributes[3]).isEmpty() && !itemElement.attribute(attributes[5]).isEmpty()) ||
                        (!itemElement.attribute(attributes[3]).isEmpty() && !itemElement.attribute(attributes[5]).isEmpty()))
                {
                    scene.texture = Texture(itemElement.attribute(attributes[5]));
                }
                // Otherwise, I'm reading the color
                else
                {
                    convertedValue = convertValue(itemElement.attribute(attributes[3]));
                    scene.color = Color(convertedValue[0], convertedValue[1], convertedValue[2], convertedValue[3]);
                }

                // Light
                convertedValue.clear();
                convertedValue = convertValue(itemElement.attribute(attributes[4]));
                scene.light.light = QVector3D(convertedValue[0], convertedValue[1], convertedValue[2]);
            }

            if(type == 8)
            {
                if(attributes.size() < 1)
                {
                    qDebug() << "Insufficient number of attributes";
                    return;
                }

                scene.meshes[i].name = itemElement.attribute(attributes[0]);

            }
        }
    }

    attributes.clear();
}

void XMLParser::readXML(QString filename)
{
    QDomDocument document;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open the file";
        return;
    }
    else
    {
        if(!document.setContent(&file))
        {
            qDebug() << "The project couldn't be loaded";
        }
        file.close();
    }

    QDomElement root = document.firstChildElement();
    QDomNodeList items = root.elementsByTagName("mesh");

    scene.meshes.resize(items.count(), Mesh(""));
    std::vector<QString> attributes;
    listElement(root, "mesh", attributes = {"name"}, 8);

    QDomNodeList meshList = root.elementsByTagName("mesh");
    for(int i = 0; i < meshList.count(); i++)
    {
        QDomNode meshNode = meshList.at(i);
        if(meshNode.isElement())
        {
            QDomElement mesh = meshNode.toElement();
            listElement(mesh, "vertice", attributes = {"x", "y", "z", "nx", "ny", "nz", "u", "v"}, 0, i);
            listElement(mesh, "face", attributes = {"A", "B", "C"}, 1, i);
            listElement(mesh, "texture", attributes = {"path", "color", "width", "height"}, 2, i);
            listElement(mesh, "position", attributes = {"x", "y", "z"}, 3, i);
            listElement(mesh, "rotation", attributes = {"x", "y", "z"}, 4, i);
            listElement(mesh, "scaling", attributes = {"x", "y", "z"}, 5, i);
            listElement(mesh, "shearing", attributes = {"x", "y", "z"}, 6, i);
        }
    }

    listElement(root, "scene", attributes = {"camera_position", "camera_target", "camera_up", "color", "light", "texture"}, 7);
}

void XMLParser::readModel(std::vector<Mesh> &meshes, QString filename)
{
    QDomDocument document;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open the file";
        return;
    }
    else
    {
        if(!document.setContent(&file))
        {
            qDebug() << "The project couldn't be loaded";
        }
        file.close();
    }

    QDomElement root = document.firstChildElement();
    QDomNodeList items = root.elementsByTagName("mesh");

    scene.meshes.resize(items.count(), Mesh(""));

    std::vector<QString> attributes;
    listElement(root, "mesh", attributes = {"name"}, 8);

    QDomNodeList meshList = root.elementsByTagName("mesh");
    for(int i = 0; i < meshList.count(); i++)
    {
        QDomNode meshNode = meshList.at(i);
        if(meshNode.isElement())
        {
            QDomElement mesh = meshNode.toElement();
            listElement(mesh, "vertice", attributes = {"x", "y", "z", "nx", "ny", "nz", "u", "v"}, 0, i);
            listElement(mesh, "face", attributes = {"A", "B", "C"}, 1, i);
            listElement(mesh, "texture", attributes = {"path", "color", "width", "height"}, 2, i);
            listElement(mesh, "position", attributes = {"x", "y", "z"}, 3, i);
            listElement(mesh, "rotation", attributes = {"x", "y", "z"}, 4, i);
            listElement(mesh, "scaling", attributes = {"x", "y", "z"}, 5, i);
            listElement(mesh, "shearing", attributes = {"x", "y", "z"}, 6, i);
        }
    }

    for(uint i = 0; i < scene.meshes.size(); i++)
    {
        if(meshes.size() > 0)
        {
            scene.meshes[i].name = scene.meshes[i].name + QString::number(scene.meshes.back().id + 1);;
            scene.meshes[i].id = scene.meshes.back().id + 1;
        }

        meshes.push_back(scene.meshes[i]);
    }
}

bool XMLParser::is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

Scene XMLParser::getScene()
{
    return scene;
}

void XMLParser::clearBuffor()
{
    if(scene.meshes.size() > 0)
        scene.meshes.clear();
}
