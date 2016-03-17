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

    QString projectsRoot() const;
    QString defaultNewProjectName() const;
    QString defaultOpenFileName() const;
    QString defaultNewFileName() const;
//    Project* project() const;

signals:
    void projectCreated(QString path);
    void projectCreateFailed();

public slots:
    void createProject(const QString &path, const QString &name);

private:

    explicit ProjectManager(QObject *parent = 0);
    virtual ~ProjectManager();
    ProjectManager(ProjectManager const&) = delete;
    void operator= (ProjectManager const&) = delete;

    Project *m_project;
};

#endif // PROJECTMANAGER_H
