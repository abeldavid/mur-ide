#include "roboideconsole.h"

#include <QPalette>
#include <QDebug>
#include <QFont>

RoboIdeConsole::RoboIdeConsole(QWidget *parent) :
    QTextEdit(parent),
    m_defaultTextColor(QColor("light grey")),
    m_errorTextColor(QColor("red")),
    m_consoleContextMenu(new QMenu(this)),
    m_copyAction(new QAction("Копировать", this)),
    m_output({QString(), false})
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    m_copyAction->setEnabled(false);
    m_consoleContextMenu->addAction(m_copyAction);

    connect(m_copyAction, SIGNAL(triggered(bool)), this, SLOT(copy()));
    connect(this, &RoboIdeConsole::customContextMenuRequested, [=](const QPoint &point){
        m_consoleContextMenu->exec(mapToGlobal(point));
    });
    connect(this, &RoboIdeConsole::copyAvailable, [=](bool isAvailable) {
        m_copyAction->setEnabled(isAvailable);
    });

    QFile styleFile(":/dark/styles/console.css");
    styleFile.open(QFile::ReadOnly);
    QString styleSheet = styleFile.readAll();
    document()->setDefaultStyleSheet(styleSheet);
    styleFile.close();

    QFont font("Courier", 10);
    font.setStyleHint(QFont::Monospace);
    setCurrentFont(font);
}

void RoboIdeConsole::appendMessage(const QString &text, bool isError)
{
    if (m_output.buffer.isEmpty()) {
        m_output.isError = isError;
    }
    m_output.buffer.append(text);
    if (m_output.buffer.endsWith("\n")) {
        if (!isError) {
            append("<div class=\"outputOk\">" + m_output.buffer + "</div>");
        }
        else {
            append("<div class=\"outputError\">" + m_output.buffer + "</div>");
        }
        m_output.buffer.clear();
    }
}

void RoboIdeConsole::keyPressEvent(QKeyEvent *event)
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
        QTextEdit::keyPressEvent(event);
    }

}

void RoboIdeConsole::dropEvent(QDropEvent *event)
{
    Q_UNUSED(event)
}
