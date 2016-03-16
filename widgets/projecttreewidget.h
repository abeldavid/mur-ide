#ifndef PROJECTTREE_H
#define PROJECTTREE_H

#include <QWidget>
#include <QFileSystemModel>
//#include <QAction>


class ProjectTree : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectTree(QWidget *parent = 0);
    virtual ~ProjectTree();
private:
    QFileSystemModel m_fileModel;
//    QTreeWidget m_treeView;
private slots:
    void ItemSelected(const QModelIndex & index);
};

#endif // PROJECTTREE_H
