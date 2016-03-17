#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QDir>
#include <QString>

class Project : public QObject
{
    Q_OBJECT
public:
    static const QString defaultSourceName;
    static const QString defaultSource;
    static const QString defaultHeaderName;
    static const QString defaultHeader;
    static const QString defaultProjectPrefix;

    QString m_projectsRoot;
    QDir m_projectDir;
    explicit Project(QObject *parent = 0);
    ~Project();
    bool create(const QString &name, const QString &path);
    QString getDefaultProjectName();
    QString getDefaultFileName();
signals:

public slots:

private:
    bool addDefaultFile(const QString &name, const QString &content = "");
};

#endif // PROJECT_H
