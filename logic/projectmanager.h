/*
It is a singleton which manages project.h and emits signals for interface widgets (it has slots also)
*/

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "project.h"
#include <QString>
#include <QStringList>

class ProjectManager : public QObject
{
    Q_OBJECT
public:
    static ProjectManager& instance();

    bool isProjectOpened() const;
    bool fileExists(const QString &fileName) const;
    QString getProjectPath() const;
    QString projectsRoot() const;
    QString getProjectName() const;
    QString pathToFile(const QString &fileName) const;
    QString defaultNewProjectName() const;
    QString defaultOpenFilePath() const;
    QString defaultNewFileName(const QString &extension) const;
    QStringList getSourceFiles() const;
    QStringList getHeaderFiles() const;

signals:
    void projectCreated(QString path);
    void projectOpened(QString path);
    void projectClosed();
//    void projectCreateFailed();
    void fileCreated(QString name);
    void fileAdded();
    void fileOpened(QString name, QString content);
    void fileSaved(QString name);
    void fileDeleted(QString name);
    void fileRenamed();
    void makeFileGenerated();

public slots:
    void createProject(const QString &path, const QString &name);
    void createFile(const QString &name);
    void addExistingFile(const QString &path);
    void openProject(const QString &path);
    void closeProject();
    void openFile(const QString &name);
    void saveFile(const QString &name, const QString &content);
    void renameFile(const QString &oldName, const QString &newName);
    void deleteFile(const QString &name);
    void generateMakeFile(const QString &compilerPath, const QString sysrootPath, const QString options);

private:
    explicit ProjectManager(QObject *parent = 0);
    virtual ~ProjectManager();
    ProjectManager(ProjectManager const&) = delete;
    void operator= (ProjectManager const&) = delete;

    Project *m_project;
};

#endif // PROJECTMANAGER_H
