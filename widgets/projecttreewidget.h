#ifndef PROJECTTREE_H
#define PROJECTTREE_H

#include <QWidget>
#include <QFileSystemModel>
#include <QTreeView>
//#include <QAction>


class ProjectTree : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectTree(QWidget *parent = 0);
    virtual ~ProjectTree();
public slots:
    void loadProject(QString projectDir);
    void closeProject();
private:
    QFileSystemModel *m_fileModel;
    QTreeView *m_tree;
private slots:
    void itemSelected(const QModelIndex & index);
};

#endif // PROJECTTREE_H
