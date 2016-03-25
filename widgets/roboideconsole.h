#ifndef ROBOIDECONSOLE_H
#define ROBOIDECONSOLE_H

#include <QWidget>
#include <QString>
#include <QTextEdit>
#include <QColor>

class RoboIdeConsole : public QTextEdit
{
    Q_OBJECT
public:
    explicit RoboIdeConsole(QWidget *parent = 0);

signals:

public slots:
    void appendMessage(const QString &text, bool isError = false);
    void appendCompilationResult(const QString &text);

private:
    QColor m_defaultTxetColor;
    QColor m_errorTextColor;
};

#endif // ROBOIDECONSOLE_H
