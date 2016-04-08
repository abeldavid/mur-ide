#ifndef ROBOIDETEXTEDITOR_H
#define ROBOIDETEXTEDITOR_H

#include <QObject>
#include <QWidget>
#include <QMultiMap>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercpp.h>

class TextEditorWidget : public QsciScintilla
{
    Q_OBJECT
public:
    explicit TextEditorWidget(QWidget *parent = 0);

signals:
    void fileModified();

public slots:
    void showContent(const QString &content, const QString &fileName);
    void onFileSaved(QString);
    QString fileName() const;
    bool fileExists() const;
    void clearText();
    void highlightError(const QString &fileName, int lineNumber);
    void highlightFatalErrorLine(int lineNumber, int columnNumber);
    void clearErrors();
    void closeFile();
    QString getWordUnderCursor();

private:
    bool m_isFileExist;
    bool m_inDoubleParenthesisMode;
    QString m_fileName;
    QsciLexerCPP* m_lexCpp;
    bool m_skipNullTextChanged = false;
    const int m_errorMarkerCode = 0;
    QMultiMap<QString, int> m_errorsFound;
    void setupEditor();
    void setupLexer();
    void setupUi();
    void keyPressEvent(QKeyEvent *e);

private slots:
    void handleChangedText();
};

#endif // ROBOIDETEXTEDITOR_H
