#ifndef PROJECTCREATEWIDGET_H
#define PROJECTCREATEWIDGET_H

#include <QDialog>
#include <QLineEdit>

class ProjectCreateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProjectCreateDialog(QWidget *parent = 0);
private:
    QLineEdit *m_dirLocationEdit;
    QLineEdit *m_nameEdit;
private slots:
    void onLoadClicked();

};

#endif // PROJECTCREATEWIDGET_H
