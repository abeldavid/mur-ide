#ifndef FILECREATEDIALOG_H
#define FILECREATEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QStringList>
#include <QComboBox>

class FileCreateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FileCreateDialog(QWidget *parent = 0);
    void init();

signals:
    void createFileConfirmed(QStringList fileNames);
public slots:

private:
    QLineEdit *m_nameEdit;
    QComboBox *m_comboBox;

private slots:
    void createFile();
    void setDefaultFileName(QString extension);
};

#endif // FILECREATEDIALOG_H
