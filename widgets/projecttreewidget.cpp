#include "projecttreewidget.h"
#include "projectmanager.h"
#include "project.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QIcon>

ProjectTree::ProjectTree(QWidget *parent) :
    QWidget(parent),
    m_tree(new QTreeWidget(this))
{
//    prepareTreeView();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->addWidget(m_tree);  

    m_tree->setColumnCount(1);
    m_tree->setHeaderHidden(true);

    QObject::connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*)));
//    connect(m_tree, SIGNAL(clicked(QModelIndex)), this, SLOT(itemSelected(QModelIndex)));
}

void ProjectTree::loadProject()
{
    closeProject();
    prepareTreeView();
    m_tree->setHeaderHidden(false);
    m_tree->setHeaderLabel(ProjectManager::instance().getProjectName());

    if (ProjectManager::instance().fileExists(Project::buildFileName)) {
        QTreeWidgetItem *buildFile = new QTreeWidgetItem(m_tree, QStringList(Project::buildFileName));
        m_tree->insertTopLevelItem(1, buildFile);
    }

    for (auto fileName : ProjectManager::instance().getSourceFiles()) {
        QTreeWidgetItem *sourceItem = new QTreeWidgetItem(m_sources, QStringList(fileName));
        QFileInfo sourceInfo(ProjectManager::instance().pathToFile(fileName));
        QIcon sourceIcon = m_iconPovider.icon(sourceInfo);
        sourceItem->setIcon(0, sourceIcon);
        m_sources->addChild(sourceItem);
    }
    for (auto fileName : ProjectManager::instance().getHeaderFiles()) {
        QTreeWidgetItem *headerItem = new QTreeWidgetItem(m_headers, QStringList(fileName));
        QFileInfo headerInfo(ProjectManager::instance().pathToFile(fileName));
        QIcon headerIcon = m_iconPovider.icon(headerInfo);
        headerItem->setIcon(0, headerIcon);
        m_headers->addChild(headerItem);
    }
    m_tree->expandAll();
}

void ProjectTree::closeProject()
{
    m_tree->clear();
    m_tree->setHeaderHidden(true);
}

void ProjectTree::onItemClicked(QTreeWidgetItem* item)
{
    QString itemText = item->data(0, Qt::DisplayRole).toString();
    if (item->parent() or itemText == Project::projectFileName or itemText == Project::buildFileName) {
        emit fileSelected(item->data(0, Qt::DisplayRole).toString());
    }
}

void ProjectTree::prepareTreeView()
{
    QIcon folderIcon = m_iconPovider.icon(QFileIconProvider::Folder);
    QIcon blankIcon = m_iconPovider.icon(QFileIconProvider::File);

    QTreeWidgetItem *project = new QTreeWidgetItem(m_tree, QStringList(Project::projectFileName));
    project->setIcon(0, blankIcon);
    m_tree->insertTopLevelItem(0, project);

    m_sources = new QTreeWidgetItem(m_tree, QStringList(QString(tr("Исходники"))));
    m_sources->setIcon(0, folderIcon);
    m_tree->insertTopLevelItem(2, m_sources);

    m_headers = new QTreeWidgetItem(m_tree, QStringList(QString(tr("Заголовочные"))));
    m_headers->setIcon(0, folderIcon);
    m_tree->insertTopLevelItem(3, m_headers);
}

void ProjectTree::itemSelected(const QModelIndex & index)
{
//    emit fileSelected(m_fileModel->fileName(index));
}

ProjectTree::~ProjectTree()
{
//    delete m_iconPovider;
}
