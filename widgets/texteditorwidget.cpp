#include "texteditorwidget.h"

#include <QFontDatabase>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QFileDialog>
#include <QInputMethodEvent>
#include <QCoreApplication>
#include <QMessageBox>
#include <QPixmap>
#include <Qsci/qsciapis.h>

TextEditorWidget::TextEditorWidget(QWidget *parent)
    : QsciScintilla(parent),
      m_isFileExist(false),
      m_inDoubleParenthesisMode(false),
      m_fileName(QString(""))
{
    setupEditor();
    setupUi();
    QObject::connect(this, SIGNAL(textChanged()), this, SLOT(handleChangedText()));
    markerDefine(QPixmap(":/icons/icons/widgeticons/bullet-red.png"), m_errorMarkerCode);
}

void TextEditorWidget::showContent(const QString &fileName, const QString &content)
{
    setReadOnly(false);
    if (content.size()) {
        m_skipNullTextChanged = true;
    }
    setText(content);
    setModified(false);

    m_isFileExist = true;
    m_fileName = fileName;

    QList<int> lines = m_errorsFound.values(fileName);
    for (auto lineNumber: lines) {
        markerAdd(lineNumber-1, m_errorMarkerCode);
    }
}

void TextEditorWidget::onFileSaved(QString)
{
    setModified(false);
}

QString TextEditorWidget::fileName() const
{
    return m_fileName;
}

bool TextEditorWidget::fileExists() const
{
    return m_isFileExist;
}

void TextEditorWidget::clearText()
{
    clear();
    setModified(false);
}

void TextEditorWidget::highlightError(const QString &fileName, int lineNumber)
{
    m_errorsFound.insert(fileName, lineNumber);
    if (m_fileName == fileName && !isModified()) {
        markerAdd(lineNumber-1, m_errorMarkerCode);
    }
}

void TextEditorWidget::highlightFatalErrorLine(int lineNumber, int columnNumber)
{
    setCursorPosition(lineNumber-1, columnNumber-1);
}

void TextEditorWidget::clearErrors()
{
    markerDeleteAll();
    m_errorsFound.clear();
}

void TextEditorWidget::closeFile()
{
    m_fileName = "";
}

QString TextEditorWidget::getWordUnderCursor()
{
    int line;
    int index;
    getCursorPosition(&line, &index);
    return wordAtLineIndex(line, index);

}

void TextEditorWidget::setupEditor()
{

    setUtf8(true);
    setEolMode(EolUnix);

    m_lexCpp = new QsciLexerCPP(this);
    QsciAPIs *api = new QsciAPIs(m_lexCpp);
    api->load("cpp.api");
    api->load("roboIDE.api");
    api->prepare();
    setLexer(m_lexCpp);
    setupLexer();
    setAutoCompletionReplaceWord(true);
    setAutoCompletionCaseSensitivity(true);
    setAutoCompletionThreshold(1);
    setAutoCompletionSource(QsciScintilla::AcsAll);

    setCallTipsPosition(CallTipsAboveText);
    setCallTipsStyle(CallTipsContext);
    setCallTipsBackgroundColor("#D0D0D0");
    setCallTipsForegroundColor("#252525");

    //! Setup highlighting on current line
    setCaretLineVisible(true);
    setCaretLineBackgroundColor(QColor("#323232"));
    setCaretForegroundColor(QColor("#D0D0D0"));
    setMarginsForegroundColor(QColor("#777777"));

    //! Setup right margin
    setMarginsBackgroundColor(QColor("gainsboro"));
    setMarginLineNumbers(1, true);
    setMarginWidth(1, 40);

    //! Setup indent for 4 spaces
    setAutoIndent(true);
    setIndentationGuides(false);
    setIndentationsUseTabs(false);
    setIndentationWidth(4);

    //! Brace matching highlighting
    setBraceMatching(QsciScintilla::SloppyBraceMatch);

    setMatchedBraceBackgroundColor("#76715E");
    setMatchedBraceForegroundColor("#FFFFFF");

    setUnmatchedBraceForegroundColor("#FFFFFF");
    setUnmatchedBraceBackgroundColor("#DA2772");

    //!Styles
    setMarginsBackgroundColor(QColor(37, 37, 37));
    setStyleSheet("QWidget {"
                     "border-top: none;"
                     "border-left: none;"
                     "border-top: none;"
                     "border-right: none;"
                 "}");

}
//1c1c1c - MENU
//363636 - SELCET
void TextEditorWidget::setupLexer()
{
    //! Setup encoding and font
    QFont font = QFont("Courier");
    font.setPointSize(10);
    m_lexCpp->setFont(font);

    m_lexCpp->setPaper(QColor(37, 37, 37));

    m_lexCpp->setColor("#FFFFFF", m_lexCpp->Default);
    m_lexCpp->setColor("#E7DB75", m_lexCpp->PreProcessor);
    m_lexCpp->setColor("#E7DB75", m_lexCpp->PreProcessorComment);

    m_lexCpp->setColor("#76715E", m_lexCpp->Comment);
    m_lexCpp->setColor("#76715E", m_lexCpp->CommentLine);
    m_lexCpp->setColor("#76715E", m_lexCpp->CommentDoc);
    m_lexCpp->setColor("#76715E", m_lexCpp->CommentDocKeyword);

    m_lexCpp->setColor("#66D9EE", m_lexCpp->Keyword);
    m_lexCpp->setColor("#AE82FF", m_lexCpp->KeywordSet2);

    m_lexCpp->setColor("#FE9720", m_lexCpp->SingleQuotedString);
    m_lexCpp->setColor("#FE9720", m_lexCpp->DoubleQuotedString);
    m_lexCpp->setColor("#FE9720", m_lexCpp->UnclosedString);

    m_lexCpp->setColor("#AE82FF", m_lexCpp->Number);
    m_lexCpp->setColor("#FFFFFF", m_lexCpp->Operator);

    m_lexCpp->setColor("#FFFFFF", m_lexCpp->Identifier);
    m_lexCpp->setColor("#36AF90", m_lexCpp->UserLiteral);
    m_lexCpp->setColor("#36AF90", m_lexCpp->UUID);
}

void TextEditorWidget::setupUi()
{
    QFile editStyle(":/dark/styles/texteditscrollbar.css");
    editStyle.open(QFile::ReadOnly);
    QString styleSheet = editStyle.readAll();
    editStyle.close();
    // autocomplete
    styleSheet += "QListView{background: #464646;color:#D0D0D0;}";
    styleSheet += "QListView::item{padding:2px;}";
    styleSheet += "QListView::item:selected{background: #252525; border: none; color: #AAAAAA;}";
    setStyleSheet(styleSheet);
}

void TextEditorWidget::keyPressEvent(QKeyEvent *e)
{
    if((e->modifiers() == Qt::CTRL) && (e->key() == Qt::Key_Space)) {
        autoCompleteFromAll();
        return; // не дает дописать NULL
    }
    if (m_inDoubleParenthesisMode) {
        int line;
        int index;
        getCursorPosition(&line, &index);
        setCursorPosition(line, index + 1);
        m_inDoubleParenthesisMode = false;
        return;
    }
    m_inDoubleParenthesisMode = false;
    if(e->key() == Qt::Key_ParenLeft) {
        int line;
        int index;
        getCursorPosition(&line, &index);
        insertAt("(", line, index);
        insertAt(")", line, index+1);
        setCursorPosition(line, index + 1);
        callTip();
        m_inDoubleParenthesisMode = true;
        return;
    }
    if(e->key() == Qt::Key_BracketLeft){
        int line;
        int index;
        getCursorPosition(&line, &index);
        insertAt("]", line, index);
        m_inDoubleParenthesisMode = true;
    }
    if(e->key() == Qt::Key_BraceLeft){
        int line;
        int index;
        getCursorPosition(&line, &index);
        insertAt("}", line, index);
        m_inDoubleParenthesisMode = true;
    }
    QsciScintilla::keyPressEvent(e);
}

void TextEditorWidget::handleChangedText()
{
   if (!isModified()) {
       setModified(true);
       emit fileModified();
   }
}

