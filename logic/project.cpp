#include "project.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QStringList>
#include <QRegExp>

const QString Project::defaultSourceName = "main.cpp";
const QString Project::defaultSource = "#include \"main.h\"\n\n"
                                        "void main(){}";
const QString Project::defaultHeaderName = "main.h";
const QString Project::defaultHeader = "#define HELLO \"Hello World!\" ";
const QString Project::defaultProjectPrefix = "Project_";

Project::Project(QObject *parent) : QObject(parent)
{
    m_projectsRoot = QStandardPaths::locate(QStandardPaths::DocumentsLocation,
                                                  "MURProjects",
                                                  QStandardPaths::LocateDirectory);
    if (m_projectsRoot.isEmpty()) {
        m_projectsRoot = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                + QDir::separator() + QString("MURProjects");
        QDir().mkdir(m_projectsRoot);
    }
}

Project::~Project(){
//    delete *m_projectDir;
}

bool Project::create(const QString &name, const QString &path){
    bool result;
    QDir projectDir(path);
    if (projectDir.exists() and
            projectDir.mkdir(name) and
            projectDir.cd(name) and
            this->addDefaultFile(Project::defaultSourceName,
                                 Project::defaultSource) and
            this->addDefaultFile(Project::defaultHeaderName,
                                 Project::defaultHeader)) {
        m_projectDir = projectDir;
        result = true;
    } else {
        result = false;
    }
    return result;
}

QString Project::getDefaultProjectName(){
    QDir dir(m_projectsRoot);
    QStringList filters;
    filters << Project::defaultProjectPrefix + QString("*");
    QStringList dirsInRoot = dir.entryList(filters, QDir::Dirs, QDir::Name);
    int maxProjectIndex = 0;
    if (dirsInRoot.size() > 0 ) {
        QRegExp rx(Project::defaultProjectPrefix + "\\d+");
        int lastProjectINdex = dirsInRoot.lastIndexOf(rx);
        if (lastProjectINdex != -1) {
            maxProjectIndex = dirsInRoot.at(lastProjectINdex).split(Project::defaultProjectPrefix)[1].toInt();
        }
    }
    return Project::defaultProjectPrefix + QString::number(maxProjectIndex + 1);
}

bool Project::addDefaultFile(const QString &name, const QString &content){
    QFile file(name);
    bool result = file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << content;
    file.close();
    return result;
}

