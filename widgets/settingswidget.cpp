#include "settingswidget.h"
#include "settingsmanager.h"

#include <QGridLayout>
#include <QPushButton>

SettingsWidget::SettingsWidget(QWidget *parent) :
    QDialog(parent),
    m_tabWidget(new QTabWidget(this)),
    m_wifiPasswordWidget(new WiFiPasswordWidget(this)),
    m_compilerSettingsWidget(new CompilerSettingsWidget(this))
{
    //!Diable "?" button
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QGridLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    mainLayout->addWidget(m_tabWidget);

    QPushButton *applyButton = new QPushButton("Применить", this);
    mainLayout->addWidget(applyButton);

    setupTabs();

    QString ip = SettingsManager::instance().ipAddress();
    QString login = SettingsManager::instance().userLogin();
    QString password = SettingsManager::instance().userPassword();
    QString pscpPath = SettingsManager::instance().pscpPath();
    QString plinkPath = SettingsManager::instance().plinkPath();

    m_wifiPasswordWidget->setTxInfo(pscpPath, plinkPath);
    m_wifiPasswordWidget->setLoginInfo(ip, login, password);

    QObject::connect(m_compilerSettingsWidget, SIGNAL(requireToUpdateCompilationOptions()), this, SLOT(setupCompilationOptions()));
    QObject::connect(applyButton, SIGNAL(pressed()),this, SLOT(applySettings()));
    QObject::connect(m_compilerSettingsWidget, SIGNAL(requireToSave()), this, SLOT(applySettings()));

    setupCompilationOptions();
}

void SettingsWidget::applySettings()
{
    SettingsManager::instance().setIpAddress(m_wifiPasswordWidget->ip());
    SettingsManager::instance().setUserLogin(m_wifiPasswordWidget->login());
    SettingsManager::instance().setUserPassword(m_wifiPasswordWidget->password());

    if (m_compilerSettingsWidget->currentTarget() == SettingsManager::TARGET::EDISON) {
        SettingsManager::instance().setEdisonCompilerOptions(m_compilerSettingsWidget->compilerOptions());
    } else if (m_compilerSettingsWidget->currentTarget() == SettingsManager::TARGET::MINGW) {
        SettingsManager::instance().setMingwCompilerOptions(m_compilerSettingsWidget->compilerOptions());
    }
}

void SettingsWidget::setupCompilationOptions()
{
    QStringList compilerOptions;
    if (m_compilerSettingsWidget->currentTarget() == SettingsManager::TARGET::EDISON) {
        compilerOptions = SettingsManager::instance().edisonCompilerOptions();
    } else if (m_compilerSettingsWidget->currentTarget() == SettingsManager::TARGET::MINGW) {
        compilerOptions = SettingsManager::instance().mingwCompilerOptions();
    }
    m_compilerSettingsWidget->setCompilationOptions(compilerOptions);
}

void SettingsWidget::setupTabs()
{
    m_tabWidget->addTab(m_compilerSettingsWidget, "Компилятор");
    m_tabWidget->addTab(m_wifiPasswordWidget, "SSH");
}

