#include "consolewidget.h"

#include <QPalette>
#include <QDebug>
#include <QFont>
#include <QRegExp>
#include <QStringList>
#include <QScrollBar>

ConsoleWidget::ConsoleWidget(QWidget *parent) :
    QTextEdit(parent),
    m_defaultTextColor(QColor("light grey")),
    m_errorTextColor(QColor("red")),
    m_consoleContextMenu(new QMenu(this)),
    m_copyAction(new QAction("Копировать", this)),
    m_clearAction(new QAction("Очистить", this)),
    m_output({QString(), false})
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    m_copyAction->setEnabled(false);
    m_consoleContextMenu->addAction(m_copyAction);
    m_consoleContextMenu->addAction(m_clearAction);

    connect(m_copyAction, SIGNAL(triggered(bool)), this, SLOT(copy()));
    connect(m_clearAction, SIGNAL(triggered(bool)), this, SLOT(clear()));
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

void ConsoleWidget::appendMessage(const QString &text, bool isError, bool isIDEMessage)
{
    if (m_output.buffer.isEmpty()) {
        m_output.isError = isError;
    }
    m_output.buffer.append(text);
    if (m_output.buffer.contains("\n")) {
        QStringList parts = m_output.buffer.split("\n");
        for (int i = 0; i < parts.size() - 1; i++) {
            QString regexpString, qssClass;
            if (!isError) {
                regexpString = "((?:\\w|\\.|-|\\\\|/)+[0-9:]*:)\\s(.+)";
                qssClass = isIDEMessage ? "outputIDEOk" : "outputOk";
            }
            else {
                regexpString = "((?:\\w|\\.|-|\\\\|/|:)+[0-9:]*:\\s?(?:error|warning)?:?)\\s(.+) ";
                qssClass = isIDEMessage ? "outputIDEError" : "outputError";
            }
            QRegExp rxCompilationOut(regexpString);
            if(rxCompilationOut.indexIn(parts[i]) != -1) {
                if (isError && !isIDEMessage) {
                    QRegExp rxError(".*((?:\\w|\\.)+):(\\d+):(\\d+):\\s?(error|warning):");
                    if (rxError.indexIn(rxCompilationOut.cap(1).simplified()) != -1) {
//                        bool isErrorFatal = (rxError.cap(4) == "error") ? true : false;
                        if (rxError.cap(4) == "error") {
                            emit errorFound(rxError.cap(1), rxError.cap(2).toInt(), rxError.cap(3).toInt());
                        }
                    }
                }
                append("<div class=\"" + qssClass + "\"><b>" + rxCompilationOut.cap(1).simplified() +
                       "</b> " + rxCompilationOut.cap(2).simplified() + "</div>");
            } else {
                append("<div class=\"" + qssClass + "\">" + parts[i].simplified() + "</div>");
            }
        }
        if (parts.last().simplified().isEmpty()) {
            m_output.buffer.clear();
        } else {
            m_output.buffer = parts.last();
        }
    }
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void ConsoleWidget::keyPressEvent(QKeyEvent *event)
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
        QTextEdit::keyPressEvent(event);
    }
}

void ConsoleWidget::dropEvent(QDropEvent *event)
{
    Q_UNUSED(event)
}
