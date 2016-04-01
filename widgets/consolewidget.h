#ifndef ROBOIDECONSOLE_H
#define ROBOIDECONSOLE_H

#include <QWidget>
#include <QString>
#include <QTextEdit>
#include <QColor>
#include <QMenu>
#include <QKeyEvent>
#include <QDropEvent>
#include <QAction>

struct OutputBuffer
{
    QString buffer;
    bool isError;
};

Q_DECLARE_METATYPE(OutputBuffer)

class ConsoleWidget : public QTextEdit
{
    Q_OBJECT
public:
    explicit ConsoleWidget(QWidget *parent = 0);

signals:

public slots:
    void appendMessage(const QString &text, bool isError = false);

private:
    QColor m_defaultTextColor;
    QColor m_errorTextColor;
    QMenu *m_consoleContextMenu;
    QAction *m_copyAction;
    OutputBuffer m_output;
    void keyPressEvent(QKeyEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // ROBOIDECONSOLE_H
