#ifndef COMPILATIONOPTIONSVIEW_H
#define COMPILATIONOPTIONSVIEW_H

#include <QTableWidget>

class CompilationOptionsView : public QTabWidget
{
    Q_OBJECT
public:
    explicit CompilationOptionsView(QWidget *parent = 0);
    void setOptions(const QStringList &options);
    QStringList getOptions();
signals:

public slots:
private:
    QStringList m_compilationOptions;
};

#endif // COMPILATIONOPTIONSVIEW_H
