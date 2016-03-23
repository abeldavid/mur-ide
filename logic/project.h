#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QJsonObject>

class Project : public QObject
{
    Q_OBJECT
public:
    static const QString projectFileName;
    static const QString projectsRootPath;
    static const QString defaultSourceName;
    static const QString defaultSourceContent;
    static const QString defaultHeaderName;
    static const QString defaultHeaderContent;
    static const QString defaultProjectPrefix;
    static const QString multiFileSeparator;
    static const QString sourceFileExtension;
    static const QString headerFileExtension;
    static const QHash<QString, QString> defaultFilePrefixes;
    static const QString sourcesSection;
    static const QString headersSection;
    static const QString availableFileExtensions;
    static const QString buildFileName;

    QString m_projectsRoot;
    QDir m_projectDir;
    explicit Project(QObject *parent = 0);
    ~Project();
    bool getIsOpened();
    bool create(const QString &name, const QString &path);
    bool open(const QString &path);
    bool close();
    bool createFile(const QString &name);
    bool addExistingFile(const QString &path);
    QString getDefaultProjectName();
    QString getDefaultFileName(const QString &extension);
    bool openFile(const QString &fileName, QString &content);
    bool saveFile(const QString &name, const QString &content);
    bool renameFile(const QString &oldName, const QString &newName);
    bool deleteFile(const QString &name);
    bool generateMakeFile(const QString &compilerPath, const QString sysrootPath, const QString options);
    QString getName();
    QStringList getSources();
    QStringList getHeaders();

signals:

private:
    QJsonObject m_projectJson;
    bool m_isOpened = false;
    bool writeFile(const QString &name, const QString &content = "");
    int getFileNameAutoIncrement(QStringList &fileList,
                                 const QString &prefix,
                                 const QString &postfix = "");
    bool createProjectFile(const QString &name);
    bool addFile(const QString &name);
    QString getSectionName(const QString &fileName);
};

#endif // PROJECT_H
