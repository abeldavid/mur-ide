/*
It is a singleton which manages project.h and emits signals for interface widgets (it has slots also)
*/

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "project.h"
#include <QString>

class ProjectManager : public QObject
{
    Q_OBJECT
public:
    static ProjectManager& instance();

    bool projectOpened() const;
    QString getProjectPath() const;
    QString projectsRoot() const;
    QString defaultNewProjectName() const;
    QString defaultOpenFilePath() const;
    QString defaultNewFileName(const QString &extension) const;
//    Project* project() const;

signals:
    void projectCreated(QString path);
    void projectOpened(QString path);
//    void projectCreateFailed();
    void fileCreated(QString name);
    void fileAdded();

public slots:
    void createProject(const QString &path, const QString &name);
    void createFile(const QString &name);
    void addExistingFile(const QString &path);
    void openProject(const QString &path);

private:
    explicit ProjectManager(QObject *parent = 0);
    virtual ~ProjectManager();
    ProjectManager(ProjectManager const&) = delete;
    void operator= (ProjectManager const&) = delete;

    Project *m_project;
};

#endif // PROJECTMANAGER_H
