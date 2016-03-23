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

    enum  class TARGET : int {
        MINGW = 10,
        EDISON = 20
    };

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
    void setMingwCompilerPath(const QString &option) const;

    QStringList mingwCompilerOptions() const;
    void setMingwCompilerOptions(const QStringList &options) const;

    QStringList mingwLibsPaths() const;
    void setMingwLibsPaths(const QStringList &options) const;

    QStringList mingwIncludesPath() const;
    void setMingwIncludesPath(const QStringList &options);

    QString mingwBinarysPath() const;
    void setMingwBinarysPath(const QString &option);

    void setCurrentTarget(TARGET target);
    TARGET currentTarget();

signals:

public slots:
private:
    explicit SettingsManager(QObject *parent = 0);
    SettingsManager(SettingsManager const&) = delete;
    void operator = (SettingsManager const&) = delete;
    QSettings *m_settings;
    TARGET m_target = TARGET::EDISON;
};

#define SETTINGS SettingsManager::instance()

#endif // SETTINGSMANAGER_H
