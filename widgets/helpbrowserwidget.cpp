#include "helpbrowserwidget.h"
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QtHelp/QHelpContentWidget>

HelpBrowser::HelpBrowser(QHelpEngine* helpEngine, QWidget* parent):
                        QTextBrowser(parent),
                        m_helpEngine(helpEngine),
                        m_helpContextMenu(new QMenu(this))
{
    setReadOnly(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    QAction *copyAction = new QAction("Копировать", this);
    m_helpContextMenu->addAction(copyAction);
    connect(copyAction, SIGNAL(triggered(bool)), this, SLOT(copy()));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(onCustomContextMenu(const QPoint)));
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
    bool isCopyEvent = event->modifiers().testFlag(Qt::ControlModifier) and
                      (event->key() == Qt::Key_C or event->key() == Qt::Key_Insert);
    bool isMoveEvent = event->key() == Qt::Key_Up or
                       event->key() == Qt::Key_Down or
                       event->key() == Qt::Key_Left or
                       event->key() == Qt::Key_Right or
                       event->key() == Qt::Key_PageUp or
                       event->key() == Qt::Key_PageDown or
                       event->key() == Qt::Key_End or
                       event->key() == Qt::Key_Home;
    if (isCopyEvent or isMoveEvent) {
        QTextBrowser::keyPressEvent(event);
    }
}

void HelpBrowser::onCustomContextMenu(QPoint point)
{
    m_helpContextMenu->exec(mapToGlobal(point));
}
