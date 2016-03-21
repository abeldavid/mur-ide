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
    QString pathToFile(const QString &fileName) const;
    QString defaultNewProjectName() const;
    QString defaultOpenFilePath() const;
    QString defaultNewFileName(const QString &extension) const;

signals:
    void projectCreated(QString path);
    void projectOpened(QString path);
    void projectClosed();
//    void projectCreateFailed();
    void fileCreated(QString name);
    void fileAdded();
    void fileOpened(QString name, QString content);
    void fileSaved(QString name);

public slots:
    void createProject(const QString &path, const QString &name);
    void createFile(const QString &name);
    void addExistingFile(const QString &path);
    void openProject(const QString &path);
    void closeProject();
    void openFile(const QString &name);
    void saveFile(const QString &name, const QString &content);

private:
    explicit ProjectManager(QObject *parent = 0);
    virtual ~ProjectManager();
    ProjectManager(ProjectManager const&) = delete;
    void operator= (ProjectManager const&) = delete;

    Project *m_project;
};

#endif // PROJECTMANAGER_H
