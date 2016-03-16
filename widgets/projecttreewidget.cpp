#include "projecttreewidget.h"
#include "projectmanager.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QDir>
#include <QString>

ProjectTree::ProjectTree(QWidget *parent) :
    QWidget(parent),
    m_fileModel(new QFileSystemModel(this)),
    m_tree(new QTreeView(this))
{
    m_tree->setModel(m_fileModel);
    m_tree->hideColumn(1);
    m_tree->hideColumn(2);
    m_tree->hideColumn(3);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->addWidget(m_tree);

    connect(m_tree, SIGNAL(clicked(QModelIndex)), this, SLOT(itemSelected(QModelIndex)));
    connect(&ProjectManager::instance(), SIGNAL(projectCreated(QString)), this, SLOT(loadProject(QString)));
}

void ProjectTree::loadProject(QString projectDir) {
    m_fileModel->setRootPath(projectDir);
    m_tree->setRootIndex(m_fileModel->index(projectDir));
    m_tree->hideColumn(1);
    m_tree->hideColumn(2);
    m_tree->hideColumn(3);
}

void ProjectTree::itemSelected(const QModelIndex & index){
    qDebug()<<index;

}

ProjectTree::~ProjectTree()
{
}
