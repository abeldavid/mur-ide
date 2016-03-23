#ifndef COMPILERSETTINGSWIDGET_H
#define COMPILERSETTINGSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>

#include "compilationoptionsview.h"
#include "sourcecompiler.h"
#include "settingsmanager.h"

class CompilerSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CompilerSettingsWidget(QWidget *parent = 0);
    void setCompilationOptions(const QStringList &options);
    QStringList compilerOptions();
    SettingsManager::TARGET currentTarget();
signals:
    void requireToSave();
    void requireToUpdateCompilationOptions();
private slots:
    void onCompilerChanged(int index);
public slots:
private:
    CompilationOptionsView* m_view;
    QComboBox* m_compilerBox;
    SettingsManager::TARGET m_target = SettingsManager::TARGET::EDISON;
};

#endif // COMPILERSETTINGSWIDGET_H
