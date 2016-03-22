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
    prepareTreeView();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->addWidget(m_tree);

    connect(m_tree, SIGNAL(clicked(QModelIndex)), this, SLOT(itemSelected(QModelIndex)));
}

void ProjectTree::loadProject(QString projectDir)
{
    m_fileModel->setRootPath(projectDir);
    m_tree->setRootIndex(m_fileModel->index(projectDir));
    prepareTreeView();
}

void ProjectTree::closeProject()
{
    qDebug()<< "closing project in tree";
}

void ProjectTree::prepareTreeView()
{
    m_tree->hideColumn(1);
    m_tree->hideColumn(2);
    m_tree->hideColumn(3);
    m_tree->setHeaderHidden(true);
}

void ProjectTree::itemSelected(const QModelIndex & index)
{
    emit fileSelected(m_fileModel->fileName(index));
}

ProjectTree::~ProjectTree()
{
}
