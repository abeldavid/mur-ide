#include "roboideconsole.h"

#include <QPalette>
#include <QDebug>
#include <QFont>

RoboIdeConsole::RoboIdeConsole(QWidget *parent) :
    QTextEdit(parent),
    m_defaultTxetColor(QColor("light grey")),
    m_errorTextColor(QColor("red"))
{
    setReadOnly(true);
    QFont font("Courier", 11);
    font.setStyleHint(QFont::Monospace);
    setCurrentFont(font);
}

void RoboIdeConsole::appendMessage(const QString &text, bool isError)
{
    if (!isError) {
        setTextColor(m_defaultTxetColor);
        append(text);
    }
    else {
        setTextColor(m_errorTextColor);
        append(text);
    }
}

void RoboIdeConsole::appendCompilationResult(const QString &text)
{
    append(text);
}

