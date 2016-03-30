#include "settingsmanager.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

SettingsManager &SettingsManager::instance()
{
    static SettingsManager instance;
    return instance;
}

SettingsManager::~SettingsManager()
{
    delete m_settings;
}

QByteArray SettingsManager::mainWindowState() const
{
    m_settings->beginGroup("MainWindowState");
    QByteArray state = m_settings->value("State", 0).toByteArray();
    m_settings->endGroup();
    return state;
}

void SettingsManager::setMainWindowState(const QByteArray &state)
{
    m_settings->beginGroup("MainWindowState");
    m_settings->setValue("State", state);
    m_settings->endGroup();
}

QString SettingsManager::plinkPath() const
{
    m_settings->beginGroup("wi-fi-connection-settings");
    QString plink = m_settings->value("Plink",
                                    QDir::cleanPath(QCoreApplication::applicationDirPath() +
                                        "/tools/plink.exe")).toString();

    m_settings->endGroup();
    return plink;
}

void SettingsManager::setPlinkPath(const QString &path)
{
    m_settings->beginGroup("wi-fi-connection-settings");
    m_settings->setValue("Plink", path);
    m_settings->endGroup();
}

QString SettingsManager::pscpPath() const
{
    m_settings->beginGroup("wi-fi-connection-settings");
    QString pscp = m_settings->value("Pscp",
                                QDir::cleanPath(QCoreApplication::applicationDirPath() +
                                    "/tools/pscp.exe")).toString();
    m_settings->endGroup();
    return pscp;
}

void SettingsManager::setPscpPath(const QString &path)
{
    m_settings->beginGroup("wi-fi-connection-settings");
    m_settings->setValue("Pscp", path);
    m_settings->endGroup();
}

QString SettingsManager::edisonSysrootPath() const
{
    m_settings->beginGroup("i586-poky-linux");
    QString sysroot = m_settings->value("Sysroot",
                                QDir::cleanPath(QCoreApplication::applicationDirPath() +
                                    "/devkit-x86/sysroots/i586-poky-linux")).toString();
    m_settings->endGroup();
    return sysroot;
}

void SettingsManager::setSysrootPath(const QString &path)
{
    m_settings->beginGroup("i586-poky-linux");
    m_settings->setValue("Sysroot", path);
    m_settings->endGroup();
}

QString SettingsManager::edisonCompilerPath() const
{
    m_settings->beginGroup("i586-poky-linux");
    QString compiler = m_settings->value("CC",
                                QDir::cleanPath(QCoreApplication::applicationDirPath() +
                                    "/devkit-x86/sysroots/x86_64-pokysdk-mingw32/usr/bin/i586-poky-linux/i586-poky-linux-g++.exe")).toString();
    m_settings->endGroup();
    return compiler;
}

void SettingsManager::setCompilerPath(const QString &path)
{
    m_settings->beginGroup("i586-poky-linux");
    m_settings->setValue("CC", path);
    m_settings->endGroup();
}

QString SettingsManager::mingwMakePath() const
{
    m_settings->beginGroup("i586-poky-linux");
    QString make = m_settings->value("mingwMake",
                                     QDir::cleanPath(QCoreApplication::applicationDirPath() +
                                        "/devkit-x86/mingw492_32/bin/mingw32-make.exe")).toString();
    m_settings->endGroup();
    return make;
}

void SettingsManager::setMingwMakePath(const QString &path)
{
    m_settings->beginGroup("i586-poky-linux");
    m_settings->setValue("mingwMake", path);
    m_settings->endGroup();
}

QString SettingsManager::ipAddress() const
{
    m_settings->beginGroup("IpLoginPassword");
    QString ip = m_settings->value("IP", "192.168.42.1").toString();
    m_settings->endGroup();
    return ip;
}

void SettingsManager::setIpAddress(const QString &ip)
{
    m_settings->beginGroup("IpLoginPassword");
    m_settings->setValue("IP", ip);
    m_settings->endGroup();
}

QString SettingsManager::userLogin() const
{
    m_settings->beginGroup("IpLoginPassword");
    QString login = m_settings->value("Login", "root").toString();
    m_settings->endGroup();
    return login;
}

void SettingsManager::setUserLogin(const QString &login)
{
    m_settings->beginGroup("IpLoginPassword");
    m_settings->setValue("Login", login);
    m_settings->endGroup();
}

QString SettingsManager::userPassword() const
{
    m_settings->beginGroup("IpLoginPassword");
    QString password = m_settings->value("Password", "2030405060").toString();
    m_settings->endGroup();
    return password;
}

void SettingsManager::setUserPassword(const QString &password) const
{
    m_settings->beginGroup("IpLoginPassword");
    m_settings->setValue("Password", password);
    m_settings->endGroup();
}

QStringList SettingsManager::edisonCompilerOptions() const
{
    m_settings->beginGroup("i586-poky-linux");
    QStringList defaultOptions;
    defaultOptions << "-O2";
    defaultOptions << "-std=c++11";
    defaultOptions << "-lmur";
    defaultOptions << "-lopencv_core";
    defaultOptions << "-lopencv_highgui";
    defaultOptions << "-lopencv_imgproc";
    defaultOptions << "-lopencv_features2d";
    defaultOptions << "-lopencv_flann";
    defaultOptions << "-lopencv_objdetect";
    defaultOptions << "-lopencv_videostab";
    defaultOptions << "-lzmq";

    QStringList compilerOpt = m_settings->value("CCOPT", defaultOptions).toStringList();
    m_settings->endGroup();
    return compilerOpt;
}

void SettingsManager::setEdisonCompilerOptions(const QStringList &options) const
{
    m_settings->beginGroup("i586-poky-linux");
    m_settings->setValue("CCOPT", options);
    m_settings->endGroup();
}

QString SettingsManager::mingwCompilerPath() const
{
    QString pathToCC = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/devkit-x86/mingw492_32/bin/g++.exe");
    m_settings->beginGroup("mingw-windows");
    QString cc = m_settings->value("MINGWCC", pathToCC).toString();
    m_settings->endGroup();
    return cc;
}

void SettingsManager::setMingwCompilerPath(const QString &option) const
{
    m_settings->beginGroup("mingw-windows");
    m_settings->setValue("MINGWCC", option);
    m_settings->endGroup();
}

QStringList SettingsManager::mingwCompilerOptions() const
{
    QStringList defaultOptions;
    defaultOptions << "-O2";
    defaultOptions << "-std=c++11";
    defaultOptions << "-lmur";
    defaultOptions << "-lopencv_core2410";
    defaultOptions << "-lopencv_highgui2410";
    defaultOptions << "-lopencv_imgproc2410";
    defaultOptions << "-lopencv_features2d2410";
    defaultOptions << "-lopencv_flann2410";
    defaultOptions << "-lopencv_objdetect2410";
    defaultOptions << "-lopencv_videostab2410";
    defaultOptions << "-lzmq";

    m_settings->beginGroup("mingw-windows");
    QStringList compilerOpt = m_settings->value("MINGWCCOPT", defaultOptions).toStringList();
    m_settings->endGroup();
    return compilerOpt;
}

void SettingsManager::setMingwCompilerOptions(const QStringList &options) const
{
    m_settings->beginGroup("mingw-windows");
    m_settings->setValue("MINGWCCOPT", options);
    m_settings->endGroup();
}

QStringList SettingsManager::mingwLibsPaths() const
{
    QStringList pathToLibs;
    pathToLibs << "-L" + QDir::cleanPath(QCoreApplication::applicationDirPath() + "/devkit-x86/murlibs");

    m_settings->beginGroup("mingw-windows");
    QStringList libs = m_settings->value("MINGWCCLIBS", pathToLibs).toStringList();
    m_settings->endGroup();
    return libs;
}

void SettingsManager::setMingwLibsPaths(const QStringList &options) const
{
    m_settings->beginGroup("mingw-windows");
    m_settings->setValue("MINGWCCLIBS", options);
    m_settings->endGroup();
}

QStringList SettingsManager::mingwIncludesPath() const
{
    QStringList pathToIncludes;
    pathToIncludes << "-I" + QDir::cleanPath(QCoreApplication::applicationDirPath() + "/devkit-x86/murlibs");

    m_settings->beginGroup("mingw-windows");
    QStringList includes = m_settings->value("MINGWCCINCLUDES", pathToIncludes).toStringList();
    m_settings->endGroup();
    return includes;
}

void SettingsManager::setMingwIncludesPath(const QStringList &options)
{
    m_settings->beginGroup("mingw-windows");
    m_settings->setValue("MINGWCCINCLUDES", options);
    m_settings->endGroup();
}

QString SettingsManager::mingwBinarysPath() const
{
    QString pathToBinarys = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/devkit-x86/murlibs");

    m_settings->beginGroup("mingw-windows");
    QString bins = m_settings->value("MINGWCCBINS", pathToBinarys).toString();
    m_settings->endGroup();
    return bins;
}

void SettingsManager::setMingwBinarysPath(const QString &option)
{
    m_settings->beginGroup("mingw-windows");
    m_settings->setValue("MINGWCCBINS", option);
    m_settings->endGroup();
}

void SettingsManager::setCurrentTarget(SettingsManager::TARGET target)
{
    m_target = target;
}

SettingsManager::TARGET SettingsManager::currentTarget()
{
    return m_target;
}

SettingsManager::SettingsManager(QObject *parent) :
    QObject(parent),
    m_settings(new QSettings("roboide_settings.ini", QSettings::IniFormat))

{

}

