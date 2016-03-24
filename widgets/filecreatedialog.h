#ifndef FILECREATEDIALOG_H
#define FILECREATEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QStringList>

class FileCreateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FileCreateDialog(QWidget *parent = 0);

signals:
    void createFileConfirmed(QStringList fileNames);
public slots:

private:
    QLineEdit *m_nameEdit;

private slots:
    void createFile();
    void setDefaultFileName(QString extension);
};

#endif // FILECREATEDIALOG_H
