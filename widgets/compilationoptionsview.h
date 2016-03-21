#ifndef COMPILATIONOPTIONSVIEW_H
#define COMPILATIONOPTIONSVIEW_H

#include <QListWidget>
#include <QAction>
#include <QMenu>

class CompilationOptionsView : public QListWidget
{
    Q_OBJECT
public:
    explicit CompilationOptionsView(QWidget *parent = 0);
    void setOptions(const QStringList &options);
    void addOption(const QString &option);
    void setModified(bool modified);
    bool isModified() const;
    QStringList getOptions() const;
signals:

public slots:
private slots:
    void showContextMenu(const QPoint &pos);
private:
    QStringList m_compilationOptions;
    QAction* m_addOptionAct;
    QAction* m_removeOptionAct;
    QMenu* m_contextMenu;
    bool m_isModified = false;

    void createMenu();
    void setup();
};

#endif // COMPILATIONOPTIONSVIEW_H
