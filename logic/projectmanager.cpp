#include "projectmanager.h"
#include <QDebug>

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

QString ProjectManager::projectsRoot() const
{
    return m_project->m_projectsRoot;
}

QString ProjectManager::getProjectName() const
{
    return m_project->getName();
}

QString ProjectManager::pathToFile(const QString &fileName) const
{
    return m_project->m_projectDir.filePath(fileName);
}

QString ProjectManager::defaultNewFileName(const QString &extension) const
{
    return m_project->getDefaultFileName(extension);
}

QStringList ProjectManager::getSourceFiles() const
{
    return m_project->getSources();
}

QStringList ProjectManager::getHeaderFiles() const
{
    return m_project->getHeaders();
}

QString ProjectManager::defaultOpenFilePath() const
{
    return m_project->Project::defaultSourceName;
}

QString ProjectManager::defaultNewProjectName() const
{
    return m_project->getDefaultProjectName();
}

bool ProjectManager::isProjectOpened() const
{
    return m_project->getIsOpened();
}

bool ProjectManager::fileExists(const QString &fileName) const
{
    return m_project->m_projectDir.exists(fileName);
}

QString ProjectManager::getProjectPath() const
{
    return m_project->m_projectDir.absolutePath();
}

void ProjectManager::createProject(const QString &name, const QString &path)
{
    if (m_project->create(name, path)) {
        emit projectCreated(m_project->m_projectDir.absolutePath());
    }
//    else {
//        emit projectCreateFailed();
//    }
}

void ProjectManager::createFile(const QString &name)
{
    if (m_project->createFile(name)) {
        emit fileCreated(name);
    }
}

void ProjectManager::addExistingFile(const QString &path)
{
    if (m_project->addExistingFile(path)) {
        emit fileAdded();
    }
}

void ProjectManager::openProject(const QString &path)
{
    if (m_project->open(path)) {
        emit projectOpened(m_project->m_projectDir.absolutePath());
    }
}

void ProjectManager::closeProject()
{
    if (m_project->close()) {
        emit projectClosed();
    }
}

void ProjectManager::openFile(const QString &name)
{
    QString content;
    if (m_project->openFile(name, content)) {
        emit fileOpened(name, content);
    }
}

void ProjectManager::saveFile(const QString &name, const QString &content)
{
    if (m_project->saveFile(name, content)) {
        emit fileSaved(name);
    }
}

void ProjectManager::renameFile(const QString &oldName, const QString &newName)
{
    if (m_project->renameFile(oldName, newName)) {
        emit fileRenamed();
    }
}

void ProjectManager::deleteFile(const QString &name)
{
    if (m_project->deleteFile(name)) {
        emit fileDeleted(name);
    }
}

void ProjectManager::generateMakeFile(const QString &compilerPath, const QString sysrootPath, const QString options)
{
    if (m_project->generateMakeFile(compilerPath, sysrootPath, options)) {
        emit fileAdded();
    }
}


