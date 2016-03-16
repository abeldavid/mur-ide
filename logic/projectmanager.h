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
    Project* project() const;

signals:
    void projectCreated();
    void projectCreateFailed();

public slots:
    void createProject(const QString &name, const QString &path);

private:

    explicit ProjectManager(QObject *parent = 0);
    virtual ~ProjectManager();
    ProjectManager(ProjectManager const&) = delete;
    void operator= (ProjectManager const&) = delete;

    Project *m_project;
};

#endif // PROJECTMANAGER_H
