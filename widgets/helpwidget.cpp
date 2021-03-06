#include <QFile>
#include <QMap>
#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QtHelp/QHelpContentWidget>
#include <QtHelp/QHelpIndexModel>

#include "helpwidget.h"

HelpWidget::HelpWidget(QWidget *parent) :
        QWidget(parent),
        m_tabWidget(new QTabWidget(this))
{
    QString helpLocation = "roboHelp.qhc";
    m_helpEngine = new QHelpEngine(helpLocation);
    m_helpEngine->contentWidget()->setExpandsOnDoubleClick(false);
    m_helpEngine->setupData();

    m_tabWidget->addTab(m_helpEngine->contentWidget(), tr("Содержание"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    m_helpViewer = new HelpBrowser(m_helpEngine);
    m_helpViewer->setSource(
                QUrl("qthelp://RoboIDE/robo/doc.html"));

    m_tabWidget->addTab(m_helpViewer, tr("Справка"));

    connect(m_helpEngine->contentWidget(), SIGNAL(linkActivated(QUrl)), this, SLOT(goToHelpUrl(QUrl)));

    mainLayout->addWidget(m_tabWidget);

    QFile helpStyle(":/dark/styles/helpwidget.css");
    helpStyle.open(QFile::ReadOnly);

    QString styleSheet = helpStyle.readAll();
    setStyleSheet(styleSheet);
    helpStyle.close();


    QFile treeSheet(":/dark/styles/helptreewidget.css");
    treeSheet.open(QFile::ReadOnly);
    QString treeStyleSheet = treeSheet.readAll();
    m_helpEngine->contentWidget()->setStyleSheet(treeStyleSheet);
    treeSheet.close();
}

void HelpWidget::goToHelpUrl(QUrl url)
{
    m_helpViewer->setSource(url);
    m_tabWidget->setCurrentWidget(m_helpViewer);
}

HelpWidget::~HelpWidget()
{
    m_helpEngine->deleteLater();
}

void HelpWidget::searchForWord(QString word)
{
    QMap<QString, QUrl> wordsFound = m_helpEngine->indexModel()->linksForKeyword(word);
    if (!wordsFound.isEmpty()) {
        goToHelpUrl(wordsFound["help"]);
    }
}
