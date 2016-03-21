#include "compilersettingswidget.h"
#include "compilationoptionsview.h"

#include <QLabel>
#include <QGridLayout>
#include <QDebug>
#include <QComboBox>
#include <QMessageBox>

CompilerSettingsWidget::CompilerSettingsWidget(QWidget *parent) :
    QWidget(parent),
    m_view(new CompilationOptionsView(this)),
    m_compilerBox(new QComboBox(this))

{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    layout->addWidget(m_compilerBox);
    layout->addWidget(m_view);

    m_compilerBox->addItem("Intel Edison GCC", "EDISON");
    m_compilerBox->addItem("Desktop MinGW", "MINGW");

    QObject::connect(m_compilerBox, SIGNAL(currentIndexChanged(int)), SLOT(onCompilerChanged(int)));
}

void CompilerSettingsWidget::setCompilationOptions(const QStringList &options)
{
    m_view->setOptions(options);
}

QStringList CompilerSettingsWidget::compilerOptions()
{
    return m_view->getOptions();
}

SourceCompiler::TARGET CompilerSettingsWidget::currentTarget()
{
    return m_target;
}

void CompilerSettingsWidget::onCompilerChanged(int index)
{
    if (m_view->isModified()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Сохранить?", "Настройки текущего компилятора не были сохранены. Сохнаить?",
                                    QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            emit requireToSave();
        }
        m_view->setModified(false);
    }

    if (index == 0) {
        m_target = SourceCompiler::TARGET::EDISON;
    }
    else if (index == 1) {
        m_target = SourceCompiler::TARGET::MINGW;
    }

    emit requireToUpdateCompilationOptions();
}
