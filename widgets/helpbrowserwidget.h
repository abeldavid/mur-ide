#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QTextBrowser>
#include <QHelpEngine>
#include <QKeyEvent>
#include <QDropEvent>
#include <QMenu>
#include <QAction>

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    HelpBrowser(QHelpEngine* helpEngine, QWidget* parent = 0);
    QVariant loadResource (int type, const QUrl& name);
private:
    void keyPressEvent(QKeyEvent *event);
    void dropEvent(QDropEvent *event);
    QHelpEngine *m_helpEngine;
    QMenu *m_helpContextMenu;
    QAction *m_copyAction;
};

#endif // HELPBROWSER_H
