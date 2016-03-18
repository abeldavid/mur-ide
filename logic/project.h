#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QHash>

class Project : public QObject
{
    Q_OBJECT
public:
    static const QString defaultSourceName;
    static const QString defaultSource;
    static const QString defaultHeaderName;
    static const QString defaultHeader;
    static const QString defaultProjectPrefix;
    static const QString multiFileSeparator;
    static const QString sourceFileExtension;
    static const QString headerFileExtension;
    static const QHash<QString, QString> defaultFilePrefixes;

    QString m_projectsRoot;
    QDir m_projectDir;
    explicit Project(QObject *parent = 0);
    ~Project();
    bool getIsOpened();
    bool create(const QString &name, const QString &path);
    bool createFile(const QString &name);
    bool addExistingFile(const QString &path);
    QString getDefaultProjectName();
    QString getDefaultFileName(const QString &extension);
signals:

public slots:

private:
    bool isOpened = false;
    bool addDefaultFile(const QString &pathName, const QString &content = "");
    bool addEmptyFile(const QString &name);
    int getFileNameAutoIncrement(QStringList &fileList,
                                 const QString &prefix,
                                 const QString &postfix = "");
};

#endif // PROJECT_H
