#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QtXml>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <vector>
#include <QStringList>
#include <QDebug>
#include <string>
#include <sstream>

#include <mesh.h>
#include <scene.h>

class XMLParser
{
public:
    XMLParser();

    void writeXML(std::vector<Mesh> meshes, Scene scene, QString filename);
    void readXML(QString filename);
    void readModel(std::vector<Mesh> &meshes, QString filename);
    void clearBuffor();
    bool is_number(const std::string& s);
    std::vector<Mesh> getMeshes();
    Scene getScene();

private:
    std::vector <Mesh> meshes;
    Scene scene;

    void listElement(QDomElement root, QString tagname, std::vector<QString> &attributes, ushort type, int index);
    std::vector<float> convertValue(QString str);
};

#endif // XMLPARSER_H
