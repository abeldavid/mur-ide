#ifndef ROBOIDETEXTEDITOR_H
#define ROBOIDETEXTEDITOR_H

#include <QObject>
#include <QWidget>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciapis.h>
#include <Qsci/qscilexercpp.h>

class RoboIdeTextEditor : public QsciScintilla
{
    Q_OBJECT
public:
    explicit RoboIdeTextEditor(QWidget *parent = 0);

signals:
    void textChanged(QString);
    void projectFileSaved();

public slots:
    void showContent(const QString &content, const QString &fileName);
    void onFileSaved(QString);
    QString fileName() const;
    bool fileExists() const;

private:
    void setupEditor();
    void setupLexer();
    void setupUi();
    bool m_isFileExist;
    QString m_fileName;
    QsciLexerCPP* m_lexCpp;
    bool m_skipNullTextChanged = false;
};

#endif // ROBOIDETEXTEDITOR_H
