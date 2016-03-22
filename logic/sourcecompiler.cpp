#include "sourcecompiler.h"
#include "settingsmanager.h"
#include "projectmanager.h"

#include <QDebug>
#include <QFileInfo>
#include <QRunnable>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QDateTime>
#include <thread>
#include <chrono>

SourceCompiler::SourceCompiler(QObject *parent) :
    QObject(parent),
    m_compilationThread(new QThread(this)),
    m_processRunner(new ProcessRunner),
    m_isCompiled(false),
    m_target(TARGET::EDISON)
{
    m_processRunner->moveToThread(m_compilationThread);
    m_compilationThread->start();
    QObject::connect(m_processRunner, SIGNAL(processOutputReady(QString)), this, SIGNAL(onCompilationOutput(QString)));
    QObject::connect(m_processRunner, SIGNAL(finished(int)), this, SLOT(onCompilationFinished(int)));
    QObject::connect(this, SIGNAL(run()), m_processRunner, SLOT(run()));
}

SourceCompiler::~SourceCompiler()
{
    m_compilationThread->terminate();
}

QString SourceCompiler::pathToBinary() const
{
    return m_pathToBinary;
}

bool SourceCompiler::isCompiled() const
{
    return m_isCompiled;
}

void SourceCompiler::setTarget(SourceCompiler::TARGET target)
{
    m_target = target;
}

void SourceCompiler::onRunCompilation(QString src)
{   
    if (m_target == TARGET::EDISON) {
        QString pathToSysRoot = SettingsManager::instance().edisonSysrootPath();
        QString pathToCC = SettingsManager::instance().edisonCompilerPath();
        QStringList options = SettingsManager::instance().edisonCompilerOptions();
        QString binaryName = src + ".bin ";

        QStringList processArgs;

        processArgs << "--sysroot=" + pathToSysRoot;
        processArgs << src;
        processArgs << "-o" + binaryName;
        processArgs << options;

        m_pathToBinary = src + ".bin";
        qDebug() << processArgs;
        m_processRunner->setArguments(processArgs);
        m_processRunner->setPath(pathToCC);
    }

    if (m_target == TARGET::MINGW) {
        QStringList pathToIncludes = SettingsManager::instance().mingwIncludesPath();
        QStringList pathToLibs = SettingsManager::instance().mingwLibsPaths();
        QString pathToCC = SettingsManager::instance().mingwCompilerPath();
        QStringList options = SettingsManager::instance().mingwCompilerOptions();
        QString binaryName = src + ".exe";

        QStringList processArgs;

        processArgs << pathToIncludes;
        processArgs << pathToLibs;
        processArgs << src;
        processArgs << "-o" + binaryName;
        processArgs << options;

        m_pathToBinary = src + ".exe";
        qDebug() << processArgs;
        m_processRunner->setArguments(processArgs);
        m_processRunner->setPath(pathToCC);
    }

    emit run();
}

void SourceCompiler::onCompilationFinished(int retCode)
{
    qDebug() << retCode;
    if (retCode != 0) {
        m_pathToBinary = "";
        m_isCompiled = false;
        onCompilationOutput(QString("Ошибка компиляции " + QDateTime::currentDateTime().toString()));
    }
    m_isCompiled = true;
    if (retCode == 0) {
        onCompilationOutput(QString("Компиляция прошла успешно " + QDateTime::currentDateTime().toString()));
    }
    emit finished();
}


