#ifndef HELPWIDGET_H
#define HELPWIDGET_H
#include <QWidget>
#include <QTabWidget>
#include "helpbrowserwidget.h"
#include <QtHelp/QHelpEngine>

class HelpWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HelpWidget(QWidget *parent = 0);
    ~HelpWidget();
private:
    QHelpEngine* m_helpEngine;
    HelpBrowser* m_helpViewer;
    QTabWidget* m_tabWidget;
private slots:
    void goToHelpUrl(QUrl url);
};
#endif // HELPWIDGET_H
