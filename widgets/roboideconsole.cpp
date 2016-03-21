#include "roboideconsole.h"

#include <QPalette>
#include <QTextCursor>
#include <QDebug>
#include <iostream>

RoboIdeConsole::RoboIdeConsole(QWidget *parent) : QTextEdit(parent)
{
    setReadOnly(true);
}


