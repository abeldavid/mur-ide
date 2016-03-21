#include "project.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QCollator>
#include <algorithm>
#include <QFile>
#include <QFileInfo>
#include <QDir>

const QString Project::projectFileName = ".roboproject";
const QString Project::projectsRootPath = "MURProjects";
const QString Project::sourceFileExtension = ".cpp";
const QString Project::headerFileExtension = ".h";
const QString Project::defaultSourceName = "main" + Project::sourceFileExtension;
const QString Project::defaultHeaderName = "main" + Project::headerFileExtension;
const QString Project::defaultSource = "#include \"" + Project::defaultHeaderName + "\"\n\n"
                                        "void main () {\n\n}";
const QString Project::defaultHeader = "#define HELLO \"Hello World!\" ";
const QString Project::defaultProjectPrefix = "Project_";
const QString Project::multiFileSeparator = " + ";
const QHash<QString, QString> Project::defaultFilePrefixes({
        {Project::sourceFileExtension, QString("source_")},
        {Project::headerFileExtension, QString("header_")},
        {Project::sourceFileExtension + Project::multiFileSeparator + Project::headerFileExtension,
            QString("module_")
        }
    });

Project::Project(QObject *parent) : QObject(parent)
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
        if (this->addFile(Project::defaultSourceName, Project::defaultSource) and
                this->addFile(Project::defaultHeaderName, Project::defaultHeader) and
                this->createProjectFile(Project::projectFileName)) {
            result = true;
            this->isOpened = true;
        }
    }
    return result;
}

bool Project::createFile(const QString &name)
{
    return m_projectDir.exists() and this->addFile(name);
}

bool Project::addExistingFile(const QString &path)
{
    QFileInfo sourceFileInfo(path);
    bool fileCopySuccess = false;
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
    fileCopySuccess = QFile::copy(path, targetFileName);
    return fileCopySuccess;
}

QString Project::getDefaultProjectName()
{
    QDir dir(m_projectsRoot);
    QString projectPrefix = Project::defaultProjectPrefix;
    QStringList filters;
    filters << projectPrefix + QString("*");
    QStringList dirsInRoot = dir.entryList(filters, QDir::Dirs, QDir::NoSort);
    int newProjectNumber = this->getFileNameAutoIncrement(dirsInRoot, projectPrefix);
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

bool Project::addFile(const QString &name, const QString &content)
{
    QFile file(m_projectDir.absolutePath() + QDir::separator() + name);
    bool result = file.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!content.isEmpty()) {
        QTextStream out(&file);
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

bool Project::createProjectFile(const QString &pathName)
{
    qDebug() << pathName;
    return true;
}

bool Project::getIsOpened()
{
    return this->isOpened;
}
