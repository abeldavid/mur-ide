#ifndef PROJECTCREATEWIDGET_H
#define PROJECTCREATEWIDGET_H

#include <QDialog>
#include <QLineEdit>

class ProjectCreateDialog : public QDialog
{
    Q_OBJECT
signals:
    void createProjectConfirmed(QString path, QString name);
public:
    explicit ProjectCreateDialog(QWidget *parent = 0);
private:
    QLineEdit *m_dirLocationEdit;
    QLineEdit *m_nameEdit;
private slots:
    void selectFolder();
    void createProject();

};

#endif // PROJECTCREATEWIDGET_H
