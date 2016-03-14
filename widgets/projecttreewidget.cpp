#include "projecttreewidget.h"
#include "logic/projectmanager.h"
#include <QDebug>


ProjectTree::ProjectTree(QWidget *parent) :
    QWidget(parent)
{
    QFileSystemModel *m_fileModel = new QFileSystemModel;
    QString projectsRoot = QStandardPaths::locate(QStandardPaths::DocumentsLocation,
                                                  "MURProjects",
                                                  QStandardPaths::LocateDirectory);
    if (projectsRoot.isEmpty()) {
        projectsRoot = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                + QDir::separator() + QString("MURProjects");
        QDir().mkdir(projectsRoot);
    }
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

    connect(tree,SIGNAL(clicked(QModelIndex)),this,SLOT(ItemSelected(QModelIndex)));
}
void ProjectTree::ItemSelected(const QModelIndex & index){
    qDebug()<<index;

}

ProjectTree::~ProjectTree()
{
//    m_treeView->deleteLater();
}
