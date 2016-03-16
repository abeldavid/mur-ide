#include "projecttreewidget.h"
#include "projectmanager.h"
#include <QDebug>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QDir>
#include <QString>

ProjectTree::ProjectTree(QWidget *parent) :
    QWidget(parent)
{
    QFileSystemModel *m_fileModel = new QFileSystemModel;

    QString projectsRoot = ProjectManager::instance().projectsRoot();
//    qDebug() << projectsRoot;
    m_fileModel->setRootPath(projectsRoot);
    QTreeView *tree = new QTreeView(this);
    tree->setModel(m_fileModel);
    tree->hideColumn(1);
    tree->hideColumn(2);
    tree->hideColumn(3);
    tree->setRootIndex(m_fileModel->index(projectsRoot));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->addWidget(tree);

    connect(tree,SIGNAL(clicked(QModelIndex)),this,SLOT(itemSelected(QModelIndex)));
}
void ProjectTree::itemSelected(const QModelIndex & index){
    qDebug()<<index;

}

ProjectTree::~ProjectTree()
{
//    m_treeView->deleteLater();
}
