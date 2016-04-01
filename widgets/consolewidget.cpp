#include "consolewidget.h"

#include <QPalette>
#include <QDebug>
#include <QFont>
#include <QRegExp>
#include <QStringList>

ConsoleWidget::ConsoleWidget(QWidget *parent) :
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
    connect(this, &ConsoleWidget::customContextMenuRequested, [=](const QPoint &point){
        m_consoleContextMenu->exec(mapToGlobal(point));
    });
    connect(this, &ConsoleWidget::copyAvailable, [=](bool isAvailable) {
        m_copyAction->setEnabled(isAvailable);
    });

    QFile editStyle(":/dark/styles/texteditscrollbar.css");
    editStyle.open(QFile::ReadOnly);
    QString styleSheet = editStyle.readAll();
    editStyle.close();
    setStyleSheet(styleSheet);

    QFile consoleStyleFile(":/dark/styles/console.css");
    consoleStyleFile.open(QFile::ReadOnly);
    QString consoleStyleSheet = consoleStyleFile.readAll();
    document()->setDefaultStyleSheet(consoleStyleSheet);
    consoleStyleFile.close();
}

void ConsoleWidget::appendMessage(const QString &text, bool isError)
{
    if (m_output.buffer.isEmpty()) {
        m_output.isError = isError;
    }
    m_output.buffer.append(text);
    if (m_output.buffer.contains("\n")) {
        QStringList parts = m_output.buffer.split("\n");
        for (int i = 0; i < parts.size()-1; i++) {
            if (!isError) {
                QRegExp rx("((?:\\w|\\.|-|\\\\|/)+[0-9:]*:)\\s(.+)");
                if(rx.indexIn(parts[i]) != -1) {
                    append("<div class=\"outputOk\"><b>" + rx.cap(1).simplified() + "</b> " + rx.cap(2).simplified() + "</div>");
                } else {
                    append("<div class=\"outputOk\">" + parts[i].simplified() + "</div>");
                }
            }
            else {
                QRegExp rx("((?:\\w|\\.|-|\\\\|/|:)+[0-9:]*:)\\s(.+)");
                if(rx.indexIn(parts[i]) != -1) {
                    append("<div class=\"outputError\"><b>" + rx.cap(1).simplified() + "</b> " + rx.cap(2).simplified() + "</div>");
                } else {
                    append("<div class=\"outputError\">" + parts[i].simplified() + "</div>");
                }
            }
        }
        if (parts.last().simplified().isEmpty()) {
            m_output.buffer.clear();
        } else {
            m_output.buffer = parts.last();
        }
    }
}

void ConsoleWidget::keyPressEvent(QKeyEvent *event)
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

void ConsoleWidget::dropEvent(QDropEvent *event)
{
    Q_UNUSED(event)
}
