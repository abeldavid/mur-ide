#ifndef PROJECTTREE_H
#define PROJECTTREE_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileIconProvider>
//#include <QAction>


class ProjectTree : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectTree(QWidget *parent = 0);
    virtual ~ProjectTree();

signals:
    void fileSelected(QString fileName);
    void projectLoaded();

public slots:
    void loadProject();
    void closeProject();
    void onItemClicked(QTreeWidgetItem* item);

private:
    QTreeWidget *m_tree;
    QTreeWidgetItem *m_sources, *m_headers;
    QFileIconProvider m_iconPovider;
    void prepareTreeView();

};

#endif // PROJECTTREE_H
