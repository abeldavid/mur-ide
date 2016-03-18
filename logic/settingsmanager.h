#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QPoint>
#include <QSize>
#include <QByteArray>

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    static SettingsManager& instance();
    ~SettingsManager();

    QByteArray mainWindowState() const;
    void setMainWindowState(const QByteArray &state);

    QString plinkPath() const;
    void setPlinkPath(const QString &path);

    QString pscpPath() const;
    void setPscpPath(const QString &path);

    QString edisonSysrootPath() const;
    void setSysrootPath(const QString &path);

    QString edisonCompilerPath() const;
    void setCompilerPath(const QString &path);

    QString mingwMakePath() const;
    void setMingwMakePath(const QString &path);

    QString ipAddress() const;
    void setIpAddress(const QString &ip);

    QString userLogin() const;
    void setUserLogin(const QString &login);

    QString userPassword() const;
    void setUserPassword(const QString &password) const;

    QStringList edisonCompilerOptions() const;
    void setEdisonCompilerOptions(const QStringList &options) const;

    QString mingwCompilerPath() const;
    void setMingwCompilerPath(const QString &options) const;

    QStringList mingwCompilerOptions() const;
    void setMingwCompilerOptions(const QStringList &options) const;

    QString mingwLibsPaths() const;
    void setMingwLibsPaths(const QStringList &options) const;

    QStringList mingwIncludesPath() const;
    void setMingwIncludesPath(const QStringList &options) const;


signals:

public slots:
private:
    explicit SettingsManager(QObject *parent = 0);
    SettingsManager(SettingsManager const&) = delete;
    void operator = (SettingsManager const&) = delete;
    QSettings *m_settings;
};

#endif // SETTINGSMANAGER_H
