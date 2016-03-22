#include <QFile>
#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QtHelp/QHelpContentWidget>

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

    connect(m_helpEngine->contentWidget(),
            SIGNAL(linkActivated(QUrl)),
            this, SLOT(goToHelpUrl(QUrl)));

    mainLayout->addWidget(m_tabWidget);
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
