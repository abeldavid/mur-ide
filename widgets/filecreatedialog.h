#ifndef FILECREATEDIALOG_H
#define FILECREATEDIALOG_H

#include <QDialog>
#include <QLineEdit>

class FileCreateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FileCreateDialog(QWidget *parent = 0);

signals:

public slots:

private:
    QLineEdit *m_nameEdit;

private slots:
    void createFile();
};

#endif // FILECREATEDIALOG_H
