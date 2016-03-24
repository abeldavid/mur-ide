#include "project.h"
#include "settingsmanager.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QCollator>
#include <algorithm>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>

const QString Project::projectFileName = ".roboproject";
const QString Project::projectsRootPath = "MURProjects";
const QString Project::sourceFileExtension = ".cpp";
const QString Project::headerFileExtension = ".h";
const QString Project::defaultSourceName = "main" + Project::sourceFileExtension;
const QString Project::defaultHeaderName = "main" + Project::headerFileExtension;
const QString Project::defaultSourceContent = "#include \"" + Project::defaultHeaderName + "\"\n"
                                              "#include <iostream>\n\n"
<<<<<<< HEAD
                                              "int main () {\n    std::cout << \"Hello world\" << std::endl;\n}";
=======
                                              "int main () {\n    std::cout << \"Hello world\" << std::end;\n}";
>>>>>>> f064550e440cdad97c4fb1145af9974b57e71705
const QString Project::defaultHeaderContent = "#pragma once";
const QString Project::defaultProjectPrefix = "Project_";
const QString Project::multiFileSeparator = " + ";
const QHash<QString, QString> Project::defaultFilePrefixes({
        {Project::sourceFileExtension, QString("source_")},
        {Project::headerFileExtension, QString("header_")},
        {Project::sourceFileExtension + Project::multiFileSeparator + Project::headerFileExtension,
            QString("module_")
        }
    });
const QString Project::sourcesSection = "sources";
const QString Project::headersSection = "headers";
const QString Project::availableFileExtensions =
        "C++ (*" + Project::sourceFileExtension +
        " *" + Project::headerFileExtension + ")";
const QString Project::buildFileName = "makefile";

Project::Project(QObject *parent) :
    QObject(parent),
    m_isOpened(false)
{
    m_projectsRoot = QStandardPaths::locate(QStandardPaths::DocumentsLocation,
                                            Project::projectsRootPath,
                                            QStandardPaths::LocateDirectory);
    if (m_projectsRoot.isEmpty()) {
        m_projectsRoot = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).
                filePath(Project::projectsRootPath);
        QDir().mkdir(m_projectsRoot);
    }
}

Project::~Project()
{
//    delete *m_projectDir;
}

bool Project::create(const QString &path, const QString &name)
{
    bool result = false;
    QDir projectDir(path); //here it is project parent dir, later it cds to project dir
    if (projectDir.exists() and projectDir.mkdir(name) and projectDir.cd(name)) {
        m_projectDir = projectDir;
        if (writeFile(Project::defaultSourceName, Project::defaultSourceContent) and
                writeFile(Project::defaultHeaderName, Project::defaultHeaderContent) and
                createProjectFile(name)) {
            result = true;
            m_isOpened = true;
        }
    }
    return result;
}

bool Project::open(const QString &path)
{
    if (m_isOpened) {
        close();
    }
    bool result = false;
    QFile projectFile(path);
    if (projectFile.open(QIODevice::ReadOnly)) {
        QByteArray projectStructure = projectFile.readAll();
        projectFile.close();
        QJsonParseError err;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(projectStructure, &err);
        if (err.error == QJsonParseError::NoError) {
            m_projectJson = jsonDoc.object();
            QFileInfo projectFileInfo(path);
            m_projectDir = projectFileInfo.dir();
            m_isOpened = true;
            result = true;
        }
    }
    return result;
}

bool Project::close()
{
    bool result = m_isOpened;
    m_isOpened = false;
    return result;
}

bool Project::createFile(const QString &name)
{
    bool result = m_projectDir.exists() and writeFile(name);
    if (result) {
        result = addFile(name);
    }
    return result;
}

bool Project::addExistingFile(const QString &path)
{
    QFileInfo sourceFileInfo(path);
    int fileNameIncrement = 1;
    QString targetFileName = m_projectDir.filePath(sourceFileInfo.fileName());
    QFileInfo initTargetFileInfo(targetFileName);
    QFileInfo targetFileInfo = initTargetFileInfo;
    while (targetFileInfo.exists() and targetFileInfo.isFile()) {
        targetFileName = m_projectDir.filePath(
                    initTargetFileInfo.completeBaseName() +
                    QString("_") +
                    QString::number(fileNameIncrement++) +
                    QString(".") +
                    initTargetFileInfo.suffix()
                    );
        targetFileInfo.setFile(targetFileName);
    }
    bool success = false;
    if (QFile::copy(path, targetFileName)) {
        success = addFile(targetFileInfo.fileName());
    }
    return success;
}

QString Project::getDefaultProjectName()
{
    QDir dir(m_projectsRoot);
    QString projectPrefix = Project::defaultProjectPrefix;
    QStringList filters;
    filters << projectPrefix + QString("*");
    QStringList dirsInRoot = dir.entryList(filters, QDir::Dirs, QDir::NoSort);
    int newProjectNumber = getFileNameAutoIncrement(dirsInRoot, projectPrefix);
    return projectPrefix + QString::number(newProjectNumber);
}

QString Project::getDefaultFileName(const QString &extension)
{
    QDir dir(m_projectDir);
    QString filePrefix = Project::defaultFilePrefixes[extension];
    QStringList filters;
    filters << filePrefix + QString("*");
    QStringList filesInProject = dir.entryList(filters, QDir::Files, QDir::NoSort);
    QStringList extensionList = extension.split(Project::multiFileSeparator);
    int newFileNumber = 0;
    for (QString fileExtension: extensionList) {
        int currFileNumber = getFileNameAutoIncrement(filesInProject, filePrefix, fileExtension);
        newFileNumber = std::max(newFileNumber, currFileNumber);
    }
    return filePrefix + QString::number(newFileNumber);
}

// content is non-const reference, because it is return value
bool Project::openFile(const QString &fileName, QString &content)
{
    QFile file(m_projectDir.filePath(fileName));
    bool result = false;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        content = in.readAll();
        file.close();
        result = true;
    }
    return result;
}

// acessor to writeFile, for possible future modification
bool Project::saveFile(const QString &name, const QString &content)
{
    return writeFile(name, content);
}

bool Project::renameFile(const QString &oldName, const QString &newName)
{
    bool result = false;
    QString fileType = getSectionName(oldName);
    if(!fileType.isEmpty()) {
        QJsonArray projectSection = m_projectJson[fileType].toArray();
        if (projectSection.contains(oldName)) {
            int i;
            for (i = 0; i < projectSection.size(); i++) {
                if (projectSection.at(i) == oldName) {
                    break;
                }
            }
            projectSection.replace(i, newName);
            m_projectJson[fileType] = projectSection;
            QJsonDocument jsonDoc(m_projectJson);
            result = m_projectDir.rename(oldName, newName) and
                    writeFile(Project::projectFileName, jsonDoc.toJson());
        }
    }
    return result;
}

bool Project::deleteFile(const QString &name)
{
    bool result = false;
    QString fileType = getSectionName(name);
    if(!fileType.isEmpty()) {
        QJsonArray projectSection = m_projectJson[fileType].toArray();
        if (projectSection.contains(name)) {
            int i;
            for (i = 0; i < projectSection.size(); i++) {
                if (projectSection.at(i) == name) {
                    break;
                }
            }
            projectSection.removeAt(i);
            m_projectJson[fileType] = projectSection;
            QJsonDocument jsonDoc(m_projectJson);
            result = m_projectDir.remove(name) and
                    writeFile(Project::projectFileName, jsonDoc.toJson());
        }
    }
    return result;
}

bool Project::addFile(const QString &name)
{
    QString fileType = getSectionName(name);
    bool result = false;
    if(!fileType.isEmpty()) {
        QJsonArray projectSection = m_projectJson[fileType].toArray();
        projectSection.append(name);
        m_projectJson[fileType] = projectSection;
        QJsonDocument jsonDoc(m_projectJson);
        result = writeFile(Project::projectFileName, jsonDoc.toJson());
    }
    return result;
}

QString Project::getSectionName(const QString &fileName)
{
    QString fileType = "";
    if (fileName.endsWith(Project::sourceFileExtension)) {
        fileType = Project::sourcesSection;
    }
    else if(fileName.endsWith(Project::headerFileExtension)){
        fileType = Project::headersSection;
    }
    return fileType;
}


bool Project::writeFile(const QString &name, const QString &content)
{
    QFile file(m_projectDir.filePath(name));
    bool result = file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    if (!content.isEmpty()) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << content;
    }
    file.close();
    return result;
}

int Project::getFileNameAutoIncrement(QStringList &fileList,
                                      const QString &prefix,
                                      const QString &postfix)
{
    int maxNumber = 0;
    if (fileList.size() > 0) {
        QCollator collator;
        collator.setNumericMode(true);
        std::sort(fileList.begin(),
                  fileList.end(),
                  [&collator](const QString &fileName1, const QString &fileName2) {
                      return collator.compare(fileName1, fileName2) < 0;
                  });
        QRegularExpression re(prefix + "(\\d+)" + postfix);
        int lastProjectIndex = fileList.lastIndexOf(re);
        if (lastProjectIndex != -1) {
            maxNumber = re.match(fileList.at(lastProjectIndex)).captured(1).toInt();
        }
    }
    return maxNumber + 1;
}

bool Project::createProjectFile(const QString &name)
{
    m_projectJson["name"] = name;
    QJsonArray sources, headers;
    sources.append(Project::defaultSourceName);
    m_projectJson[Project::sourcesSection] = sources;
    headers.append(Project::defaultHeaderName);
    m_projectJson[Project::headersSection] = headers;
    QJsonDocument jsonDoc(m_projectJson);
    return writeFile(Project::projectFileName, jsonDoc.toJson());
}

bool Project::getIsOpened()
{
    return m_isOpened;
}

bool Project::generateMakeFile()
{
    QFile makeFile(m_projectDir.filePath(Project::buildFileName));
    bool result = false;
    if (makeFile.open(QIODevice::WriteOnly)) {
        QTextStream makefile(&makeFile);
        if (SETTINGS.currentTarget() == SettingsManager::TARGET::EDISON) {
            makefile << QString("CC=%1\n").arg(SETTINGS.edisonCompilerPath());
            makefile << QString("%1:\n").arg(getName());
            makefile << QString("\t$(CC) --sysroot=%1 ").arg(SETTINGS.edisonSysrootPath());
        }

        if (SETTINGS.currentTarget() == SettingsManager::TARGET::MINGW) {
            makefile << QString("CC=%1\n").arg(SETTINGS.mingwCompilerPath());
            makefile << QString("%1:\n").arg(getName());
            makefile << QString("\t$(CC) ");
        }

        for (const auto &source : getSources()) {
            makefile << source << " ";
        }

        for (const auto &source : getHeaders()) {
            makefile << source << " ";        }


        if (SETTINGS.currentTarget() == SettingsManager::TARGET::MINGW) {
            makefile << QString("-o %1.exe ").arg(getName());

            for (const auto &option : SETTINGS.mingwCompilerOptions()) {
                makefile << option << " ";
            }

            for (const auto &includes : SETTINGS.mingwIncludesPath()) {
                makefile << includes << " ";
            }

            for (const auto &libs : SETTINGS.mingwLibsPaths()) {
                makefile << libs << " ";
            }
        }

        if (SETTINGS.currentTarget() == SettingsManager::TARGET::EDISON) {
            makefile << QString("-o %1.bin ").arg(getName());

            for (const auto &options : SETTINGS.edisonCompilerOptions()) {
                makefile << options << " ";
            }
        }

        makeFile.close();
        result = true;
    }
    return result;
}

QString Project::getName()
{
    return m_projectJson["name"].toString();
}

QStringList Project::getSources()
{
    QStringList sourceList;
    for (auto fileName : m_projectJson[Project::sourcesSection].toArray().toVariantList()) {
        sourceList << fileName.toString();
    }
    return sourceList;
}

QStringList Project::getHeaders()
{
    QStringList headerList;
    for (auto fileName : m_projectJson[Project::headersSection].toArray().toVariantList()) {
        headerList << fileName.toString();
    }
    return headerList;
}
