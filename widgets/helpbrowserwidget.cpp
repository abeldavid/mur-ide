#include "helpbrowserwidget.h"
#include <QApplication>
#include <QDebug>
#include <QtHelp/QHelpContentWidget>

HelpBrowser::HelpBrowser(QHelpEngine* helpEngine, QWidget* parent):
                        QTextBrowser(parent),
                        m_helpEngine(helpEngine),
                        m_helpContextMenu(new QMenu(this)),
                        m_copyAction(new QAction("Копировать", this))
{
    setReadOnly(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    m_copyAction->setEnabled(false);
    m_helpContextMenu->addAction(m_copyAction);
    connect(m_copyAction, SIGNAL(triggered(bool)), this, SLOT(copy()));
    connect(this, &HelpBrowser::customContextMenuRequested, [=](const QPoint &point){
        m_helpContextMenu->exec(mapToGlobal(point));
    });
    connect(this, &HelpBrowser::copyAvailable, [=](bool isAvailable) {
        m_copyAction->setEnabled(isAvailable);
    });
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
    if (name.scheme() == "qthelp") {
        return QVariant(m_helpEngine->fileData(name));
    }
    else {
        return QTextBrowser::loadResource(type, name);
    }
}

void HelpBrowser::keyPressEvent(QKeyEvent *event)
{
    bool isCopyEvent = event->modifiers().testFlag(Qt::ControlModifier) &&
                      (event->key() == Qt::Key_C || event->key() == Qt::Key_Insert);
    bool isMoveEvent = event->key() == Qt::Key_Up ||
                       event->key() == Qt::Key_Down ||
                       event->key() == Qt::Key_Left ||
                       event->key() == Qt::Key_Right ||
                       event->key() == Qt::Key_PageUp ||
                       event->key() == Qt::Key_PageDown ||
                       event->key() == Qt::Key_End ||
                       event->key() == Qt::Key_Home;
    if (isCopyEvent || isMoveEvent) {
        QTextBrowser::keyPressEvent(event);
    }
}

void HelpBrowser::dropEvent(QDropEvent *event)
{
    Q_UNUSED(event)
}
