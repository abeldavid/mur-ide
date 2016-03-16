#include "projectmanager.h"

ProjectManager::ProjectManager(QObject *parent) :
    QObject(parent),
    m_project(new Project(this))
{

}

ProjectManager &ProjectManager::instance()
{
    static ProjectManager instance;
    return instance;
}

ProjectManager::~ProjectManager()
{
    delete m_project;
}

QString ProjectManager::projectsRoot() const {
    return m_project->m_projectsRoot;
}

QString ProjectManager::defaultNewProjectName() const {
    return m_project->getDefaultProjectName();
}

void ProjectManager::createProject(const QString &name, const QString &path) {
    if (m_project->create(name, path)) {
        emit projectCreated();
    } else {
        emit projectCreateFailed();
    }
}


